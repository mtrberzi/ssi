#include "world.h"
#include "voxel_occupant.h"
#include "transport_tube.h"
#include "time_constants.h"
#include <algorithm>
#include <deque>

World::World(uint32_t xd, uint32_t yd) : xDim(xd), yDim(yd) {
}

World::~World() {
	// delete all occupants
	for (VoxelOccupant *occ : all_occupants) {
		delete occ;
	}
}

bool World::location_in_bounds(const Vector position) const {
    if (position.getX() < 0 || position.getY() < 0 || position.getZ() < 0) return false;
    if ((uint32_t)position.getX() >= xDim || (uint32_t)position.getY() >= yDim) return false;
    return true;
}

std::unordered_set<VoxelOccupant*> World::get_occupants(const Vector position) const {
	if (!location_in_bounds(position)) {
		return std::unordered_set<VoxelOccupant*>();
	} else {
		std::unordered_map<Vector, std::unordered_set<VoxelOccupant*> >::const_iterator it;
		it = voxels.find(position);
		if (it == voxels.end()) {
			return std::unordered_set<VoxelOccupant*>();
		} else {
			return std::unordered_set<VoxelOccupant*>(it->second);
		}
	}
}

std::unordered_set<VoxelOccupant*> World::get_occupants(const Vector position, const Vector extents) const {
	std::unordered_set<VoxelOccupant*> allOccupants;
	for (int32_t x = position.getX(); x < position.getX() + extents.getX(); ++x) {
		for (int y = position.getY(); y < position.getY() + extents.getY(); ++y) {
			for (int z = position.getZ(); z < position.getZ() + extents.getZ(); ++z) {
				Vector v(x, y, z);
				std::unordered_set<VoxelOccupant*> occupants = get_occupants(v);
				allOccupants.insert(occupants.begin(), occupants.end());
			}
		}
	}
	return std::unordered_set<VoxelOccupant*>(allOccupants);
}

bool World::can_occupy(Vector position, const VoxelOccupant *obj) const {
	if (obj == NULL) return false;
	if (!location_in_bounds(position)) return false;

	for (int x = position.getX(); x < position.getX() + obj->get_extents().getX(); ++x) {
		for (int y = position.getY(); y < position.getY() + obj->get_extents().getY(); ++y) {
			for (int z = position.getZ(); z < position.getZ() + obj->get_extents().getZ(); ++z) {
				Vector v(x, y, z);
				if (!location_in_bounds(v)) return false;
				std::unordered_set<VoxelOccupant*> occupants = get_occupants(v);
				for (const VoxelOccupant *occ : occupants) {
					if (occ->impedesXYMovement() && occ->impedesZMovement()) {
						// cannot move there
						return false;
					}
					if (occ->is_transport_tube() && obj->is_transport_tube()) {
						TransportTube *tThis = (TransportTube*)obj;
						TransportTube *tThat = (TransportTube*)occ;
						// two transport tubes with the same transport ID cannot share a voxel
						if (tThis->get_transport_id() == tThat->get_transport_id()) {
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

bool World::add_occupant(Vector position, Vector subvoxelPosition, VoxelOccupant *obj) {
	if (!can_occupy(position, obj)) {
		return false;
	}
	// place the object;
	// track it in our list of all occupants, and add it to
	// every voxel that it occupies
	all_occupants.push_back(obj);

	for (int x = position.getX(); x < position.getX() + obj->get_extents().getX(); ++x) {
		for (int y = position.getY(); y < position.getY() + obj->get_extents().getY(); ++y) {
			for (int z = position.getZ(); z < position.getZ() + obj->get_extents().getZ(); ++z) {
				Vector v(x, y, z);
				voxels[v].insert(obj);
			}
		}
	}
    obj->set_position(position);
    obj->set_subvoxel_position(subvoxelPosition);
    return true;
}

void World::remove_occupant(VoxelOccupant *obj) {
	// delete from all_occupants
	all_occupants.erase(std::remove(all_occupants.begin(), all_occupants.end(), obj), all_occupants.end());
	// delete from all voxels it occupies
	for (int x = obj->get_position().getX(); x < obj->get_position().getX() + obj->get_extents().getX(); ++x) {
		for (int y = obj->get_position().getY(); y < obj->get_position().getY() + obj->get_extents().getY(); ++y) {
			for (int z = obj->get_position().getZ(); z < obj->get_position().getZ() + obj->get_extents().getZ(); ++z) {
				Vector v(x, y, z);
				std::unordered_set<VoxelOccupant*>::iterator it = voxels[v].find(obj);
				if (it != voxels[v].end()) {
					if (obj->is_transport_tube()) {
						remove_transport_tube((TransportTube*)obj);
					}
					voxels[v].erase(it);
				}
			}
		}
	}
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

/**
 * Traces a ray from the center of an origin voxel in a given direction
 * (with distances measured in voxels),
 * stopping when the voxel at (origin + direction) is reached
 * or when the resulting position would be outside of the world.
 *
 * This algorithm is based on an algorithm presented in the paper
 * "A Fast Voxel Traversal Algorithm for Ray Tracing",
 * by J. Amanatides and A. Woo, 1987
 * (http://www.cse.yorku.ca/~amana/research/grid.pdf)
 *
 * @param origin The position from which to start the trace
 * @param direction The translation vector along which to trace
 * @return A list of all voxels passed through by the ray,
 * in the order in which they are visited starting at origin,
 * not including the origin itself but including the endpoint
 * (unless the endpoint coincides with the origin)
 */
std::vector<Vector> World::raycast(Vector origin, Vector direction) const {
	std::vector<Vector> visitedVoxels;

	int x = origin.getX();
	int y = origin.getY();
	int z = origin.getZ();

	int dx = direction.getX();
	int dy = direction.getY();
	int dz = direction.getZ();

	int stepX = sgn(dx);
	int stepY = sgn(dy);
	int stepZ = sgn(dz);

	double tMaxX = intbound(x + 0.5, dx);
	double tMaxY = intbound(y + 0.5, dy);
	double tMaxZ = intbound(z + 0.5, dz);

	double tDeltaX = (double)(stepX) / (double)(dx);
	double tDeltaY = (double)(stepY) / (double)(dy);
	double tDeltaZ = (double)(stepZ) / (double)(dz);

	if (dx == 0 && dy == 0 && dz == 0) {
		return visitedVoxels;
	}

	Vector destination = origin + direction;
	while (true) {
		// find the next voxel we enter
		if (tMaxX < tMaxY) {
			if (tMaxX < tMaxZ) {
				x += stepX;
				tMaxX += tDeltaX;
			} else {
				z += stepZ;
				tMaxZ += tDeltaZ;
			}
		} else {
			if (tMaxY < tMaxZ) {
				y += stepY;
				tMaxY += tDeltaY;
			} else {
				z += stepZ;
				tMaxZ += tDeltaZ;
			}
		}
		Vector nextVoxel(x, y, z);
		if (!location_in_bounds(nextVoxel)) break;
		visitedVoxels.push_back(nextVoxel);
		if (nextVoxel == destination) break;
	} // while()

	return std::vector<Vector>(visitedVoxels);
}

Vector World::calculate_trajectory(VoxelOccupant *obj, std::vector<Vector> &traj) {
    std::deque<Vector> trajectory;
    // prime the queue with the current position so that staying still will
    // resolve gravity, etc.
    trajectory.push_back(obj->get_position());
    trajectory.insert(trajectory.end(), traj.begin(), traj.end());
    Vector currentPosition = obj->get_position();
    Vector nextPosition = currentPosition;
    while (!trajectory.empty()) {
        nextPosition = trajectory.front(); trajectory.pop_front();
        // based on what we so far believe to be the next position,
        // if at this point we can't actually move into that voxel coming from
        // that direction, we don't make any further moves
        // (checking each voxel that could be entered this way due to extents)
        Vector deltaP = nextPosition - currentPosition;
        bool moveXY = deltaP.getX() != 0 || deltaP.getY() != 0;
        bool moveZ = deltaP.getZ() != 0;
        bool canMove = true;

        for (VoxelOccupant *occ : get_occupants(currentPosition, obj->get_extents())) {
            // check for special stuff in our current position that can let us move differently
            // TODO occupant is ramp
            /*
             if (occ instanceof Ramp) {
              Ramp ramp = (Ramp)occ;
              Vector preferredDirection = ramp.getPreferredDirection();
              if (deltaP.equals(preferredDirection)) {
                // increase nextPosition.z by 1, we attempt to move up the ramp
                nextPosition = nextPosition.add(new Vector(0, 0, 1));
                // correct the rest of the trajectory to have z+1
                Supplier<Queue<Vector>> supplier = () -> new LinkedList<Vector>();
                Queue<Vector> newTrajectory = trajectory.stream()
                    .map((v) -> v.add(new Vector(0, 0, 1)))
                    .collect(Collectors.toCollection(supplier));
                trajectory = newTrajectory;
                break;
              }
            }
            */
        }
        // check for objects in the next voxel that prevent movement
        for (VoxelOccupant *occ : get_occupants(nextPosition, obj->get_extents())) {
            if ((moveXY && occ->impedesXYMovement()) || (moveZ && occ->impedesZMovement())) {
                canMove = false;
                break;
            }
        }
        if (!canMove) {
            // do not change currentPosition, we cannot make this move
            break;
        }
        // check to see whether any voxel in the target position + extents, only on the lowest z-level,
        // provides support
        if (obj->needsSupport()) {
            bool isSupported = false;
            Vector footprintSize(obj->get_extents().getX(), obj->get_extents().getY(), 1);
            for (VoxelOccupant *occ : get_occupants(nextPosition, footprintSize)) {
                if (occ->supportsOthers()) {
                    isSupported = true;
                    break;
                }
            }
            // if we're not supported, we might fall
            if (!isSupported) {
                // if there's anything below us that might impede our movement in the z-direction,
                // we cannot fall
                bool canMoveDown = true;
                // save a copy of this set as we will use it more than once
                std::unordered_set<VoxelOccupant*> occupantsBelowNewPosition
                    = get_occupants(nextPosition - Vector(0, 0, 1), footprintSize);
                for (VoxelOccupant *occ : occupantsBelowNewPosition) {
                    if (occ->impedesZMovement()) {
                        canMoveDown = false;
                        break;
                    }
                }
                if (canMoveDown) {
                    // now check to see whether any of the voxels below us contains a ramp
                    // that we are attempting to traverse in the reverse of its preferred direction;
                    // if this is the case, we move down one voxel and continue on our current trajectory
                    for (VoxelOccupant *occ : occupantsBelowNewPosition) {
                        // TODO occupant is ramp
                        /*
                        if (occ instanceof Ramp) {
                            Ramp ramp = (Ramp)occ;
                            Vector preferredDirection = ramp.getPreferredDirection();
                            if (deltaP.equals(preferredDirection.negate())) {
                                // decrease nextPosition.z by 1, we move down the ramp
                                nextPosition = nextPosition.subtract(new Vector(0, 0, 1));
                                // correct the rest of the trajectory to have z-1
                                Supplier<Queue<Vector>> supplier = () -> new LinkedList<Vector>();
                                Queue<Vector> newTrajectory = trajectory.stream()
                                  .map((v) -> v.subtract(new Vector(0, 0, 1)))
                                  .collect(Collectors.toCollection(supplier));
                                trajectory = newTrajectory;
                                break;
                            }
                        }
                        */
                    }
                    // TODO gravity and whatever else
                }
            }
        }
        // now the move was successful, so in the next iteration we start here
        currentPosition = nextPosition;
    }
    return currentPosition;
}

void World::timestep() {
    // build up a list of all objects that require pre-timestep processing
    // TODO maybe cache this?
    std::vector<VoxelOccupant*> preprocessList;
    for (auto elem : voxels) {
        std::unordered_set<VoxelOccupant*> &occupants = elem.second;
        for (VoxelOccupant *occupant : occupants) {
            if (occupant->requires_preprocessing()) {
                preprocessList.push_back(occupant);
            }
        }
    }

    // run processing for each occupant
    // TODO these can be run in parallel
    for (VoxelOccupant *proc : preprocessList) {
        proc->preprocess();
    }

    // perform timestep update
    // TODO maybe cache these too
    for (auto elem : voxels) {
        std::unordered_set<VoxelOccupant*> &occupants = elem.second;
        for (VoxelOccupant *occupant : occupants) {
            if (occupant->requires_timestep()) {
                occupant->timestep();
            }
        }
    }

    // perform world updates
    std::unordered_map<VoxelOccupant*, std::vector<WorldUpdate*>> worldUpdates;
    for (auto elem : voxels) {
        std::unordered_set<VoxelOccupant*> &occupants = elem.second;
        for (VoxelOccupant *occupant : occupants) {
            if (occupant->has_world_updates()) {
                worldUpdates[occupant] = occupant->get_world_updates();
            }
        }
    }
    for (auto entry : worldUpdates) {
        VoxelOccupant *obj = entry.first;
        std::vector<WorldUpdate*> &updates = entry.second;
        std::unordered_map<WorldUpdate*, WorldUpdateResult> results;
        for (WorldUpdate* update : updates) {
            WorldUpdateResult result = update->apply(this);
            results.insert(std::unordered_map<WorldUpdate*, WorldUpdateResult>::value_type(update, result));
        }
        obj->collect_update_results(results);
    }

    std::unordered_set<VoxelOccupant*> movingObjects;
    Vector zeroVector(0,0,0);
    for (auto elem : voxels) {
        std::unordered_set<VoxelOccupant*> &occupants = elem.second;
        for (VoxelOccupant *occupant : occupants) {
            // if velocity component is non-zero, the object is moving
            if (!(occupant->get_subvoxel_velocity() == zeroVector)
                    || !(occupant->get_velocity() == zeroVector)) {
                movingObjects.insert(occupant);
            }
        }
    }

    // perform movement updates
    // TODO this could potentially be parallelized, but updating the map would require locking/concurrent data structures
    // at least as much as calculating new positions could be done in parallel
    for (VoxelOccupant *obj : movingObjects) {
        // calculate new position
        Vector newPos = obj->get_position() + obj->get_velocity();
        int32_t newX = newPos.getX();
        int32_t newY = newPos.getY();
        int32_t newZ = newPos.getZ();
        Vector newSVPos = obj->get_subvoxel_position() + obj->get_subvoxel_velocity();
        int32_t newX_sv = newSVPos.getX();
        int32_t newY_sv = newSVPos.getY();
        int32_t newZ_sv = newSVPos.getZ();

        // determine whether we've ended up in a new voxel

        if (newX_sv <= -SUBVOXELS_PER_VOXEL) {
            newX_sv += SUBVOXELS_PER_VOXEL;
            --newX;
        } else if (newX_sv >= SUBVOXELS_PER_VOXEL) {
            newX_sv -= SUBVOXELS_PER_VOXEL;
            ++newX;
        }

        if (newY_sv <= -SUBVOXELS_PER_VOXEL) {
            newY_sv += SUBVOXELS_PER_VOXEL;
            --newY;
        } else if (newY_sv >= SUBVOXELS_PER_VOXEL) {
            newY_sv -= SUBVOXELS_PER_VOXEL;
            ++newY;
        }

        if (newZ_sv <= -SUBVOXELS_PER_VOXEL) {
            newZ_sv += SUBVOXELS_PER_VOXEL;
            --newZ;
        } else if (newZ_sv >= SUBVOXELS_PER_VOXEL) {
            newZ_sv -= SUBVOXELS_PER_VOXEL;
            ++newZ;
        }

        newPos = Vector(newX, newY, newZ);
        newSVPos = Vector(newX_sv, newY_sv, newZ_sv);

        std::vector<Vector> visitedPositions = raycast(obj->get_position(), newPos - obj->get_position());
        newPos = calculate_trajectory(obj, visitedPositions);
        // now newPos and newSVPos are correct;
        // we remove the object from its old position and add it to its new one
        remove_occupant(obj);
        add_occupant(newPos, newSVPos, obj);
    }
}

// TODO completion of all World member functions

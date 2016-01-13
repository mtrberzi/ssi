#include "world.h"
#include "voxel_occupant.h"
#include "transport_tube.h"
#include <algorithm>

// TODO completion of all World member functions

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
    if (position.getX() >= xDim || position.getY() >= yDim) return false;
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

bool World::can_occupy(const Vector position, const VoxelOccupant *obj) const {
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
std::vector<Vector> World::raycast(const Vector origin, const Vector direction) const {
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


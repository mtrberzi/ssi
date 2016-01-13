#include "world.h"
#include "voxel_occupant.h"
#include "transport_tube.h"

// TODO completion of all World member functions

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

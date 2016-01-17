#include "voxel_occupant.h"
#include "world_updates.h"

VoxelOccupant::VoxelOccupant()
: uuid(), position(0,0,0), subvoxelPosition(0,0,0),
  velocity(0,0,0), subvoxelVelocity(0,0,0),
  durability(1) {
}

std::vector<WorldUpdate> VoxelOccupant::on_destroy() {
	std::vector<WorldUpdate> updates;
	updates.push_back(RemoveObjectUpdate(this));
	return updates;
}

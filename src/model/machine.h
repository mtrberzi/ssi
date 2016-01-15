#ifndef _MODEL_MACHINE_H_
#define _MODEL_MACHINE_H_

#include "voxel_occupant.h"
#include <cstdint>

/**
 * A Machine is a voxel occupant that represents a mechanical device or robot.
 * Machines cannot be directly manipulated by robots, but can be manipulated indirectly.
 * Machines may occupy one or more voxels.
 * Machines can interface with inventory controllers in order to manipulate items
 * in their environment.
 */

class Machine : public VoxelOccupant {
    // TODO Machine class
};

#endif // _MODEL_MACHINE_H_

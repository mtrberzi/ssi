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


    virtual uint16_t get_kind() const { return 3; }
    // TODO machine durability
    virtual int32_t get_maximum_durability() const { return 1; }
    virtual bool requires_timestep() const { return true; }
};

#endif // _MODEL_MACHINE_H_

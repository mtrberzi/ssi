#ifndef _MODEL_VOXEL_OCCUPANT_
#define _MODEL_VOXEL_OCCUPANT_

#include <cstdint>
#include "world.h"
#include "vector.h"
#include "uuid.h"
#include <vector>
#include <unordered_map>
#include <cmath>
#include "material.h"

class VoxelOccupant {
public:
	VoxelOccupant();
	virtual ~VoxelOccupant() {}

	// when UUIDs are transferred to and from the microcontroller,
	// the least significant long always appears at the lower address in memory
	UUID get_uuid() const { return uuid; }
	void set_uuid(UUID & u) { uuid = u; }

	virtual bool impedesXYMovement() const = 0;
	virtual bool impedesZMovement() const = 0;
	virtual bool impedesXYFluidFlow() const = 0;
	virtual bool impedesZFluidFlow() const = 0;
	virtual bool supportsOthers() const = 0;
	virtual bool needsSupport() const = 0;
	virtual bool canMove() const = 0;

	virtual uint16_t get_kind() const = 0;
	virtual uint32_t get_type() const = 0; // more specific than kind, but sometimes unavailable

	Vector get_position() const { return position; }
	void set_position(Vector & p ) { position = p; }
	// invariant: each component of subvoxelPosition is at least 0
	// and smaller than TimeConstants.SUBVOXELS_PER_VOXEL
	Vector get_subvoxel_position() const { return subvoxelPosition; }
	void set_subvoxel_position(Vector & p) { subvoxelPosition = p; }

	// gets the region of space occupied by this thing
	virtual Vector get_extents() const = 0;

	Vector get_velocity() const { return velocity; }
	void set_velocity(Vector & v) {	velocity = v; }

	// invariant: the absolute value of each component of
	// subvoxelVelocity is no larger than TimeConstants.SUBVOXELS_PER_VOXEL
	Vector get_subvoxel_velocity() const { return subvoxelVelocity; }
	void set_subvoxel_velocity(Vector & v) { subvoxelVelocity = v; }

	// true iff the object has stuff to do before the timestep update,
	// e.g. running a processor
	virtual bool requires_preprocessing() const { return false; }
	virtual void preprocess() {}

	virtual bool requires_timestep() const = 0;
	virtual void timestep() {}

	virtual bool has_world_updates() const = 0;
	// this method is always called after the call to timestep(),
	// but only if hasWorldUpdates() is true
	virtual std::vector<WorldUpdate*> get_world_updates() {
		return std::vector<WorldUpdate*>();
	}
	virtual void collect_update_results(std::unordered_map<WorldUpdate*, WorldUpdateResult> & results) {}

	// really, really simplistic "damage" system
	virtual int32_t get_current_durability() const { return durability; }
	virtual void set_current_durability(int d) { durability = d; }
	virtual int32_t get_maximum_durability() const = 0;

	virtual std::vector<WorldUpdate> on_destroy();

	virtual bool is_transport_tube() const { return false; }

protected:
	UUID uuid;
	Vector position;
	Vector subvoxelPosition;
	Vector velocity;
	Vector subvoxelVelocity;

	int32_t durability;
};

/**
 * An Item is a voxel occupant that represents a tool, raw material, or part.
 * They are always made out of a single material.
 * Items can be directly manipulated by robots and machines,
 * and carried by conveyors. Items always occupy
 * exactly one voxel, cannot move on their own, and must be supported.
 */

class Item : public VoxelOccupant {
public:
    Item(Material *mat) : material(mat) {
        set_current_durability(get_maximum_durability());
    }
    virtual ~Item(){}
    Material *get_material() const { return material; }

    virtual bool impedesXYMovement() const { return false; }
    virtual bool impedesZMovement() const { return false; }
    virtual bool impedesXYFluidFlow() const { return false; }
    virtual bool impedesZFluidFlow() const { return false; }
    virtual bool supportsOthers() const { return false; }
    virtual bool needsSupport() const { return true; }
    virtual bool canMove() const { return false; }
    virtual Vector get_extents() const { return Vector(1,1,1); }

    virtual bool requires_timestep() const { return false; }
    virtual bool has_world_updates() const { return false; }

    // TODO item durability
    virtual int32_t get_maximum_durability() const { return 1; }

    virtual bool is_ore() const { return false; }
    virtual bool is_component() const { return false; }
protected:
    Material *material;
};

// Machine is defined in machine.h/machine.cc

/**
 * A Structure is a voxel occupant that typically represents a
 * terrain feature, building piece, or other non-mechanical large object.
 * Structures are always made out of a single material and cannot move on their own.
 * They also cannot be directly manipulated by robots, but can be manipulated
 * indirectly (e.g. mining out a block or placing a floor tile).
 * Structures always occupy exactly one voxel.
 * Every structure has a base durability, which is modified by the material's durability factor
 * to compute the structure's maximum durability.
 */

class Structure : public VoxelOccupant {
public:
    Structure(Material *m) : material(m) {
        set_current_durability(get_maximum_durability());
    }
    virtual ~Structure() {}
    Material *get_material() const { return material; }

    virtual Vector get_extents() const { return Vector(1,1,1); }
    virtual bool canMove() const { return false; }
    virtual bool requires_timestep() const { return false; }
    virtual bool has_world_updates() const { return false; }
    virtual uint16_t get_kind() const { return 4; }
    virtual int32_t get_maximum_durability() const {
        return (int32_t)floor((double)get_base_durability() * get_material()->get_durability_modifier());
    }

protected:
    Material *material;
    virtual int32_t get_base_durability() const = 0;
};

// TransportTube is defined in transport_tube.h/transport_tube.cc

#endif // _MODEL_VOXEL_OCCUPANT_

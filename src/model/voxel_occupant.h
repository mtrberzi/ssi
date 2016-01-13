#ifndef _MODEL_VOXEL_OCCUPANT_
#define _MODEL_VOXEL_OCCUPANT_

#include <cstdint>
#include "world.h"
#include "vector.h"
#include "uuid.h"
#include <vector>
#include <unordered_map>

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

	virtual uint16_t get_kind() = 0;
	virtual uint32_t get_type() = 0; // more specific than kind, but sometimes unavailable

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
	virtual std::vector<WorldUpdate> get_world_updates() {
		return std::vector<WorldUpdate>();
	}
	virtual void collect_update_results(std::unordered_map<WorldUpdate, WorldUpdateResult> & results) {}

	// really, really simplistic "damage" system
	virtual int32_t get_current_durability() const { return durability; }
	virtual void set_current_durability(int d) { durability = d; }
	virtual int32_t get_maximum_durability() const = 0;

	virtual std::vector<WorldUpdate> on_destroy();

protected:
	UUID uuid;
	Vector position;
	Vector subvoxelPosition;
	Vector velocity;
	Vector subvoxelVelocity;

	int32_t durability;
};

#endif // _MODEL_VOXEL_OCCUPANT_

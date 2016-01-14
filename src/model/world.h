#ifndef _MODEL_WORLD_
#define _MODEL_WORLD_

#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <vector>
#include <cmath>
#include "vector.h"

class VoxelOccupant;
class TransportTube;

class WorldUpdateResult {
public:
	WorldUpdateResult(bool success) : succeeded(success) {}
	~WorldUpdateResult() {}

	bool was_successful() const { return succeeded; }
protected:
	bool succeeded;
};

class World {
public:
	World(uint32_t xd, uint32_t yd);
	~World();

	uint32_t get_xDim() const { return xDim; }
	uint32_t get_yDim() const { return yDim; }

	bool location_in_bounds(const Vector position) const;
	std::unordered_set<VoxelOccupant*> get_occupants(const Vector position) const;
	std::unordered_set<VoxelOccupant*> get_occupants(const Vector position, const Vector extents) const;
	bool can_occupy(const Vector position, const VoxelOccupant *occupant) const;
	bool add_occupant(Vector position, Vector subvoxelPosition, VoxelOccupant *obj);
	void remove_occupant(VoxelOccupant *obj);

	/*
	 * Traces a ray from the center of an origin voxel in a given direction
	 * (with distances measured in voxels),
	 * stopping when the voxel at (origin + direction) is reached
	 * or when the resulting position would be outside of the world.
	 */
	std::vector<Vector> raycast(const Vector origin, const Vector direction) const;

	/*
	 * Computes the rest position of `obj` as it moves through each voxel of `traj` in turn.
	 * Returns the final resting position of `obj`.
	 */
	Vector calculate_trajectory(VoxelOccupant *obj, std::vector<Vector> &traj);

	void timestep();

protected:
	std::unordered_map<Vector, std::unordered_set<VoxelOccupant*> > voxels;
	uint32_t xDim;
	uint32_t yDim;

	std::vector<VoxelOccupant*> all_occupants; // for memory management

	void create_bedrock_layer();

	void remove_transport_tube(TransportTube *transport);

	// Find the smallest positive t such that s + t*ds is an integer
	double intbound(double s, double ds) const {
		if (ds < 0.0) {
			return intbound(-s, -ds);
		} else {
			s = fmod(s, 1.0);
			// now we have s + t*ds = 1
			return (1.0 - s) / ds;
		}
	}
};

class WorldUpdate {
public:
	WorldUpdate() {}
	virtual ~WorldUpdate() {}

	virtual WorldUpdateResult apply(World *w);
};

#endif // _MODEL_WORLD_

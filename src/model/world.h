#ifndef _MODEL_WORLD_
#define _MODEL_WORLD_

#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <vector>
#include <cmath>
#include "vector.h"
#include "voxel_occupant.h"
#include "transport_tube.h"

class World {
public:
	World();
	~World();

	bool location_in_bounds(const Vector position) const;
	void get_occupants(const Vector position, std::unordered_set<VoxelOccupant*> &occupant_set);
	void get_occupants(const Vector position, const Vector extends, std::unordered_set<VoxelOccupant*> &occupant_set);
	bool can_occupy(const Vector position, const VoxelOccupant *occupant) const;
	bool add_occupant(const Vector position, const Vector subvoxelPosition, VoxelOccupant *obj);
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

#endif // _MODEL_WORLD_

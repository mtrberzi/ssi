#ifndef _MODEL_TRANSPORT_TUBE_
#define _MODEL_TRANSPORT_TUBE_

#include "voxel_occupant.h"
#include <cstdint>

class TransportTube : public VoxelOccupant {
public:
	TransportTube(uint32_t txID);
	virtual ~TransportTube();

	bool is_transport_tube() const { return true; }
	uint32_t get_transport_id() const { return transport_id; }

protected:
	uint32_t transport_id;
};

#endif // _MODEL_TRANSPORT_TUBE_

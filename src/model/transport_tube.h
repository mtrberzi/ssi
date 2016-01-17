#ifndef _MODEL_TRANSPORT_TUBE_
#define _MODEL_TRANSPORT_TUBE_

#include "voxel_occupant.h"
#include "transport_device.h"
#include <cstdint>
#include <vector>

class TransportTube : public VoxelOccupant, public TransportDevice {
public:
	TransportTube(uint32_t txID);
	virtual ~TransportTube();

	bool is_transport_tube() const { return true; }
	uint32_t get_transport_id() const { return transport_id; }

	virtual uint16_t get_kind() const { return 6; }
	virtual uint32_t get_type() const { return transport_id; }
	virtual int32_t get_maximum_durability() const { return 1; }
	virtual bool impedesXYMovement() const { return false; }
	virtual bool impedesZMovement() const { return false; }
	virtual bool impedesXYFluidFlow() const { return false; }
	virtual bool impedesZFluidFlow() const { return false; }
	virtual bool supportsOthers() const { return false; }
	virtual bool needsSupport() const { return false; }
	virtual bool canMove() const { return false; }
	virtual Vector get_extents() const { return Vector(1,1,1); }
	// Transport tubes are handled specially by the World and so do not individually receive timesteps.
	virtual bool requires_timestep() const { return false; }
	virtual bool has_world_updates() const { return false; }

	TransportDevice *get_connectionA() const { return connectionA; }
	TransportDevice *get_connectionB() const { return connectionB; }

	uint32_t get_number_of_connected_devices() const {
	    uint32_t count = 0;
	    if (connectionA != NULL) ++count;
	    if (connectionB != NULL) ++count;
	    return count;
	}

	void connect(TransportDevice *other);
	virtual void disconnect(TransportDevice *connected);
	Item *get_outgoing_to_A() const { return outgoingToA; }
	Item *get_outgoing_to_B() const { return outgoingToB; }
	std::vector<Item*> get_contents() const;
	virtual bool receive(TransportDevice *neighbour, Item *item);
protected:
	uint32_t transport_id;
	TransportDevice *connectionA;
	TransportDevice *connectionB;
	Item *outgoingToA;
	Item *outgoingToB;
};

#endif // _MODEL_TRANSPORT_TUBE_

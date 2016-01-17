#ifndef _MODEL_WORLD_UPDATES_
#define _MODEL_WORLD_UPDATES_

#include "world.h"
#include "voxel_occupant.h"
#include "transport_endpoint.h"
#include "transport_tube.h"

// TODO complete world updates header

class ConnectTransportEndpointUpdate : public WorldUpdate {
public:
    ConnectTransportEndpointUpdate(uint32_t txID, Vector pos, TransportEndpoint *ept, uint32_t eptID)
        : transportID(txID), position(pos), endpoint(ept), endpointID(eptID) {}
    ~ConnectTransportEndpointUpdate(){}
    WorldUpdateResult apply(World *w);
protected:
    uint32_t transportID;
    Vector position;
    TransportEndpoint *endpoint;
    uint32_t endpointID;
};

class ConnectTransportTubeUpdate : public WorldUpdate {
public:
    ConnectTransportTubeUpdate(uint32_t txID, Vector pos1, Vector pos2) : transportID(txID), position1(pos1), position2(pos2) {}
    ~ConnectTransportTubeUpdate() {}
    WorldUpdateResult apply(World *w);
protected:
    uint32_t transportID;
    Vector position1;
    Vector position2;
};

class CreateTransportTubeUpdate : public WorldUpdate {
public:
    CreateTransportTubeUpdate(Vector position, uint32_t txID) : position(position), transportID(txID) {}
    ~CreateTransportTubeUpdate() {}
    WorldUpdateResult apply(World *w);
protected:
    Vector position;
    uint32_t transportID;
};

class MiningLaserUpdate : public WorldUpdate {

};

class PutWithManipulatorUpdate : public WorldUpdate {

};

class ReduceDurabilityUpdate : public WorldUpdate {

};

class RelativeAddObjectUpdate : public WorldUpdate {

};

class RelativeRemoveObjectUpdate : public WorldUpdate {

};

class RemoveObjectUpdate : public WorldUpdate {
public:
	RemoveObjectUpdate(VoxelOccupant * tgt) : target(tgt) {}
	~RemoveObjectUpdate() {}
	WorldUpdateResult apply(World *w);
protected:
	VoxelOccupant *target;
};

class TakeWithManipulatorByUUIDUpdate : public WorldUpdate {

};



#endif // _MODEL_WORLD_UPDATES_

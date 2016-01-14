#ifndef _MODEL_WORLD_UPDATES_
#define _MODEL_WORLD_UPDATES_

#include "world.h"
#include "voxel_occupant.h"

// TODO complete world updates header

class ConnectTransportEndpointUpdate : public WorldUpdate {

};

class ConnectTransportTubeUpdate : public WorldUpdate {

};

class CreateTransportTubeUpdate : public WorldUpdate {

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
	WorldUpdateResult apply(World &w);
protected:
	VoxelOccupant *target;
};

class TakeWithManipulatorByUUIDUpdate : public WorldUpdate {

};



#endif // _MODEL_WORLD_UPDATES_

#ifndef _MACHINES_SMELTER_
#define _MACHINES_SMELTER_

#include <cstdint>
#include <queue>
#include "transport_endpoint.h"
#include "ore.h"

/*
 * A Smelter is a machine that refines smeltable ore into bars of material
 * that can then be used to produce other items.
 *
 * Endpoints: 0=input, 1=output
 */

class Smelter : public TransportEndpoint {
public:
	Smelter();
	virtual ~Smelter();

	virtual std::unordered_set<uint32_t> get_transport_endpoints() const {
		return std::unordered_set<uint32_t>{0,1};
	}
	virtual Vector get_extents() const {
		return Vector(1,1,1);
	}
	virtual bool has_world_updates() const {
		return false;
	}
	virtual bool receive_to_endpoint(uint32_t eptID, Item *item);
	virtual uint32_t get_type() const { return 1; }

	Ore* get_current_ore() const { return currentOre;}
	uint32_t get_smelting_time_remaining() const { return smeltingTimeLeft; }
protected:
	enum SmelterState {
		STATE_LOAD, // waiting to receive ore
		STATE_SMELT, // smelting ore
		STATE_OUTPUT // waiting to output bars
	};
	SmelterState state;
	Ore *currentOre;
	uint32_t smeltingTimeLeft;
	std::queue<Item*> outputQueue;

	virtual void pre_send_timestep();
	virtual void post_send_timestep(std::unordered_map<uint32_t,bool> results);
};

#endif // _MACHINES_SMELTER_

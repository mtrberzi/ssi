#ifndef _TESTUTIL_ENDPOINTS_
#define _TESTUTIL_ENDPOINTS_

#include "transport_endpoint.h"
#include <cstdint>
#include <queue>

// Defines transport endpoints useful for testing other devices.

// A transport device that attempts to send an item through endpoint 0 continuously.
class TestUtilSourceEndpoint : public TransportEndpoint {
public:
	TestUtilSourceEndpoint();
	virtual ~TestUtilSourceEndpoint();

	virtual uint32_t get_type() const { return 0; }
	virtual Vector get_extents() const { return Vector(1,1,1); }
	virtual bool has_world_updates() const { return false; }
	virtual std::unordered_set<uint32_t> get_transport_endpoints() const { return std::unordered_set<uint32_t>{0}; }
	virtual bool receive_to_endpoint(uint32_t endpoint, Item *item) { return false; }

	std::queue<Item*> get_send_queue() const { return sendQueue; }
	void queue_send(Item *i) { sendQueue.push(i); }

	uint32_t get_send_period() const { return period; }
	void set_send_period(uint32_t p) { period = p; }
	void reset_send_counter() { counter = 0; }
protected:
	std::queue<Item*> sendQueue;
	uint32_t period;
	uint32_t counter;
	virtual void pre_send_timestep();
	virtual void post_send_timestep(std::unordered_map<uint32_t, bool> results);
};

#endif // _TESTUTIL_ENDPOINTS_

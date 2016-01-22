#include "testutil_endpoints.h"

TestUtilSourceEndpoint::TestUtilSourceEndpoint()
: period(1), counter(0)
{}

TestUtilSourceEndpoint::~TestUtilSourceEndpoint() {
	// TODO delete all items
}

void TestUtilSourceEndpoint::pre_send_timestep() {
	counter += 1;
	if (counter == period) {
		counter = 0;
		if (!sendQueue.empty()) {
			set_endpoint_output(0, sendQueue.front());
		}
	}
}

void TestUtilSourceEndpoint::post_send_timestep(std::unordered_map<uint32_t, bool> results) {
	for (auto entry : results) {
		if (entry.second) {
			sendQueue.pop();
		}
	}
}

TestUtilSinkEndpoint::TestUtilSinkEndpoint() {}

TestUtilSinkEndpoint::~TestUtilSinkEndpoint() {
	// TODO delete all items
}

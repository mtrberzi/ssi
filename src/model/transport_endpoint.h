#ifndef _MODEL_TRANSPORT_ENDPOINT_
#define _MODEL_TRANSPORT_ENDPOINT_

#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include "machine.h"
#include "transport_device.h"
#include "transport_endpoint.h"

// A TransportEndpoint is a kind of machine that connects to TransportTubes.
// It is always stationary.

class TransportEndpoint : public Machine, public TransportDevice {
public:
    TransportEndpoint() {}
    // TODO TransportEndpoint(Microcontroller)
    virtual ~TransportEndpoint() {}

    virtual bool impedesXYMovement() const { return false; }
    virtual bool impedesZMovement() const { return false; }
    virtual bool impedesXYFluidFlow() const { return false; }
    virtual bool impedesZFluidFlow() const { return false; }
    virtual bool supportsOthers() const { return false; }
    virtual bool needsSupport() const { return true; }
    virtual bool canMove() const { return false; }
    virtual void timestep();

    bool is_valid_endpoint(uint32_t endpointID) const;
    bool is_connected(uint32_t endpointID) const;
    TransportTube *get_connected_transport(uint32_t endpointID) const;
    uint32_t get_endpoint_id(TransportTube *transport) const;
    bool connect(uint32_t endpointID, TransportTube *transport);
    virtual void disconnect(TransportDevice *transport);
    virtual bool receive(TransportDevice *neighbour, Item *item);

    // Enumerates all legal transport endpoint IDs.
    virtual std::unordered_set<uint32_t> get_transport_endpoints() const = 0;
    // Callback for when an item is inbound to an endpoint.
    // Returns true iff the item could successfully be received.
    virtual bool receive_to_endpoint(uint32_t endpointID, Item *item) = 0;
protected:
    std::unordered_map<uint32_t, TransportTube*> connectedTransports;
    std::unordered_map<uint32_t, Item*> endpointOutputs;

    bool send(uint32_t endpointID, Item *item);
    void set_endpoint_output(uint32_t endpointID, Item *item) {
        endpointOutputs[endpointID] = item;
    }

    virtual void pre_send_timestep() {}
    virtual void post_send_timestep(std::unordered_map<uint32_t, bool> send_results) {}
};

#endif // _MODEL_TRANSPORT_ENDPOINT_

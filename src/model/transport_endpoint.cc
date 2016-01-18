#include "transport_endpoint.h"
#include "transport_tube.h"

bool TransportEndpoint::is_valid_endpoint(uint32_t endpointID) const {
    auto epts = get_transport_endpoints();
    return epts.find(endpointID) != epts.end();
}

bool TransportEndpoint::is_connected(uint32_t endpointID) const {
    if (!is_valid_endpoint(endpointID)) return false;
    return connectedTransports.find(endpointID) != connectedTransports.end();
}

TransportTube *TransportEndpoint::get_connected_transport(uint32_t endpointID) const {
    if (!is_valid_endpoint(endpointID)) return NULL;
    auto entry = connectedTransports.find(endpointID);
    if (entry == connectedTransports.end()) {
        return NULL;
    } else {
        return entry->second;
    }
}

uint32_t TransportEndpoint::get_endpoint_id(TransportTube *transport) const {
    for (auto entry : connectedTransports) {
        if (entry.second == transport) {
            return entry.first;
        }
    }
    // not found
    // this should almost always be illegal
    return UINT32_MAX;
}

bool TransportEndpoint::connect(uint32_t endpointID, TransportTube *transport) {
    if (!is_valid_endpoint(endpointID)) return false;
    for (auto entry : connectedTransports) {
        if (entry.first == endpointID) return false;
        if (entry.second == transport) return false;
    }
    connectedTransports[endpointID] = transport;
    return true;
}

void TransportEndpoint::disconnect(TransportDevice *dev) {
    if (!(dev->is_transport_tube())) return;
    TransportTube *transport = (TransportTube*)dev;
    uint32_t eptID = get_endpoint_id(transport);
    // double-check
    if (connectedTransports[eptID] == transport) {
        connectedTransports.erase(eptID);
    }
}

bool TransportEndpoint::receive(TransportDevice *neighbour, Item *item) {
    if (neighbour->is_transport_tube()) {
        // if the item is null, it is an empty slot; absorb it
        if (item == NULL) {
            return true;
        } else {
            uint32_t endpointID = get_endpoint_id((TransportTube*)neighbour);
            return receive_to_endpoint(endpointID, item);
        }
    } else {
        // cannot receive from non-tube transport
        return false;
    }
}

bool TransportEndpoint::send(uint32_t endpointID, Item *item) {
    TransportTube *tx = get_connected_transport(endpointID);
    if (tx == NULL) {
        return false;
    } else {
        return tx->receive(this, item);
    }
}

void TransportEndpoint::timestep() {
    pre_send_timestep();
    // now endpointOutputs is populated
    std::unordered_map<uint32_t, bool> sendResults;
    for (uint32_t endpoint : get_transport_endpoints()) {
        auto entry = endpointOutputs.find(endpoint);
        if (entry != endpointOutputs.end()) {
            bool couldSend = send(endpoint, entry->second);
            sendResults[endpoint] = couldSend;
        } else {
            // push an empty slot to advance the pipeline
            send(endpoint, NULL);
        }
    }
    endpointOutputs.clear();
    post_send_timestep(sendResults);
}

#include "world_updates.h"

static TransportTube *find_transport(World *w, Vector position, uint32_t transportID) {
    auto occupants = w->get_occupants(position);
    for (VoxelOccupant *occ : occupants) {
        if (!(occ->is_transport_tube())) continue;
        TransportTube *transport = (TransportTube*)occ;
        if (transport->get_transport_id() == transportID) {
            return transport;
        }
    }
    return NULL;
}

WorldUpdateResult ConnectTransportEndpointUpdate::apply(World *w) {
    // TODO ConnectTransportEndpointUpdate::apply()
    return new WorldUpdateResult(false);
}

WorldUpdateResult ConnectTransportTubeUpdate::apply(World *w) {
    // TODO make sure the two positions are adjacent
    // look for transport tubes with this transportID in first/second positions
    TransportTube *first = find_transport(w, position1, transportID);
    TransportTube *second = find_transport(w, position2, transportID);
    if (first == NULL || second == NULL) {
      return new WorldUpdateResult(false);
    }
    // make sure that <2 connections per transport
    if (first->get_number_of_connected_devices() > 1 || second->get_number_of_connected_devices() > 1) {
      return new WorldUpdateResult(false);
    }
    // connect each to the other
    first->connect(second);
    second->connect(first);
    return new WorldUpdateResult(true);
}

WorldUpdateResult CreateTransportTubeUpdate::apply(World *w) {
    TransportTube *transport = new TransportTube(transportID);
    if (w->can_occupy(position, transport)) {
        bool result = w->add_occupant(position, Vector(0,0,0), transport);
        if (result) {
            return WorldUpdateResult(true);
        } else {
            delete transport;
            return WorldUpdateResult(false);
        }
    } else {
        delete transport;
        return WorldUpdateResult(false);
    }
}

WorldUpdateResult RemoveObjectUpdate::apply(World *w) {
	w->remove_occupant(target);
	return WorldUpdateResult(true);
}

// TODO complete world updates implementation

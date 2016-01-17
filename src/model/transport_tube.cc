#include "transport_tube.h"

TransportTube::TransportTube(uint32_t txID)
: transport_id(txID), connectionA(NULL), connectionB(NULL),
  outgoingToA(NULL), outgoingToB(NULL) {
}

TransportTube::~TransportTube() {
    // absolutely don't delete our outgoing items;
    // assume they were taken from us by the World
}

void TransportTube::connect(TransportDevice *other) {
    if (connectionA == NULL) {
        connectionA = other;
    } else if (connectionB == NULL) {
        connectionB = other;
    } else {
        // cannot connect more than two devices
    }
}

void TransportTube::disconnect(TransportDevice *other) {
    if (connectionA == other) {
        connectionA = NULL;
    } else if (connectionB == other) {
        connectionB = NULL;
    } else {
        // do nothing
    }
}

std::vector<Item*> TransportTube::get_contents() const {
    if (outgoingToA == NULL && outgoingToB == NULL) {
        return std::vector<Item*>();
    } else if (outgoingToA != NULL && outgoingToB == NULL) {
        return std::vector<Item*>{outgoingToA};
    } else if (outgoingToA == NULL && outgoingToB != NULL) {
        return std::vector<Item*>{outgoingToB};
    } else {
        return std::vector<Item*>{outgoingToA, outgoingToB};
    }
}

bool TransportTube::receive(TransportDevice *neighbour, Item *item) {
    if (connectionA != NULL && neighbour == connectionA) {
        // first try to send outgoingToB to connectionB
        if (connectionB == NULL) {
            if (item == NULL) {
                // absorb the empty slot
                return true;
            } else {
                if (outgoingToB == NULL) {
                    // can't send, but we can receive it for now
                    outgoingToB = item;
                    return true;
                } else {
                    // cannot replace existing outgoing item
                    return false;
                }
            }
        } else {
            // first try to send whatever we have to B
            bool sendResult = connectionB->receive(this, outgoingToB);
            if (sendResult) {
                // now our slot is empty so we can always receive
                outgoingToB = item;
                return true;
            } else {
                if (item == NULL) {
                    // absorb the empty slot
                    return true;
                } else {
                    if (outgoingToB == NULL) {
                        // can't send, but we can receive it for now
                        outgoingToB = item;
                        return true;
                    } else {
                        // cannot replace existing outgoing item
                        return false;
                    }
                }
            }
        }
    } else if (connectionB != NULL && neighbour == connectionB) {
        // first try to send outgoingToA to connectionA
        if (connectionA == NULL) {
            if (item == NULL) {
                // absorb the empty slot
                return true;
            } else {
                if (outgoingToA == NULL) {
                    // can't send, but we can receive it for now
                    outgoingToA = item;
                    return true;
                } else {
                    // cannot replace existing outgoing item
                    return false;
                }
            }
        } else {
            // first try to send whatever we have to A
            bool sendResult = connectionA->receive(this, outgoingToA);
            if (sendResult) {
                // now our slot is empty so we can always receive
                outgoingToA = item;
                return true;
            } else {
                if (item == NULL) {
                    // absorb the empty slot
                    return true;
                } else {
                    if (outgoingToA == NULL) {
                        // can't send, but we can receive it for now
                        outgoingToA = item;
                        return true;
                    } else {
                        // cannot replace existing outgoing item
                        return false;
                    }
                }
            }
        }
    } else {
        // do nothing; cannot receive from an unconnected device
        return false;
    }
}

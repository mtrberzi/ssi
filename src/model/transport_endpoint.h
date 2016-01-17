#ifndef _MODEL_TRANSPORT_ENDPOINT_
#define _MODEL_TRANSPORT_ENDPOINT_

#include <cstdint>

#include "machine.h"
#include "transport_device.h"
#include "transport_endpoint.h"

// A TransportEndpoint is a kind of machine that connects to TransportTubes.
// It is always stationary.

class TransportEndpoint : public Machine, public TransportDevice {
public:
    TransportEndpoint();
    // TODO TransportEndpoint(Microcontroller)
    virtual ~TransportEndpoint();

    // TODO complete this header
protected:

};

#endif // _MODEL_TRANSPORT_ENDPOINT_

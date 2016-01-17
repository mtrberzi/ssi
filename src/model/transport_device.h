#ifndef _MODEL_TRANSPORT_DEVICE_
#define _MODEL_TRANSPORT_DEVICE_

class Item;

class TransportDevice {
public:
    TransportDevice(){}
    virtual ~TransportDevice(){}

    virtual void disconnect(TransportDevice *connected) = 0;
    /**
       * Asks this transport device to receive an item from a connected neighbour.
       * @param neighbour the transport device the item is being received from
       * @param item the object to receive
       * @return true if the item was successfully received, or false if unsuccessful
       * (e.g. no space left)
       */
    virtual bool receive(TransportDevice *neighbour, Item *item) = 0;
};

#endif // _MODEL_TRANSPORT_DEVICE_

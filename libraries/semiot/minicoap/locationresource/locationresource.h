#ifndef LOCATIONRESOURCE_H
#define LOCATIONRESOURCE_H

#include "coapresource.h"

// observable by default
class LocationResource: public CoAPResource
{
public:
    LocationResource(const char* locationName, MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    int putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);

private:
    int roomNumber = 0;
    char roomJs[100]; // FIXME: magic number
    void setNumber(int number);
    void updateNumberJs();
};

#endif // LOCATIONRESOURCE_H

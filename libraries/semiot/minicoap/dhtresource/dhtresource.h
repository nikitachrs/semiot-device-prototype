#ifndef DHTRESOURCE_H
#define DHTRESOURCE_H

#include "coapresource.h"
#include "DHT.h"

// observable by default
class DHTResource: public CoAPResource
{
public:
    DHTResource(const char* dhtName, uint8_t pin, uint8_t type, MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    void updateDHT();

private:
    float temperature;
    float humidity;
    char dhtJs[100]; // FIXME: magic number
    void updateDHTJs();
    DHT dht;
};

#endif // DHTRESOURCE_H

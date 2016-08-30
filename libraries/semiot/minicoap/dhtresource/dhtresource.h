#ifndef DHTRESOURCE_H
#define DHTRESOURCE_H

#include "coapresource.h"
#include "DHT.h"

// observable by default
// 0 from 100 -- pwm; 51 -- digital high
class DHTResource: public CoAPResource
{
public:
    DHTResource(const char* tickName, MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    int putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    void updateDHT();

private:
    void setTick(long new_tick_value);
    float temperature;
    float humdity;
    char dhtJs[100]; // FIXME: magic number
    void updateDHTJs();
    DHT dht();
};

#endif // DHTRESOURCE_H

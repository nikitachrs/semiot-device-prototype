#ifndef TICKRESOURCE_H
#define TICKRESOURCE_H

#include "coapresource.h"
#ifdef WIRINGPI
// #include <wiringPi.h>
#endif // WIRINGPI


// observable by default
// 0 from 100 -- pwm; 51 -- digital high
class TickResource: public CoAPResource
{
public:
    TickResource(const char* tickName, MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    int putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    void incTick();

private:
    void setTick(long new_tick_value);
    long tick; // unsigned not supported in arduino json
    char tickJs[100]; // FIXME: magic number
    void updateTickJs();
};

#endif // TICKRESOURCE_H

#ifndef LIGHTRESOURCE_H
#define LIGHTRESOURCE_H

#include "coapresource.h"
#ifdef WIRINGPI
// #include <wiringPi.h>
#endif // WIRINGPI


// observable by default
// 0 from 100 -- pwm; 51 -- digital high
class LightResource: public CoAPResource
{
public:
    LightResource(const char* lightName, unsigned int connectedPin, bool isPwmSupported, MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    int putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    void setLight(unsigned char new_light_value);
private:
    char light;
    char lightStringBuf[4]; // // FIXME: magic number
    char lightJs[100]; // FIXME: magic number
    void updateLightJs();
    bool pwmSupported = false;
    unsigned int pin;
};

#endif // LIGHTRESOURCE_H

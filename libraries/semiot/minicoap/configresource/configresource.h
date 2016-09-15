#ifndef CONFIGRESOURCE_H
#define CONFIGRESOURCE_H

#include "coapresource.h"
#include "wl_definitions.h" // ESP8266 wlan definitions

#ifdef ARDUINO
#include <ArduinoJson.h>
#endif

// <json/json.h>


class ConfigResource : public CoAPResource
{
public:
    ConfigResource(MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    int putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    const char* SSID();
    const char* psk();
    const char* AP_SSID();
    const char* AP_PSK();
    void setSTA(const char* ssid, const char* pass);
private:
    char configjs[MAXRESPLEN];
    static const char ssidMaxSize = WL_SSID_MAX_LENGTH+1; // SMT_0000
    static const char pskMaxSize = WL_WPA_KEY_MAX_LENGTH+1;
    char sta_ssid[ssidMaxSize];
    char sta_psk[pskMaxSize];
    char ap_ssid[ssidMaxSize];
    char ap_psk[pskMaxSize];
    void updateConfigJs();
};

#endif // CONFIGRESOURCE_H

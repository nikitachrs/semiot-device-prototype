#include "dhtresource.h"
#include <string.h>
#ifdef ARDUINO
#include <Arduino.h>
#endif // ARDUINO

DHTResource::DHTResource(const char *dhtName, uint8_t pin, uint8_t type, MiniCoAP* coapServer):CoAPResource(coapServer)
{
    resourcePath = {1,{dhtName}};
    dht.begin(pin,type);
    updateDHT();
}

int DHTResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    updateDHTJs();
    return getServer()->coap_make_response(inpkt, outpkt, (uint8_t*)dhtJs, strlen(dhtJs), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_JSON);
}

void DHTResource::updateDHTJs()
{
    memset(dhtJs,0,100); // FIXME: magic number
    // fixme: via json
    strcpy(dhtJs,"{\"@context\":\"/config/context\",\"temperature-value\":\"");
    strcat(dhtJs,String(temperature,2).c_str());
    strcat(dhtJs,"\",\"humidity-value\":\"");
    strcat(dhtJs,String(humidity,2).c_str());
    strcat(dhtJs,"\"}");
}

void DHTResource::updateDHT()
{
    // FIXME:
    float h = 63; // dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = 18; // dht.readTemperature();
    if ((t!=temperature) || (h!=humidity)) {
        humidity = h;
        temperature = t;
        resourceChanged = true;
    }
}

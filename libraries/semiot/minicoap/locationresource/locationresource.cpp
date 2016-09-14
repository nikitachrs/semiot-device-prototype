#include "locationresource.h"
#include <string.h>
#ifdef ARDUINO
#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#endif // ARDUINO

LocationResource::LocationResource(const char *locationName, MiniCoAP* coapServer):CoAPResource(coapServer)
{
    EEPROM.begin(5);
    int value;
    EEPROM.get(0, value); // FIXME: magic address
    // TODO: tick from EEPROM
    resourcePath = {1,{locationName}};
    roomNumber = value;
    updateNumberJs();
    EEPROM.end();
}

int LocationResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    return getServer()->coap_make_response(inpkt, outpkt, (uint8_t*)roomJs, strlen(roomJs), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_JSON);
}

int LocationResource::putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    // FIXME: magic number:
    if (inpkt->payload.len<100) {
        memcpy(roomJs,inpkt->payload.p,inpkt->payload.len);
        // FIXME: magic number:
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& rootJson = jsonBuffer.parseObject(roomJs);
        if (rootJson.success()) {
            int value = rootJson["room-number"];
            setNumber(value);
            return getServer()->coap_make_response(inpkt, outpkt,
                                                   NULL,
                                                   0,
                                                   COAP_RSPCODE_CHANGED,
                                                   COAP_CONTENTTYPE_APPLICATION_JSON);

        }
    }
    return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
}

void LocationResource::updateNumberJs()
{
    memset(roomJs,0,100); // FIXME: magic number
    // fixme: via json
    strcpy(roomJs,"{\"@context\":\"/config/context\",\"room-number\":\"");
    strcat(roomJs,String(roomNumber,DEC).c_str());
    strcat(roomJs,"\"}");
}

void LocationResource::setNumber(int number)
{
    if (roomNumber!=number) {
        roomNumber = number;
        resourceChanged = true;
        updateNumberJs();
        EEPROM.begin(5);
        EEPROM.put(0, roomNumber); // FIXME: magic address
        EEPROM.commit();
        EEPROM.end();
    }
}

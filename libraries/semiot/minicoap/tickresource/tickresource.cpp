#include "tickresource.h"
#include <string.h>
#ifdef ARDUINO
#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#endif // ARDUINO

TickResource::TickResource(const char *tickName, MiniCoAP* coapServer):CoAPResource(coapServer)
{
    EEPROM.begin(5);
    long value;
    // FIXME: EEPROM hardcoded to 2nd byte after led
    EEPROM.get(1, value); // FIXME: magic address
    // TODO: tick from EEPROM
    resourcePath = {1,{tickName}};

    setTick(value);
    EEPROM.end();
}

int TickResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    updateTickJs();
    return getServer()->coap_make_response(inpkt, outpkt, (uint8_t*)tickJs, strlen(tickJs), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_JSON);
}

int TickResource::putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    // FIXME: magic number:
    if (inpkt->payload.len<100) {
        memcpy(tickJs,inpkt->payload.p,inpkt->payload.len);
        // FIXME: magic number:
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& rootJson = jsonBuffer.parseObject(tickJs);
        if (rootJson.success()) {
            long tick_value = rootJson["tick-value"];
            setTick(tick_value);
            return getServer()->coap_make_response(inpkt, outpkt,
                                                   NULL,
                                                   0,
                                                   COAP_RSPCODE_CHANGED,
                                                   COAP_CONTENTTYPE_APPLICATION_JSON);

        }
    }
    return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
}

void TickResource::updateTickJs()
{
    Serial.println("tick:");
    Serial.println(tick);
    memset(tickJs,0,100); // FIXME: magic number
    // fixme: via json
    strcpy(tickJs,"\{\"@context\":\"\/config\/context\",\"tick-value\":\"");
    strcat(tickJs,String(tick,DEC).c_str());
    strcat(tickJs,"\"\}");
}

void TickResource::setTick(long new_tick_value)
{
    printf("from payload:%d\n",new_tick_value);
    if (new_tick_value!=tick) {
        tick = new_tick_value;
        resourceChanged = true;
        EEPROM.begin(5);
        EEPROM.put(1, tick); // FIXME: magic address
        EEPROM.commit();
        EEPROM.end();
    }
}

void TickResource::incTick()
{
    setTick(tick+1);
}

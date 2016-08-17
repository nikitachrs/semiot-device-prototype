#include "lightresource.h"
#include <string.h>
#ifdef ARDUINO
#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#endif // ARDUINO

LightResource::LightResource(const char *lightName, unsigned int connectedPin, bool isPwmSupported, MiniCoAP* coapServer):CoAPResource(coapServer)
{
    EEPROM.begin(4);
    char pmw_value;
    // FIXME: EEPROM hardcoded to 1 led
    EEPROM.get(0, pmw_value);
    // TODO: light from EEPROM
    resourcePath = {1,{lightName}};
    pin = connectedPin;
    pwmSupported = isPwmSupported;
    setLight(pmw_value);
    EEPROM.end();
}

int LightResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    updateLightJs();
    return getServer()->coap_make_response(inpkt, outpkt, (uint8_t*)lightJs, strlen(lightJs), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_OCTECT_STREAM);
}

int LightResource::putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    // FIXME: magic number:
    if (inpkt->payload.len<100) {
        memcpy(lightJs,inpkt->payload.p,inpkt->payload.len);
        // FIXME: magic number:
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& rootJson = jsonBuffer.parseObject(lightJs);
        if (rootJson.success()) {
            const char *pwm_value = rootJson["pwm-value"];
            Serial.println("Parsed PWM value:");
            Serial.println(pwm_value);
            setLight((unsigned char)String(pwm_value).toInt());
            return getServer()->coap_make_response(inpkt, outpkt,
                                                   NULL,
                                                   0,
                                                   COAP_RSPCODE_CHANGED,
                                                   COAP_CONTENTTYPE_APPLICATION_JSON);

        }
    }
    return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
}

void LightResource::updateLightJs()
{
    Serial.println("light:");
    Serial.println(light);
    memset(lightJs,0,100); // FIXME: magic number
    // fixme: via json
    strcpy(lightJs,"\{\"@context\":\"\/config\/context\",\"pwm-value\":\"");
    strcat(lightJs,String((int)light,DEC).c_str());
    strcat(lightJs,"\"\}");
}

void LightResource::setLight(unsigned char new_light_value)
{
    printf("from payload:%d\n",new_light_value);
    if (new_light_value!=light) {
#ifdef LED

#if defined(WIRINGPI)
        if (pwmSupported) {
            pwmWrite(pin, (PWMRANGE)/100*new_light_value);
            printf("pwm set:%d\n",new_light_value);
        }
        else {
            if (light>50) {
                digitalWrite(pin,HIGH);
                printf("digital set:%d\n",HIGH);
            }
            else {
                digitalWrite(pin,LOW);
                printf("digital set:%d\n",LOW);
            }
        }
#endif // WIRINGPI

#if defined(ARDUINO)
        if (pwmSupported) {
            analogWrite(pin, (PWMRANGE)/100*new_light_value);
        }
        else {
            digitalWrite(pin, new_light_value>50);
        }
#endif // ARDUINO

#endif // pin
        light = new_light_value;
        resourceChanged = true;
        EEPROM.begin(4);
        EEPROM.put(0, light); // FIXME: magic address
        EEPROM.commit();
        EEPROM.end();
    }
}

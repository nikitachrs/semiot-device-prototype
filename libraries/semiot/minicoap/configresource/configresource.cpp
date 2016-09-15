#include "configresource.h"
#include <string.h>
#ifdef ARDUINO
#include <ESP8266WiFi.h>
#include <Hash.h>
#endif // ARDUINO

ConfigResource::ConfigResource(MiniCoAP *coapServer):CoAPResource(coapServer)
{
    memset(ap_ssid,0,pskMaxSize);
    memset(ap_psk,0,pskMaxSize);
    strcpy(ap_ssid,
        String(String("SMT_")+sha1(WiFi.macAddress()).substring(0,4)).c_str()); // ;
    strcpy(ap_psk,"semiotproject");

    resourcePath = {1,{"config"}};
}

int ConfigResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    if (WiFi.getMode() == WIFI_AP) {
        updateConfigJs();
        if (strlen(configjs)) {
            return getServer()->coap_make_response(inpkt, outpkt, (uint8_t
            *)configjs, strlen(configjs), COAP_RSPCODE_CONTENT,
    COAP_CONTENTTYPE_APPLICATION_JSON);
        }
    }
    return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_NOT_FOUND, COAP_CONTENTTYPE_TEXT_PLAIN);
}

int ConfigResource::putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    //    {
    //    "@context": "/config/context",
    //    "wifi-name": "ISST",
    //    "wifi-password": "blahbalh"
    //    }
    if (WiFi.getMode() == WIFI_AP)  {
        memset(configjs,0,MAXRESPLEN);
        if (inpkt->payload.len<MAXRESPLEN) {
            memcpy(configjs,inpkt->payload.p,inpkt->payload.len);
            Serial.println(configjs);
            // FIXME: magic number:
            StaticJsonBuffer<200> jsonBuffer;
            JsonObject& rootJson = jsonBuffer.parseObject(configjs);
            Serial.println("parsed object:");
            if (rootJson.success()) {
                Serial.println("success:");
                rootJson["wifi-name"].printTo(Serial);
                const char *wifiName = rootJson["wifi-name"];
                Serial.println((int)wifiName,DEC);
                const char *wifiPassword = rootJson["wifi-password"];
                Serial.println(wifiPassword);
                if (wifiName && wifiPassword) {
                    setSTA(wifiName,wifiPassword);
                    return getServer()->coap_make_response(inpkt, outpkt,
                                                           NULL,
                                                           0,
                                                           COAP_RSPCODE_CHANGED,
                                                           COAP_CONTENTTYPE_APPLICATION_JSON);
                }
            }
        }
    }
    return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
}

const char *ConfigResource::SSID()
{
    return sta_ssid;
}

const char *ConfigResource::psk()
{
    return sta_psk;
}

const char *ConfigResource::AP_SSID()
{
    return ap_ssid;
}

const char *ConfigResource::AP_PSK()
{
    return ap_psk;
}

void ConfigResource::setSTA(const char *ssid, const char *pass)
{
    memset(sta_ssid,0,pskMaxSize);
    memset(sta_psk,0,pskMaxSize);
    strcpy(sta_ssid,ssid);
    strcpy(sta_psk,pass);
}

void ConfigResource::updateConfigJs()
{
    Serial.println("pararams:");
    Serial.print(sta_ssid);
    Serial.println(sta_psk);
    memset(configjs,0,MAXRESPLEN);
    // fixme: via json
    strcpy(configjs,"{\"@context\":\"/config/context\",\"wifi-name\":\"");
    strcat(configjs,sta_ssid);
    strcat(configjs,"\",\"wifi-password\":\"");
    strcat(configjs,sta_psk);
    strcat(configjs,"\"}");
}

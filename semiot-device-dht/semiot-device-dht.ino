#include <ESP8266WiFi.h>
#include <string.h>
#include "minicoap.h"
#include "wellknowncoreresource.h"
#include "configresource.h"
#include "labelresource.h"
#include "dhtresource.h"
#include "locationresource.h"

const int sleep_interval = 1500;

const char *ap_ssid = "regularnetwork2";
const char *ap_password = "changeme";
const char numberOfSTAAttempts = 23; // about 10 sec

MiniCoAP coap;

const char configContextPath[] = "\/config\/context";
const char configSchemaPath[] = "\/config\/schema";

const char configContextText[] = "{\"@context\":{\"xsd\":\"http:\/\/www.w3.org\/2001\/XMLSchema#\",\"rdfs\":\"http:\/\/www.w3.org\/2000\/01\/rdf-schema#\",\"@vocab\":\"\/config\/schema\"}}";
const char configSchemaText[] = "{\"@context\":\"\/config\/context\",\"wifi-name\":{\"@type\":\"xsd:string\",\"rdfs:label\":\"Wi-Fi Network Name\"},\"wifi-password\":{\"@type\":\"xsd:string\",\"rdfs:label\":\"Wi-Fi Password\"}}";


ConfigResource confRes(&coap);
LabelResource configContext(configContextPath,configContextText,&coap);
LabelResource configSchema(configSchemaPath,configSchemaText,&coap);

const char wnkPublicAnswer[] = "<\/dht1>;ct=50,<\/dht1\/schema>;ct=50,<\/location>;ct=50,<\/location\/schema>;ct=50";
const char wnkLocalAnswer[] = "<\/dht1>;ct=50,<\/dht1\/schema>;ct=50,<\/location>;ct=50,<\/location\/schema>;ct=50,<\/config>;ct=50,<\/config\/schema>;ct=50,<\/config\/context>;ct=50";
WellKnownCoreResource wnkRes(&coap);

const char dht1SchemaPath[] = "\/config\/schema";
const char dht1SchemaText[] = "{\"@context\":\"\/config\/context\",\"temperature-value\":{\"@type\":\"xsd:string\",\"rdfs:label\":\"Temperature value from 21 to 23 Celsius\"},\"humidity-value\":{\"@type\":\"xsd:string\",\"rdfs:label\":\"Humidity value in percent\"}}";

const int dht1Pin = D2;
const int dht1Type = DHT22;
const char dht1Name[] = "dht1";
DHTResource dht1(dht1Name,dht1Pin,dht1Type,&coap);
// TODO: dht context
LabelResource dht1Schema(dht1SchemaPath,dht1SchemaText,&coap);

const char locationSchemaPath[] = "\/config\/schema";
const char locationSchemaText[] = "{\"@context\":\"\/location\/context\",\"room-number\":{\"@type\":\"xsd:string\",\"rdfs:label\":\"Room number\"}}";
const char locationName[] = "location";
LocationResource locationRes(locationName,&coap);
LabelResource locationSchema(locationSchemaPath,locationSchemaText,&coap);

const int buttonPin = D7;
const int highOutPin = D6;
const int staLedPin = D1;
const int apLedPin = D2;
bool buttonPressed, configured;

bool setAP() {
    analogWrite(staLedPin,0);
    analogWrite(apLedPin,PWMRANGE/4);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    if (myIP[0]!=0) {
        analogWrite(apLedPin,PWMRANGE);
        configured=true;
        return true;
    }
    return false;
}

bool setSTA() {
    // FIXME: not inf
    analogWrite(apLedPin,0);
    analogWrite(staLedPin,PWMRANGE/4);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting STA");
    WiFi.begin(confRes.SSID(),confRes.psk());
    char a = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        a++;
        if (a>numberOfSTAAttempts) {
            break;
        }
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    analogWrite(staLedPin,PWMRANGE);
    configured=true;
    return true; // FIXME
}

void setup() {
    Serial.begin(115200);
    pinMode(highOutPin,OUTPUT);
    digitalWrite(highOutPin,HIGH);
    pinMode(buttonPin,INPUT);
    pinMode(apLedPin,OUTPUT);
    pinMode(staLedPin,OUTPUT);
    confRes.setSTA(WiFi.SSID().c_str(),WiFi.psk().c_str());
    coap.begin();
    Serial.println("Started");
}

void loop() {
    dht1.updateDHT();
    buttonPressed = digitalRead(buttonPin);
    if (buttonPressed) {
        if (WiFi.getMode()!=WIFI_AP) {
            Serial.println("to AP");
            wnkRes.setAnswer(wnkLocalAnswer);
            setAP();
        }
    }
    else {
        if (WiFi.getMode()!=WIFI_STA) {
            Serial.println("to STA");
            wnkRes.setAnswer(wnkPublicAnswer);
            setSTA();
        }
    }
    if (configured) {
        coap.answerForObservations();
        coap.answerForIncomingRequest();
    }
    // sleep for 1.5 sec without disconnection
    // should save the power
    delay(sleep_interval);
}


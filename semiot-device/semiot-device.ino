#include <ESP8266WiFi.h>
#include <string.h>
#include "minicoap.h"
#include "wellknowncoreresource.h"
#include "lightresource.h"
#include "configresource.h"
#include "labelresource.h"
#include "tickresource.h"

const char *ap_ssid = "regularnetwork2";
const char *ap_password = "changeme";
const char numberOfSTAAttempts = 23; // about 10 sec

MiniCoAP coap;

const char configContextPath[] = "\/config\/context";
const char configSchemaPath[] = "\/config\/schema";
const char led1SchemaPath[] = "\/led1\/schema";
const char tickSchemaPath[] = "\/tick1\/schema";

const char configContextText[] = "{\"@context\":{\"xsd\":\"http:\/\/www.w3.org\/2001\/XMLSchema#\",\"rdfs\":\"http:\/\/www.w3.org\/2000\/01\/rdf-schema#\",\"@vocab\":\"\/config\/schema\"}}";
const char configSchemaText[] = "{\"@context\":\"\/config\/context\",\"wifi-name\":{\"@type\":\"xsd:string\",\"rdfs:label\":\"Wi-Fi Network Name\"},\"wifi-password\":{\"@type\":\"xsd:string\",\"rdfs:label\":\"Wi-Fi Password\"}}";
const char led1SchemaText[] = "{\"@context\":\"\/config\/context\",\"pwm-value\":{\"@type\":\"xsd:string\",\"rdfs:label\":\"PWM Brightness value from 0 to 100\"}}";
const char tickSchemaText[] = "{\"@context\":\"\/config\/context\",\"tick-value\":{\"@type\":\"xsd:string\",\"rdfs:label\":\"Impulse counter\"}}";


ConfigResource confRes(&coap);
LabelResource configContext(configContextPath,configContextText,&coap);
LabelResource configSchema(configSchemaPath,configSchemaText,&coap);

const char wnkPublicAnswer[] = "<\/led1>;ct=50,<\/led1\/schema>;ct=50,<\/tick1>;ct=50,<\/tick1\/schema>;ct=50";
const char wnkLocalAnswer[] = "<\/led1>;ct=50,<\/led1\/schema>;ct=50,<\/tick1>;ct=50,<\/tick1\/schema>;ct=50,<\/config>;ct=50,<\/config\/schema>;ct=50,<\/config\/context>;ct=50";
WellKnownCoreResource wnkRes(&coap);


long tick;
const int tickPin = D5;
TickResource tickRes("tick",&coap);
LabelResource tickSchema(tickSchemaPath,tickSchemaText,&coap);
void incTick() {
    tickRes.incTick();
}

const int led1Pin = D3;
const char led1Name[] = "led1";
LightResource led1(led1Name,led1Pin,true,&coap);
LabelResource led1Schema(led1SchemaPath,led1SchemaText,&coap);

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
    pinMode(tickPin,INPUT);
    pinMode(apLedPin,OUTPUT);
    pinMode(staLedPin,OUTPUT);
    confRes.setSTA(WiFi.SSID().c_str(),WiFi.psk().c_str());
    coap.begin();
    Serial.println("Started");
    attachInterrupt(digitalPinToInterrupt(tickPin),incTick,RISING);
}

void loop() {
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
}


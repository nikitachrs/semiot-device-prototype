#include "coapresource.h"
#include <ESP8266WiFi.h> // FIXME: remove

CoAPResource::CoAPResource(MiniCoAP *coapServer)
{
    server = coapServer;
    coap_endpoint_func getMP = std::function<int(const coap_packet_t *inpkt, coap_packet_t *outpkt)>(std::bind(&CoAPResource::getMethod, this, std::placeholders::_1, std::placeholders::_2));
    coap_endpoint_func putMP = std::function<int(const coap_packet_t *inpkt, coap_packet_t *outpkt)>(std::bind(&CoAPResource::putMethod, this, std::placeholders::_1, std::placeholders::_2));
    server->addEndpoint(COAP_METHOD_GET,getMP,&resourcePath,coreAttr,&resourceChanged);
    server->addEndpoint(COAP_METHOD_PUT,putMP,&resourcePath);
}

int CoAPResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    server->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_NOT_FOUND, COAP_CONTENTTYPE_TEXT_PLAIN);
}

int CoAPResource::putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    server->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_NOT_FOUND, COAP_CONTENTTYPE_TEXT_PLAIN);
}

int CoAPResource::postMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    server->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_NOT_FOUND, COAP_CONTENTTYPE_TEXT_PLAIN);
}

int CoAPResource::deleteMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    server->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_NOT_FOUND, COAP_CONTENTTYPE_TEXT_PLAIN);
}

MiniCoAP *CoAPResource::getServer()
{
    return server;
}

#include "labelresource.h"
#include <string.h>

LabelResource::LabelResource(const char *path, const char *label, MiniCoAP *coapServer):CoAPResource(coapServer)
{
    memset(&_label,0,sizeof(_label));
    strcpy(_label,label);
    // FIXME: real parse
    switch (strlen(path)) {
    case 15:
        resourcePath = {2, {"config", "context"}};
        break;
    case 14:
        resourcePath = {2, {"config", "schema"}};
        break;
    case 12:
        resourcePath = {2, {"led1", "schema"}};
        break;
    case 13:
        resourcePath = {2, {"tick1", "schema"}};
        break;
    default:
        resourcePath = {1, {"error"}};
        break;
    }

}

int LabelResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    return getServer()->coap_make_response(inpkt, outpkt, (uint8_t *)&_label, strlen(_label), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_JSON);
}

//int LabelResource::putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
//{
//    if (inpkt->payload.len>sizeof(_label)) {
//        return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN); // FIXME
//    }
//    else {
//        memset(&_label,0,sizeof(_label));
//        strncpy(_label,(char *)inpkt->payload.p,inpkt->payload.len);
//        return getServer()->coap_make_response(inpkt, outpkt, NULL, 0, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
//    }
//}

#include "wellknowncoreresource.h"

WellKnownCoreResource::WellKnownCoreResource(MiniCoAP *coapServer):CoAPResource(coapServer)
{
    resourcePath = path_well_known_core;
}

int WellKnownCoreResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    return getServer()->coap_make_response(inpkt, outpkt, (uint8_t *)_answer, strlen(_answer), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_LINKFORMAT);
}

void WellKnownCoreResource::setAnswer(const char *answer)
{
    _answer = answer;
}

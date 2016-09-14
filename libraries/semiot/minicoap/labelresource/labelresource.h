#ifndef LABELRESOURCE_H
#define LABELRESOURCE_H

#include "coapresource.h"

class LabelResource : public CoAPResource
{
public:
    LabelResource(const char * path, const char * label, MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
    // int putMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
private:
    const char *_label;
};

#endif // LABELRESOURCE_H

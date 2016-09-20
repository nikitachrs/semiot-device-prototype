#ifndef DOCRESOURCE_H
#define DOCRESOURCE_H

#include <string.h>
#include "coapresource.h"
#include <ArduinoJson.h>
#define ANSWER_JS_MAXLEN 1000
#define DOC_URI_MAXLEN 60

class DocResource : public CoAPResource
{
public:
    DocResource(MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
private:
    StaticJsonBuffer<ANSWER_JS_MAXLEN> jsBuffer;
    String answer;
    // JsonObject& rootJson;
    // JsonObject& contextJsObj;

    void updateAnswerJs();

    const char *rdfsLabelLiteral = "New Device";

    const char *context_prefix = "@context";
    const char *id_prefix = "@id";
    const char *type_prefix = "@type";

    const char *doc_prefix = "doc";
    const char *doc_protocol = "coap://";
    const char *doc_path = "/doc#";
    char doc_uri[DOC_URI_MAXLEN];
    const char *docApiDocumentation_uri = "doc:ApiDocumentation";
    const char *docTempDevice_uri = "doc:TempDevice";
    const char *docLatestProcessOutput_uri = "doc:latestProcessOutput";

    const char *ssn_prefix = "ssn";
    const char *ssn_uri = "http://purl.oclc.org/NET/ssnx/ssn#";
    const char *ssnDevice_uri = "ssn:Device";

    const char *rdfs_prefix = "rdfs";
    const char *rdfs_uri = "http://www.w3.org/2000/01/rdf-schema#";
    const char *rdfsLabel_uri = "rdfs:label";
    const char *rdfsDomain_uri = "rdfs:domain";
    const char *rdfsRange_uri = "rdfs:range";

    const char *dcterms_prefix = "dcterms";
    const char *dcterms_uri = "http://purl.org/dc/terms/";
    const char *dctermsIndentifier_uri = "dcterms:identifier";

    const char *hydra_prefix = "hydra";
    const char *hydra_uri = "http://www.w3.org/ns/hydra/core#";
    const char *hydraApiDocumentation_uri = "hydra:ApiDocumentation";
    const char *hydraSupportedClass_uri = "hydra:supportedClass";
    const char *hydraLink_uri = "hydra:Link";


    const char *qudt_prefix = "qudt";
    const char *qudt_uri = "http://qudt.org/schema/qudt#";

    const char *semiot_prefix = "semiot";
    const char *semiot_uri = "https://w3id.org/semiot/ontologies/semiot#";
    const char *semiotHasPrototype_uri = "semiot:hasPrototype";
    const char *semiotProcess_uri = "semiot:Process";
    const char *semiotProcessOutput_uri = "semiot:ProcessOutput";

    const char *tempDevicePrototype_uri = "http://example.com/prototypes/TempDevice";

};

#endif // DOCRESOURCE_H

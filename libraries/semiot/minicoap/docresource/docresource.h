#ifndef DOCRESOURCE_H
#define DOCRESOURCE_H

#include <string.h>
#include "coapresource.h"
#include <ArduinoJson.h>
#define JS_BUF_LEN 1500

class DocResource : public CoAPResource
{
public:
    DocResource(MiniCoAP* coapServer);
    int getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt);
private:
    String answer;

    void updateAnswer();

    String rdfsLabelLiteral = "New Device";

    String context_prefix = "@context";
    String id_prefix = "@id";
    String type_prefix = "@type";

    String doc_prefix = "doc";
    String doc_protocol = "coap://";
    String doc_path = "/doc#";
    String doc_uri;
    String docApiDocumentation_uri = "doc:ApiDocumentation";
    String docTempDevice_uri = "doc:TempDevice";
    String docLatestProcessOutput_uri = "doc:latestProcessOutput";

    String ssn_prefix = "ssn";
    String ssn_uri = "http://purl.oclc.org/NET/ssnx/ssn#";
    String ssnDevice_uri = "ssn:Device";

    String rdfs_prefix = "rdfs";
    String rdfs_uri = "http://www.w3.org/2000/01/rdf-schema#";
    String rdfsLabel_uri = "rdfs:label";
    String rdfsDomain_uri = "rdfs:domain";
    String rdfsRange_uri = "rdfs:range";

    String dcterms_prefix = "dcterms";
    String dcterms_uri = "http://purl.org/dc/terms/";
    String dctermsIndentifier_uri = "dcterms:identifier";

    String hydra_prefix = "hydra";
    String hydra_uri = "http://www.w3.org/ns/hydra/core#";
    String hydraApiDocumentation_uri = "hydra:ApiDocumentation";
    String hydraSupportedClass_uri = "hydra:supportedClass";
    String hydraLink_uri = "hydra:Link";


    String qudt_prefix = "qudt";
    String qudt_uri = "http://qudt.org/schema/qudt#";

    String semiot_prefix = "semiot";
    String semiot_uri = "https://w3id.org/semiot/ontologies/semiot#";
    String semiotHasPrototype_uri = "semiot:hasPrototype";
    String semiotProcess_uri = "semiot:Process";
    String semiotProcessOutput_uri = "semiot:ProcessOutput";

    String tempDevicePrototype_uri = "http://example.com/prototypes/TempDevice";

};

#endif // DOCRESOURCE_H

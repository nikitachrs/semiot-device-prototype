#include "docresource.h"
#include <ESP8266WiFi.h>
#include <Hash.h>

DocResource::DocResource(MiniCoAP *coapServer):CoAPResource(coapServer)
{
    resourcePath = {1,{"doc"}};

    updateAnswerJs();

    JsonObject& rootJson = jsBuffer.createObject();
    JsonObject& contextJsObj = rootJson.createNestedObject(context_prefix);
        contextJsObj[doc_prefix] = doc_uri;
        contextJsObj[ssn_prefix] = ssn_uri;
        contextJsObj[rdfs_prefix] = rdfs_uri;
        contextJsObj[dcterms_prefix] = dcterms_uri;
        contextJsObj[hydra_prefix] = hydra_uri;
        contextJsObj[semiot_prefix] = semiot_uri;
    rootJson[id_prefix] = docApiDocumentation_uri;
    rootJson[type_prefix] = hydraApiDocumentation_uri;
    JsonArray& hydraSupportedClassJsObj = rootJson.createNestedArray(hydraSupportedClass_uri);
        JsonObject& docTempDeviceJsObj = hydraSupportedClassJsObj.createNestedObject();
            docTempDeviceJsObj[id_prefix] = docTempDevice_uri;
            docTempDeviceJsObj[type_prefix] = ssnDevice_uri;
            docTempDeviceJsObj[semiotHasPrototype_uri] = tempDevicePrototype_uri;
            docTempDeviceJsObj[rdfsLabel_uri] = rdfsLabelLiteral;
            docTempDeviceJsObj[dctermsIndentifier_uri] = sha1(WiFi.macAddress()).c_str();
        JsonObject& docLatestProcessOutputJsObj = hydraSupportedClassJsObj.createNestedObject();
            docLatestProcessOutputJsObj[id_prefix] = docLatestProcessOutput_uri;
            docLatestProcessOutputJsObj[type_prefix] = hydraLink_uri;
            docLatestProcessOutputJsObj[rdfsDomain_uri] = semiotProcess_uri;
            docLatestProcessOutputJsObj[rdfsRange_uri] = semiotProcessOutput_uri;
    rootJson.printTo(answer);
}

int DocResource::getMethod(const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    updateAnswerJs(); // FIXME: too frequent
    return getServer()->coap_make_response(inpkt, outpkt, (uint8_t*)answer.c_str(), strlen(answer.c_str()), COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_JSON);
}

void DocResource::updateAnswerJs() // FIXME: not saving to json
{
    memset(doc_uri,0,DOC_URI_MAXLEN);
    strcpy(doc_uri,doc_protocol);
    switch (WiFi.getMode()) {
    case WIFI_STA:
        strcat(doc_uri,WiFi.localIP().toString().c_str());
        break;
    case WIFI_AP:
        strcat(doc_uri,WiFi.softAPIP().toString().c_str());
        break;
    default:
        strcat(doc_uri,"localhost");
        break;
    }
    strcat(doc_uri,doc_path);
}

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// FORMAT:
// "WORD" (4B)
// low_counter (2B) -- 00
// high_counter (4B)
// MAC (6B)
// == 16 BYTES
// TODO: checksum?

// EnergoMera-CE102R5-AK: "NRGM"
// Teplovodokhran Pulsar water consumption meter: "PLSR"

#define MODEL_WORD "NRGM"

// debug led to VCC:
#define DEBUG_LED_PIN 0 // GPIO0
#define DEBUG_LED_LIGHT LOW
#define DEBUG_LED_DARK HIGH

bool _debug = false;
bool _debug_led = false;

#define MAX_COUNTER_LOW_NUMBER 1

uint16_t low_counter = 0; // 2 bytes
unsigned int high_counter = 0; // 4 bytes for esp8266
bool counter_changed = false;
bool need_to_reconnect = false;

WiFiUDP _udp;
IPAddress ip;
byte mac[6];
const int udp_port = 44444;
#define UDP_GTW_OK_SIZE 5
#define UDP_GTW_OK "GTWOK"
#define UDP_GTW_PING "GTW"
char gtw_ok_buffer[UDP_GTW_OK_SIZE];
char gtw_ip;

#define M_BUF_SIZE 15
int _m_buf[M_BUF_SIZE];
int _m_buf_count=0;

void gtw_search() {
    ip = (~WiFi.subnetMask()) | WiFi.gatewayIP();
    gtw_ip=0;
    while (gtw_ip == 0) {
        if (_udp.beginPacket(ip, udp_port)) {
            _udp.write(UDP_GTW_PING);
            _udp.endPacket();
            delay(1500);
            int sz = _udp.parsePacket();
            if (sz==UDP_GTW_OK_SIZE) {
                _udp.read(gtw_ok_buffer,UDP_GTW_OK_SIZE);
                if (memcmp(gtw_ok_buffer, UDP_GTW_OK, UDP_GTW_OK_SIZE) == 0) {
                    ip=_udp.remoteIP();
                    break;
                }
            }
        }
    }
}

// WPS:
void reconnect_to_wlan() {
    while (WiFi.status() != WL_CONNECTED) {
        if (_debug_led) {
            digitalWrite(DEBUG_LED_PIN, DEBUG_LED_LIGHT);
        }
        // WiFi.begin("",""); // decided to get rid of this
        if (_debug_led) {
            digitalWrite(DEBUG_LED_PIN, DEBUG_LED_DARK);
        }
        // Long delay required especially soon after power on.
        delay(4000);
        // Check if WiFi is already connected and if not, begin the WPS process.
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.beginWPSConfig();
            // Another long delay required.
            delay(3000);
        }
    }
    WiFi.macAddress(mac);
    _udp.begin(udp_port);
    gtw_search();
    if (_debug_led) {
        digitalWrite(DEBUG_LED_PIN, DEBUG_LED_LIGHT);
    }
}

// TODO: separate to lib:
//------------------------------------------------
// RS485 PROTOCOL:
#define SerialTxControl 2 // GPIO 2
#define RS485Transmit    HIGH
#define RS485Receive     LOW

// CE PROTOCOL:
#define CE_BAUDRATE 9600

#define END_CH 0xC0
#define END_REPL_1_CH 0xDB
#define END_REPL_2_CH 0xDC

#define ESC_CH 0xDB
#define ESC_REPL_1_CH 0xDB
#define ESC_REPL_2_CH 0xDD

#define OPT_CH 0x48
#define DIRECT_REQ_CH 0b10000000
#define CLASS_ACCESS_CH 0b1010000

#define Ping_Command 0x0001
#define ReadSerialNumber_Command 0x011A
#define ReadTariffSum_Command 0x0131

char Buffer[14];
unsigned char crc8;
int i;
const unsigned char crc8tab[256] = {
0x0 ,0xb5,0xdf,0x6a,0xb ,0xbe,0xd4,0x61, 0x16,0xa3,0xc9,0x7c,0x1d,0xa8,0xc2,0x77,
0x2c,0x99,0xf3,0x46,0x27,0x92,0xf8,0x4d,0x3a,0x8f,0xe5,0x50,0x31,0x84,0xee,0x5b,
0x58,0xed,0x87,0x32,0x53,0xe6,0x8c,0x39,0x4e,0xfb,0x91,0x24,0x45,0xf0,0x9a,0x2f,
0x74,0xc1,0xab,0x1e,0x7f,0xca,0xa0,0x15,0x62,0xd7,0xbd,0x8 ,0x69,0xdc,0xb6,0x3 ,
0xb0,0x5 ,0x6f,0xda,0xbb,0xe ,0x64,0xd1,0xa6,0x13,0x79,0xcc,0xad,0x18,0x72,0xc7,
0x9c,0x29,0x43,0xf6,0x97,0x22,0x48,0xfd,0x8a,0x3f,0x55,0xe0,0x81,0x34,0x5e,0xeb,
0xe8,0x5d,0x37,0x82,0xe3,0x56,0x3c,0x89,0xfe,0x4b,0x21,0x94,0xf5,0x40,0x2a,0x9f,
0xc4,0x71,0x1b,0xae,0xcf,0x7a,0x10,0xa5,0xd2,0x67,0xd ,0xb8,0xd9,0x6c,0x6 ,0xb3,
0xd5,0x60,0xa ,0xbf,0xde,0x6b,0x1 ,0xb4,0xc3,0x76,0x1c,0xa9,0xc8,0x7d,0x17,0xa2,
0xf9,0x4c,0x26,0x93,0xf2,0x47,0x2d,0x98,0xef,0x5a,0x30,0x85,0xe4,0x51,0x3b,0x8e,
0x8d,0x38,0x52,0xe7,0x86,0x33,0x59,0xec,0x9b,0x2e,0x44,0xf1,0x90,0x25,0x4f,0xfa,
0xa1,0x14,0x7e,0xcb,0xaa,0x1f,0x75,0xc0,0xb7,0x2 ,0x68,0xdd,0xbc,0x9 ,0x63,0xd6,
0x65,0xd0,0xba,0xf ,0x6e,0xdb,0xb1,0x4,0x73,0xc6,0xac,0x19,0x78,0xcd,0xa7,0x12,
0x49,0xfc,0x96,0x23,0x42,0xf7,0x9d,0x28,0x5f,0xea,0x80,0x35,0x54,0xe1,0x8b,0x3e,
0x3d,0x88,0xe2,0x57,0x36,0x83,0xe9,0x5c,0x2b,0x9e,0xf4,0x41,0x20,0x95,0xff,0x4a,
0x11,0xa4,0xce,0x7b,0x1a,0xaf,0xc5,0x70,0x7 ,0xb2,0xd8,0x6d,0xc ,0xb9,0xd3,0x66 };


void sendByteToRS485(uint16_t outByte) {
    digitalWrite(SerialTxControl, RS485Transmit);
    if (_debug) {
        _udp.beginPacket(ip, udp_port);
        _udp.write(outByte);
        _udp.endPacket();
    }
    Serial.write(outByte);
    Serial.flush();
    digitalWrite(SerialTxControl, RS485Receive);
    delay(1000);
}

void startCePacket() {
    sendByteToRS485(END_CH);
}

void endCEPacket() {
    sendByteToRS485(END_CH);
}

void sendByteToCE(uint16_t outByte) {
    if (outByte==END_CH) {
        sendByteToRS485(END_REPL_1_CH);
        sendByteToRS485(END_REPL_2_CH);
    }
    else if (outByte==ESC_CH) {
        sendByteToRS485(ESC_REPL_1_CH);
        sendByteToRS485(ESC_REPL_1_CH);
    }
    else {
        sendByteToRS485(outByte);
    }
}

void sendCommandToCE(int AddrD, int Command) {
    startCePacket();

    crc8 = 0;
    sendByteToCE(OPT_CH); crc8= crc8tab[crc8 ^ OPT_CH];


    unsigned char AddrDH = (unsigned char) (AddrD >> 8);
    unsigned char  AddrDL = (unsigned char ) (AddrD & 0xff);
    sendByteToCE(AddrDL); crc8= crc8tab[crc8 ^ AddrDL];
    sendByteToCE(AddrDH); crc8= crc8tab[crc8 ^ AddrDH];

    sendByteToCE(0); crc8= crc8tab[crc8 ^ 0];
    sendByteToCE(0); crc8= crc8tab[crc8 ^ 0];

    // PAL:
    unsigned char passwd[4] = {0x0,0x0,0x0,0x0}; // TODO: user pass
    for (i=0;i<4;i++) { // FIXME: magic 4 bytes
        sendByteToCE(passwd[i]); crc8= crc8tab[crc8 ^ passwd[i]];
    }

    // serv:
    unsigned char MessageLength = 0; // TODO:
    if (Command==ReadTariffSum_Command) {
        MessageLength=1;
    }

    unsigned char serv = DIRECT_REQ_CH + CLASS_ACCESS_CH + MessageLength;
    sendByteToCE(serv); crc8= crc8tab[crc8 ^ serv];

    unsigned char CommandH = (unsigned char) (Command >> 8);
    unsigned char CommandL =  (unsigned char ) (Command & 0xff);
    sendByteToCE(CommandH); crc8= crc8tab[crc8 ^ CommandH];
    sendByteToCE(CommandL); crc8= crc8tab[crc8 ^ CommandL];

    // if Ping -- No Additional Data in PAL
    if (Command==ReadTariffSum_Command) {
        sendByteToCE(0); crc8= crc8tab[crc8 ^ 0];
    }

    sendByteToCE(crc8);

    endCEPacket();
}

void Ping(uint16_t AddrD) {
    sendCommandToCE(AddrD, Ping_Command);
}

void ReadSerialNumber(uint16_t AddrD) {
    sendCommandToCE(AddrD, ReadSerialNumber_Command);
}

// TODO: additional param
void ReadTariffSum(uint16_t AddrD) {
    sendCommandToCE(AddrD, ReadTariffSum_Command);
}

void BadCommand() {
    startCePacket();
    sendByteToRS485(0);
    endCEPacket();
}

// END of CE Protocol
//------------------------------------------------


void setup() {
    if (_debug_led) {
        pinMode(DEBUG_LED_PIN, OUTPUT);
    }
    //EEPROM.begin(16);
    Serial.begin(CE_BAUDRATE);
    pinMode(SerialTxControl, OUTPUT);
    digitalWrite(SerialTxControl, RS485Receive);
    reconnect_to_wlan();
    if (_debug) {
        _udp.beginPacket(ip, udp_port);
        _udp.write("Setup completed");
        _udp.endPacket();
    }
}

void loop() {
    delay(3000);
    if (Serial.available()) {

        while (Serial.available()!=0) {
            if (_m_buf_count<M_BUF_SIZE) {
                _m_buf[_m_buf_count]=Serial.read();
                _m_buf_count++;
            }


        }
        _udp.beginPacket(ip, udp_port);
        for (int x=0;x<_m_buf_count;x++) {
            _udp.write(_m_buf[x]);
        }
        _udp.endPacket();
    }
    //digitalWrite(SerialTxControl, RS485Transmit);
    //delay(500);
    ReadTariffSum(1363);
    //delay(500);
    //digitalWrite(SerialTxControl, RS485Receive);
}

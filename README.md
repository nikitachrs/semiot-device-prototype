# Arduino ESP8266 WiFI module SemIoT device prototypes.

Based on [esp8266 Arduino IDE libraries](https://github.com/esp8266/Arduino)

## Russian

Текушая версия на основе ESP8266 Wemos D1 mini.

### Сборка модуля

![photo example](https://raw.githubusercontent.com/semiotproject/semiot-device-prototype/issue-19/doc/mcs-dev-prototype/example.jpg)

![bb scheme](https://raw.githubusercontent.com/semiotproject/semiot-device-prototype/issue-19/doc/mcs-dev-prototype/semiot-device-mcs_bb.png)

### Тестирование

Я предпочитаю [SMCP](https://github.com/darconeous/smcp/):

+ get: `smcpctl get "coap://dev_ip/.well-known/core"`
+ put: `smcpctl put -i "coap://dev_ip/led1" "{\"@context\":\"/config/context\",\"pwm-value\":\"50\"}"`, где pwm-value устанавливаем в 50
+ observe: `smcpctl observe "coap://dev_ip/tick1"`

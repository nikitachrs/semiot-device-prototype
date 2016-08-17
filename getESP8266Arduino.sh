#!/bin/bash
git clone https://github.com/esp8266/Arduino.git
mv Arduino esp8266
cd esp8266/tools
python get.py
cd ..
git checkout tags/2.3.0

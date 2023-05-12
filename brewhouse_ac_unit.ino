
//for DS18B20 temperature sensor
#include <OneWire.h> // https://www.pjrc.com/teensy/td_libs_OneWire.html
#include <DallasTemperature.h> // https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/DallasTemperature.cpp
//uFire SHT20 from board manager

#include "uFire_SHT20.h"

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>

#include <EEPROM.h>

//webserver from tutorial https://randomnerdtutorials.com/esp32-esp8266-input-data-html-form/

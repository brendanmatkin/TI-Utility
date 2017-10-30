#ifndef SETTINGS_H
#define SETTINGS_H

#include "Arduino.h"
#include <ESP8266WiFi.h>
// #include <WebSocketsServer.h>
// #include <Hash.h>
#include <WiFiUDP.h>

#define SERIAL_DEBUG true                     // enabling this will disable the LEDDAR and vice versa

//#define FW_VERSION "0.2.0"
#define FW_PHASE   ""
#define HW_VERSION "1.0.1"
#define HW_PHASE   ""
const int   FW_VERSION  = 0;           // 0.0.00, e.g. 1.2.10 = 1210, 0.5.9 = 509, no leading 0s or it is interpreted as octal, whoops!
//const char* fwUrlBase   = "http://10.0.3.100:8888/";      // MAMP server
//const char* fwUrlBase   = "http://10.0.3.100:7002/fw/";   // node server
const char* fwUrlBase   = "http://10.0.3.100/";             // MAMP server port 80
const char* fwName      = "field_fw";
bool  fwAutoCheck       = false;     // true: check w/ check frequency, false: check with button
float fwCheckFrequency  = 0.25f;     // check for new FW every 4 seconds.
float fwCheckButtonTime = 2000.0f;   // how long to hold button down.

uint8_t address = 99;                         // set by DIPs (if you see 99 then it's wrong!)
String deviceName = "FieldWiFi_";             // i.e. FieldWiFi_001

const char* ssid = "FIELD";
const char* password = "nosotros";

IPAddress staticIP(10, 0, 3, address);        // static IP  - use staticIP[index] to get & set individual octets
IPAddress gateway(10, 0, 3, 254);             // gateway (for static)
IPAddress subnet(255, 255, 255, 0);           // subnet (for static)

// WebSocketsServer webSocket = WebSocketsServer(81);

#define LED_BUILTIN   2                 // not correctly mapped for ESP-12x
#define BOOTLOAD_PIN  0                 // BOOTLOAD button


#endif /* SETTINGS_H */


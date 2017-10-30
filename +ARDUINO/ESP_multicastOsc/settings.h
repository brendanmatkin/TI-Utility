#ifndef SETTINGS_H
#define SETTINGS_H

#include "Arduino.h"
#include <ESP8266WiFi.h>
// #include <WebSocketsServer.h>
// #include <Hash.h>
#include <ESP8266mDNS.h>    // necessary?
#include <WiFiUDP.h>
#include "OSCMessage.h"

#define SERIAL_DEBUG true                     // enabling this will disable the LEDDAR and vice versa

//#define FW_VERSION "0.2.0"
#define FW_PHASE   "-beta"
#define HW_VERSION "1.0.1"
#define HW_PHASE   ""
const int FW_VERSION = 307;         // 0.3.06, e.g. 1.2.10 = 1210, 0.5.9 = 509, no leading 0s or it is interpreted as octal, whoops!
float fwCheckFrequency = 0.25f;     // check for new FW every 4 seconds.
const char* fwUrlBase = "http://10.0.3.100/";
const char* fwName = "field_fw";


uint8_t address = 99;                         // set by DIPs (if you see 99 then it's wrong!)
String deviceName = "FieldWiFi_";             // i.e. FieldWiFi_001

const char* ssid = "FIELD";
const char* password = "nosotros";

float sendFrequency = 20.0f;       // OSC send rate (fps/Hz)
//float maxDist = 3000.0f;              // distance from leddar is in mm?
float maxDist = 3500.0f;
//bool sendStatus = false;

IPAddress staticIP(10, 0, 3, address);        // static IP  - use staticIP[index] to get & set individual octets
IPAddress gateway(10, 0, 3, 254);             // gateway (for static)
IPAddress subnet(255, 255, 255, 0);           // subnet (for static)
IPAddress oscIP(10,0,3,100);                  // Server
uint16_t localPort_OSC = 8889;                // OSC unicast listen port
uint16_t remotePort_OSC = 8888;               // OSC unicast send port
IPAddress mIP(239, 0, 3, 1);                  // multicast ip address
uint16_t mPort = 7777;                        // multicast port

// WebSocketsServer webSocket = WebSocketsServer(81);

#define LED_BUILTIN 2                 // not correctly mapped for ESP-12x

#endif /* SETTINGS_H */


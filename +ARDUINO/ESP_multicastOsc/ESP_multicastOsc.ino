#include <TI_OTA.h>
#include "settings.h"
#include <Wire.h>
#include "FS.h"
#include "Adafruit_MCP23017.h"



WiFiUDP udp;
Adafruit_MCP23017 io;
TI_OTA ota;

String oscAddress;
uint32_t sendTimer;

bool ledToggled = false;

void routeAll(OSCMessage &msg, int addrOffset ) {
  float testVal = -1.0;
  if (!msg.isFloat(0)) {
    if (SERIAL_DEBUG)Serial.printf("routeAll(%c) called\n", msg.getType(0));
  }
  else {
    testVal = msg.getFloat(0);
    if (SERIAL_DEBUG) Serial.printf("routeAll: %s\n", String(testVal).c_str());
  }
}

void toggleLed(OSCMessage &msg, int addrOffset) {
  if (!msg.isInt(0)) {
    if (SERIAL_DEBUG)Serial.printf("Warning: toggleLed(%c) called (arg not int)\n", msg.getType(0));
  }
  else {
    ledToggled = msg.getInt(0);
    if (ledToggled > 1) ledToggled = 1;
    if (ledToggled < 0) ledToggled = 0;
    digitalWrite(LED_BUILTIN, !ledToggled);
    if (SERIAL_DEBUG) Serial.printf("toggleLed: %s\n", ledToggled ? "true" : "false");
  }
}


/*------ UPDATE ADDRESS ------*/
void updateAddress() {
  for (size_t i = 0; i < 8; i++) {
    bitWrite(address, i, 1 - io.digitalRead(i));
  }
}





/*---------- SETUP ---------*/
void setup() {

  if (SERIAL_DEBUG) {
    Serial.begin(115200);
    Serial.printf("\n");
    delay(500);
    Serial.printf("\n\n\nFieldWiFi\n");
    Serial.printf("FW: %u%s   ", FW_VERSION, FW_PHASE);
    Serial.printf("HW: %s%s\n", HW_VERSION, HW_PHASE);
    Serial.printf("(c)2017 Tangible Interaction Design Inc.\n\n");
  }


  // begin SPIFFS
  if (SERIAL_DEBUG) Serial.print("Mounting file system...");
  if (!SPIFFS.begin()) {
    if (SERIAL_DEBUG) Serial.println("Failed to mount file system");
    return;
  }
  if (SERIAL_DEBUG) Serial.println(" mounted.");


  // init mcp23017 and get the address:
  if (SERIAL_DEBUG) Serial.print("Getting physical address...");
  Wire.begin();                     // used for mcp23017 and the (optional) LIDAR Lite v3 if populated
  io.begin();
  for (size_t i = 0; i < 8; i++) {  // the first 8 are the DIPs
    io.pinMode(i, INPUT);
    io.pullUp(i, HIGH);
  }
  // io.pinMode(7, INPUT);   // leddar interrupt. Current not used. Active high when new measurement is available.
  // io.pinMode(8, OUTPUT);  // leddar reset. Active low. Pullup on module.
  //io.pullUp (8, HIGH);  // leddar has pullup.
  delay(200);             // let the thing settle!
  updateAddress();        // get address from DIPs
  if (SERIAL_DEBUG) Serial.printf(" address=%u\n", address);
  if (address < 10) deviceName.concat("00");
  else if (address < 100) deviceName.concat("0");
  deviceName.concat(address);
  yield();


  // init LED PIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledToggled);   // low is on

  // connect to WiFi
  if (SERIAL_DEBUG) Serial.print("Connecting to ");
  if (SERIAL_DEBUG) Serial.print(ssid);
  WiFi.persistent(false);                   //
  WiFi.mode(WIFI_STA);                      //
  WiFi.hostname(deviceName);                // DHCP Hostname
  staticIP[3] = address;                    // set 4th octect with hardware address
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);


  /* auto reset if it's not connecting (occasionally hangs otherwise) */
  int restartCounter = 0;
  while (!WiFi.isConnected()) {
    delay(100);
    if (SERIAL_DEBUG) Serial.print(".");
    restartCounter++;
    if (restartCounter > 25) ESP.restart();  // if it takes more than ~2.5 seconds to connect, restart!
  }
  if (SERIAL_DEBUG) Serial.println("  connected."); // yay it worked!
  yield();

  //ota.begin(&deviceName);


  /* initialize OSC */
  oscAddress = "/" + deviceName;
  //msg.setAddress(oscAddress.c_str());

  //  if (SERIAL_DEBUG) Serial.print("\nOpening UDP port for OSC...");
  //  if (udp.begin(localPort_OSC)) {
  //    if (SERIAL_DEBUG) Serial.printf("listening on %u\n", localPort_OSC);
  //  }
  //  else if (SERIAL_DEBUG) Serial.printf("(! port %u not available)\n", localPort_OSC);
  if (SERIAL_DEBUG) Serial.print("\nJoining multicast group...");
  if (udp.beginMulticast(WiFi.localIP(), mIP, mPort)) {
    if (SERIAL_DEBUG) {
      Serial.printf("listening on multicast group: ");
      Serial.print(mIP);
      Serial.printf(":%u\n", mPort);
    }
  }
  else if (SERIAL_DEBUG) {
    Serial.printf("(could not join multicast group at: ");
    Serial.print(mIP);
    Serial.printf(":%u\n", mPort);
  }


  /* who am I this time?  */
  delay(100);
  //if (SERIAL_DEBUG) Serial.printf("\nWiFi connected.\n");
  if (SERIAL_DEBUG) {
    Serial.printf("\nIP address:  ");
    Serial.println(WiFi.localIP());
    Serial.printf("OSC Address: %s\n", oscAddress.c_str());
    Serial.printf("%s (%s) ready. \n", deviceName.c_str(), WiFi.macAddress().c_str());
    Serial.printf("\n---\n\n");
  }
  delay(100);

  sendTimer = millis();
  yield();                  // precaution to make sure the stack is ready..
}
/*---------- END SETUP ---------*/




/*----------- LOOP -----------*/
unsigned long otaTimer;


void loop() {
  if (millis() - otaTimer > (1000.0f / fwCheckFrequency)) {
    //digitalWrite(LED_BUILTIN, LOW);
    ota.checkServer(fwUrlBase, fwName, FW_VERSION);
    if (SERIAL_DEBUG) Serial.println("chk fw");
    otaTimer = millis();
  }
  //digitalWrite(LED_BUILTIN, HIGH);


  /**** OSC in ****/
  OSCMessage inMsg;
  int udpSize = udp.parsePacket();
  if (udpSize > 0) {
    if (SERIAL_DEBUG) {
      Serial.printf("received packet from: ");
      Serial.println(udp.destinationIP());
    }
    while (udpSize--) {
      inMsg.fill(udp.read());
    }
    if (!inMsg.hasError()) {

      int callbackGood = inMsg.route("/*", routeAll);
      callbackGood += inMsg.route("/toggleLed", toggleLed);
      if (SERIAL_DEBUG) Serial.printf("Good Message (%u)\n", callbackGood);
    }
  }
}



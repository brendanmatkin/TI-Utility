#include <TI_OTA.h>
#include "settings.h"
#include "FS.h"


/* // for FIELD modules:
  #include <Wire.h>
  #include "Adafruit_MCP23017.h"
*/


WiFiUDP udp;
TI_OTA ota;



/*---------- SETUP ---------*/
void setup() {

  if (SERIAL_DEBUG) {
    Serial.begin(115200);
    Serial.printf("\n");
    delay(500);
    Serial.printf("\n\n\nOTA server checker (empty shell)\n");
    Serial.printf("FW: %u\n", FW_VERSION);
    //Serial.printf("FW: %u%s   ", FW_VERSION, FW_PHASE);
    //Serial.printf("HW: %s%s\n", HW_VERSION, HW_PHASE);
    Serial.printf("(c)2017 Tangible Interaction Design Inc.\n\n");
  }

  /* // For FIELD modules to get hardware address:
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
  */


  // init LED PIN
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BOOTLOAD_PIN, INPUT);      // external pullup
  digitalWrite(LED_BUILTIN, HIGH);   // low is on

  // connect to WiFi
  if (SERIAL_DEBUG) Serial.print("Connecting to ");
  if (SERIAL_DEBUG) Serial.print(ssid);
  WiFi.persistent(false);                   //
  WiFi.mode(WIFI_STA);                      //
  WiFi.hostname(deviceName);                // DHCP Hostname
//  staticIP[3] = address;                    // set 4th octect with hardware address
//  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);

  // TODO connect dynamically! Why not working? 


  /* auto reset if it's not connecting (occasionally hangs otherwise) */
  int restartCounter = 0;
  while (!WiFi.isConnected()) {
    delay(100);
    if (SERIAL_DEBUG) Serial.print(".");
    restartCounter++;
    if (restartCounter > 50) ESP.restart();  // if it takes more than ~5 seconds to connect, restart!
  }
  if (SERIAL_DEBUG) Serial.println("  connected."); // yay it worked!
  yield();


  /* who am I this time?  */
  delay(100);
  //if (SERIAL_DEBUG) Serial.printf("\nWiFi connected.\n");
  if (SERIAL_DEBUG) {
    Serial.printf("IP address:  ");
    Serial.println(WiFi.localIP());
    Serial.printf("%s (%s) ready. \n", deviceName.c_str(), WiFi.macAddress().c_str());
    Serial.printf("\n---\n\n");
    Serial.printf("** DON'T FORGET TO POWER CYCLE AFTER 1st UPLOAD! **\n\n");
  }
  delay(100);

  yield();                  // precaution to make sure the stack is ready..
}
/*---------- END SETUP ---------*/




unsigned long otaTimer, buttonTimer;

void otaUpdate() {
  if (fwAutoCheck) {
    if (millis() - otaTimer > (1000.0f / fwCheckFrequency)) {
      digitalWrite(LED_BUILTIN, LOW);
      ota.checkServer(fwUrlBase, fwName, FW_VERSION);       // (( Server OTA ))
      otaTimer = millis();
      if (SERIAL_DEBUG) Serial.println("check fw (auto)");
    }
  } else {
    if (digitalRead(BOOTLOAD_PIN) == HIGH) {
      buttonTimer = millis();   // reset timer
    }
    else if (digitalRead(BOOTLOAD_PIN) == LOW && millis() - buttonTimer >= fwCheckButtonTime) {
      digitalWrite(LED_BUILTIN, LOW);
      ota.checkServer(fwUrlBase, fwName, FW_VERSION);       // (( Server OTA ))
      otaTimer = millis();
      if (SERIAL_DEBUG) Serial.println("check fw (button)");
    }
  }
  digitalWrite(LED_BUILTIN, HIGH);
}


unsigned long bangTimer;
uint8_t bangCounter;
bool heartBeat() {
  if (millis() - bangTimer > 1000) {
    bangCounter++;
    //Serial.printf("bang! (%u)\n", bangTimer);
    if (SERIAL_DEBUG) Serial.print("* ");
    if (bangCounter >= 10) {
      bangCounter = 0;
      if (SERIAL_DEBUG) Serial.println();
    }
    bangTimer = millis();
    return true;
  }
  return false;
}


/*----------- LOOP -----------*/
void loop() {
  //ota.loop(); --> requires ota.begin(&deviceName);      // (( Arduino OTA ))
  heartBeat();
  otaUpdate();
}
/*----------- END LOOP -----------*/


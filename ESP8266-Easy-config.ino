
/* 
 *  Tested with Arduino IDE 1.6.8
 * Please install the following libraries:
 * https://github.com/bblanchon/ArduinoJson
 * https://github.com/Imroy/pubsubclient
 */

#define ErrorLed 13

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266SSDP.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DNSServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <PubSubClient.h>

WiFiClient wclient;
PubSubClient mqttClient(wclient);
File fsUploadFile;
Ticker wifiErrTick;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
DNSServer dnsServer;

boolean isAP=false,hasCustomPage=false,hasMqtt=false,subscribedMqtt=false;
String ssidN="",ssidP="",session="",uName="",uPass="",apName="",apPass="",chipId="",mqttBase="easyESP";
unsigned long time1=0,time2=0;
const char* mqttUser = "";
const char* mqttPass = "";

void mPublish(String mTopic, String mPayload, int mQos=0, boolean mRetain=false);

void setup() {
  Serial.begin(115200);
//  Serial.swap();  // uncomment if you waut to use GPIO1 and GPIO3 as INPUT/OUTPUT
  Serial.print("Start ");
  loadConfig();
  wifiErrTick.attach(ssidN=="" ? 0.3 : 1.3, WifiErrLed);
  sendInfo();
  startWifi();
  configServer();
  pinMode(0, INPUT);
  mqttSetup();
  customSetup();
}

void loop() {
  httpServer.handleClient();
  checkClean();
  if (isAP) {
    dnsServer.processNextRequest();
    if (millis()>time1+300000) rst();
  } else {
    if (WiFi.status() != WL_CONNECTED) rst();
    mqttLoop();
  }
  customLoop();
}

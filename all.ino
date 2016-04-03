//WiFi.forceSleepBegin();
//WiFi.forceSleepWake();

#define ErrorLed 13

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266SSDP.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DNSServer.h>
#include "FS.h"
#include <Ticker.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

Ticker wifiErrTick;
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
File fsUploadFile;
DNSServer dnsServer;
boolean isAP=false ;
String ssidN="",ssidP="",session="",uName="",uPass="",apName="",apPass="";
unsigned long time1=0,time2=0;

#include <OneWire.h>
OneWire  ds(14); 

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
}

void loop() {
  httpServer.handleClient();
  checkClean();
  if (isAP) {
    dnsServer.processNextRequest();
    if (millis()>time1+300000) reset();
  } else {
    if (WiFi.status() != WL_CONNECTED) reset();
  }
}


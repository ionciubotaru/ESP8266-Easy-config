String showArgs() {
  String message = "URI: ";
  message += httpServer.uri();
  message += "\nMethod: ";
  message += (httpServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += httpServer.args();
  message += "\n";
  for (uint8_t i=0; i<httpServer.args(); i++){
    message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
  }
  return message;
}

void handleSave(){
  time1=millis();
  Serial.println(showArgs());
  String page = httpServer.arg("page");
  if ( page == "pageLogin" ) { 
    if (httpServer.arg("user") == uName and httpServer.arg("pass") == uPass) {
      httpServer.send(200, "text/json", "{\"error\":\"0\", \"sess\": \""+session+"\"}");
    } else {
      httpServer.send(200, "text/json", "{\"error\":\"1\"}");
    }
    return;
  }
  if (httpServer.arg("sess") != session) {
    httpServer.send(404, "text/html", "Error");
    return;
  }
  if ( page == "getWIFI") {
    httpServer.send(200, "text/json", scanWifi());
    return;
  } else if ( page == "getDevice") {
     httpServer.send(200, "text/json", "{\"uName\":\""+uName+"\",\"uPass\":\""+uPass+"\",\"apName\":\""+apName+"\",\"apPass\":\""+apPass+"\" }");
    return;
  } else if (page == "pageWIFI") {
    ssidN = httpServer.arg("ssid");
    ssidP = httpServer.arg("pass");
    if (ssidN == "NO WIFI") {
      ssidN = "";
      ssidP = "";
    }
    writeEeprom(100,50,ssidN);
    writeEeprom(150,50,ssidP);
  } else if (page == "pageDevice") {
    uName = httpServer.arg("uName");
    uPass = httpServer.arg("uPass");
    apName = httpServer.arg("apName");
    apPass = httpServer.arg("apPass");
    writeDevice();
  } else if (page == "pageStatus") { 
    File configFile = SPIFFS.open("/gpio.txt", "r");
    if (!configFile) {
      Serial.println("Failed to open gpio file");
    } else {
      size_t size = configFile.size();
      if (size > 1023) {
        Serial.println("Config file too large");
      } else {
        String gpio = configFile.readString();
        httpServer.send(200, "text/json", gpio);
        return;
      }
    }
  } else if (page == "pageGPIO" or page == "pageGraph") { 
    int gpio = httpServer.arg("gpio").toInt();
    String val = (digitalRead(gpio)==0 ? "0" : "1" );
    if (gpio == 13) val = (val=="0" ? "1" : "0");
    httpServer.send(200, "text/json", "{\"value\":\""+val+"\"}");
    return;
  } else if (page == "changeGPIO") { 
      int gpio = httpServer.arg("gpio").toInt();
      int val = httpServer.arg("value").toInt();
      int val1=val;
      if (gpio == 13) val1 = (val==0 ? 1 : 0);
      if (gpio != 0) {
        pinMode(gpio, OUTPUT);
        digitalWrite(gpio, val1);
      }
      httpServer.send(200, "text/json", "{\"value\":\""+String(val==0 ? "0" : "1")+"\"}");
      return;
  } else if (page == "commit") reset(); 
  httpServer.send(200, "text/html", "Ok");
}

String scanWifi() {
  time1=millis();
  Serial.println("Scan WIFI");
  int n = WiFi.scanNetworks();
  String output = "[";
  for (int i = 0; i < n; i++) {
    if (output != "[") output += ',';
    String ssid = WiFi.SSID(i);
    output += "{\"wifi\": \""+ssid+
      "\",\"rssi\": \""+WiFi.RSSI(i)+"\",\"def\": \""
      +((ssid == ssidN)?"1":"0")+"\",\"enc\": \""
      +((WiFi.encryptionType(i) == ENC_TYPE_NONE)?"0":"1")+"\", \"pass\": \""+((ssid == ssidN) ? ssidP : "")+"\"}";
    delay(10);
  }
  output += ",{\"wifi\": \"NO WIFI\", \"rssi\": \"\", \"def\": \"0\", \"enc\": \"0\", \"pass\": \"\"} ]";
  return output;
}

void graph () {
  time1=millis();
  int gpio = httpServer.arg("gpio").toInt();
  String val = (digitalRead(gpio)==0 ? "0" : "1" );
  String json = "{ \"analog\":\""+val+"\" }";
  httpServer.send(200, "text/json", json);
}


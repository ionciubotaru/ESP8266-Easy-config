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
  customPageNoAuth();
  if (hasCustomPage) return;
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
    httpServer.send(200, "text/json", readGpioFile());
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
      gpioOut(gpio,val1);
      httpServer.send(200, "text/json", "{\"value\":\""+String(val==0 ? "0" : "1")+"\"}");
      return;
  } else if (page == "commit") {
    rst(); 
  } else {
    customPageAuth();
    if (hasCustomPage) return;
  }
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
  String val = gpioIn(gpio);
  String json = "{ \"analog\":\""+val+"\" }";
  httpServer.send(200, "text/json", json);
}

void gpioOut(uint8_t gpio, uint8_t val1) {
  if (gpio != 0 and gpio <= 16 and (val1==0 or val1==1)) {
    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, val1);
    mPublish("A/"+String(gpio),String(val1));
  } else {
    Serial.println("Invalid GPIO request");
    Serial.println(gpio);
    Serial.println(val1);
  }
}

String gpioIn(uint8_t gpio) {
  return (digitalRead(gpio)==0 ? "0" : "1" );
}


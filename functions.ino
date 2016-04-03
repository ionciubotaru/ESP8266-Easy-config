void reset() {
  httpServer.send(200, "text/html", "Rebooting .....");
  Serial.println("Rebooting .....");
  delay(1000);
  ESP.restart();
}

void startWifi() {
  char ssidN1[ssidN.length()+1],ssidP1[ssidP.length()+1];
  ssidN.toCharArray(ssidN1, ssidN.length()+1);
  ssidP.toCharArray(ssidP1, ssidP.length()+1);
  WiFi.disconnect();
  if (ssidN == "") {
    startAP();
  } else {
    Serial.print("Start WIFI: ");
    Serial.println(ssidN1);
    WiFi.mode(WIFI_STA);
    if (ssidP == "") {
      WiFi.begin(ssidN1);
    } else {
      WiFi.begin(ssidN1,ssidP1);
    }
    while (WiFi.status() != WL_CONNECTED and millis()<30000) delay(10); 
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(WiFi.localIP());
      ssdp();
      SSDP.begin();
      wifiErrTick.detach();
      digitalWrite(ErrorLed, HIGH);
    } else {
      startAP();
    }
  }
}

void startAP() {
  isAP=true;
  String apN2 = apName+"-"+String(ESP.getChipId());
  char apN1[apN2.length()+1],apP1[apPass.length()+1];
  apN2.toCharArray(apN1, apN2.length()+1);
  apPass.toCharArray(apP1, apPass.length()+1);
  Serial.print("Start AP: ");
  Serial.println(apN1);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(172,31,31,1), IPAddress(172,31,31,1),IPAddress(255,255,255,0));
  if (apPass == "") {
    WiFi.softAP (apN1);
  } else {
    WiFi.softAP (apN1,apP1);
  }
  dnsServer.start(53, "*", IPAddress(172,31,31,1));
}

void checkClean() {
  if (digitalRead(0)==1) time2=millis();
  if (millis()>time2+5000) clearEeprom();
}


char rnd() {
  while (true) {
    char i = random(48,122);
    if (not ((i>57 and i<65) or (i>90 and i<97))) {
      return i;
    }
  }
}

void WifiErrLed() {
  pinMode(ErrorLed, OUTPUT);
  digitalWrite(ErrorLed, !digitalRead(ErrorLed));
}

String readEeprom(int start, int len) {
  String tmp="";
  for (int i=start; i<(len+start); i++) {
    char tmp1 = EEPROM.read(i);
    if (tmp1 == 126) return tmp;
    tmp += tmp1;
  }
}

void writeEeprom(int start, int len, String tmp) {
  int sSize = tmp.length();
  for (int i=start; i<(len+start); i++) {
    if (i<(sSize+start)) {
      EEPROM.write(i, tmp[i-start]);
    } else {
      EEPROM.write(i, 126);
    }
  }
  EEPROM.commit();
}

void clearEeprom() {
  Serial.println("Clear Eeprom");
  writeEeprom(100,50,"");
  writeEeprom(150,50,"");
  uName="admin";
  uPass="admin";
  ssidN="";
  ssidP="";
  apName="ESP";
  apPass="";
  writeDevice();
  ESP.restart();
}

void writeDevice() {
  writeEeprom(0,50,uName);
  writeEeprom(50,50,uPass);
  writeEeprom(200,50,apName);
  writeEeprom(250,50,apPass);
}


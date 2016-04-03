void loadConfig() {
  for (int i=0; i<32; i++) {
    session += rnd();
  }
  Serial.println(session);
  EEPROM.begin(512);
  uName=readEeprom(0,50);
  uPass=readEeprom(50,50);
  ssidN=readEeprom(100,50);
  ssidP=readEeprom(150,50);
  apName=readEeprom(200,50);
  apPass=readEeprom(250,50);
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system..rebooting....");
    format();
    ESP.restart();
  }
}

void configServer(){
  httpServer.on("/format", HTTP_GET, format);
  httpServer.on("/info", HTTP_GET, sendInfo);
  httpServer.on("/list", HTTP_GET, handleFileList);
  httpServer.on("/edit", HTTP_GET, [](){if(!handleFileRead("/edit.htm")) httpServer.send(404, "text/plain", "FileNotFound"); });
  httpServer.on("/edit", HTTP_PUT, handleFileCreate);
  httpServer.on("/edit", HTTP_DELETE, handleFileDelete);
  httpServer.on("/edit", HTTP_POST, [](){ httpServer.send(200, "text/plain", ""); }, handleFileUpload);
  httpServer.on("/save", HTTP_POST, handleSave);
  httpServer.on("/reset", HTTP_GET, reset);
//  httpServer.on("/clear", HTTP_GET, clearEeprom);
  httpServer.on("/description.xml", HTTP_GET, [](){ SSDP.schema(httpServer.client()); });
  httpServer.on("/all", HTTP_GET, graph);
  httpServer.onNotFound([](){
    time1=millis();
    if(!handleFileRead(httpServer.uri()))
      if (isAP) {
        handleFileRead("/redirect.htm");
      } else {
        httpServer.send(404, "text/plain", "File Not Found\n\n"+showArgs());
      }
  });
  httpUpdater.setup(&httpServer);
  httpServer.begin();
}

void ssdp() {
  Serial.println("SSDP start");
  SSDP.setName("ESP");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setURL("/");
  File ssdpFile = SPIFFS.open("/ssdp.txt", "r");
  if (ssdpFile) {
    size_t size = ssdpFile.size();
    if (size < 512) {
      std::unique_ptr<char[]> buf(new char[size]);
      ssdpFile.readBytes(buf.get(), size);
      StaticJsonBuffer<512> jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      if (json.success()) {
        const char* tmp = json["Name"];
        String sir1 = String(ESP.getChipId());
        String sir2 = String(tmp)+"-"+sir1;
        char sir3[sir2.length()+1];
        sir2.toCharArray(sir3, sir2.length()+1);
        SSDP.setName(sir2);
        SSDP.setSerialNumber(sir1);
        tmp = json["ModelName"];
        SSDP.setModelName(String(tmp).length()==0 ? "NOT set" : tmp);
        tmp = json["ModelNumber"];
        SSDP.setModelNumber(String(tmp).length()==0 ? "NOT set" : tmp);
        tmp = json["ModelURL"];
        SSDP.setModelURL(String(tmp).length()==0 ? "NOT set" : tmp);
        tmp = json["Manufacturer"];
        SSDP.setManufacturer(String(tmp).length()==0 ? "NOT set" : tmp);
        tmp = json["ManufacturerURL"];
        SSDP.setManufacturerURL(String(tmp).length()==0 ? "NOT set" : tmp);
      }
    }
  }
  ssdpFile.close();
}

void sendInfo() {
  uint32_t realSize = ESP.getFlashChipRealSize();
  Serial.print("Flash real size: ");
  Serial.println(int(realSize/1024));
  FSInfo fs_info;
  SPIFFS.info(fs_info);
  Serial.print("FS size: ");
  Serial.println(int(fs_info.totalBytes/1024));
  Serial.print("FS free: ");
  Serial.println(int((fs_info.totalBytes-fs_info.usedBytes)/1024));
  Serial.println("Files:");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {    
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
  }
  Serial.print("user: ");
  Serial.print(uName);
  Serial.print(" pass: ");
  Serial.println(uPass);
  Serial.print("wifi: ");
  Serial.print(ssidN);
  Serial.print(" pass: ");
  Serial.println(ssidP);
  Serial.print("AP: ");
  Serial.print(apName);
  Serial.print(" pass: ");
  Serial.println(apPass);
  Serial.println("");
  httpServer.send(200, "text/plain", "Ok");
}


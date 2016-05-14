void callback(const MQTT::Publish& pub) {
  String order=pub.payload_string();
  String rTopic=pub.topic();
  Serial.print("New message: ");Serial.println(rTopic+" ~ "+order);
//   /chipId/in/I/gpio => 0|1
//   /chipId/in/O => gpio
  int len=rTopic.length();
  if (rTopic[len-4]=='O') {
    gpioOut(rTopic.substring(len-2).toInt(),order.toInt());
  } else if (rTopic[len-1]=='I') {
    String out = gpioIn(order.toInt());
    mPublish("A/"+order, out);
  }
}

void mqttSetup() {
  if (!isAP) {
    File mqttFile = SPIFFS.open("/mqtt.txt", "r");
    if (mqttFile) {
      size_t size = mqttFile.size();
      if (size < 512) {
        std::unique_ptr<char[]> buf(new char[size]);
        mqttFile.readBytes(buf.get(), size);
        StaticJsonBuffer<512> jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        if (json.success()) {
          const char* server = json["Server"];
          const char* port = json["Port"];
          uint16_t iPort = String(port).toFloat();
          mqttBase = json["Base"].asString();
          mqttUser = json["User"];
          mqttPass = json["Pass"];
          mqttClient.set_callback(callback);
          mqttClient.set_server(server, iPort);
          hasMqtt=true;
          Serial.print("MQTT start: ");
          Serial.print(server);
          Serial.print(" port ");
          Serial.println(iPort);
          time1=millis();
        }
      }
    }
    mqttFile.close();
    time1=millis()-25000;
  }
}

void mqttLoop() {
  if (hasMqtt) {
    if (!mqttClient.connected() and millis()>time1+30000) {
      if (mqttClient.connect(MQTT::Connect(chipId)
                                  .set_auth(mqttUser, mqttPass)
                                  .set_clean_session(false)
                                  .set_will(mqttBase+"/"+chipId+"/status", "0",1,true)
                                  .set_keepalive(60)
                         )) {
        Serial.println("Connected to MQTT server");
        if (mqttClient.connected() and !subscribedMqtt) {
          mPublish("status","1",1);
          mPublish("name",apName,1);
          mPublish("gpio",readGpioFile(),1);
          mqttClient.subscribe(mqttBase+"/"+chipId+"/in/#");
          subscribedMqtt=true;
        } else {
          Serial.println("Reseting");
          rst();
        }
      } else {
        Serial.println("Could not connect to MQTT server");
      }
      time1=millis();
    }
    if (mqttClient.connected()) {
      mqttClient.loop();
    }
  }
}






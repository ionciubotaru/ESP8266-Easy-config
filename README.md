# ESP8266-Easy-config
Easy configuration web interface fost ESP8266 based on jquery mobile

## Minimum requirements
* ESP8266 with 1Mb Eprom
* Arduino IDE / Tools / Flash Size - select 1Mb (256Kb SPIFFS)

## Instalation
1. Install missing libraries:
   * https://github.com/bblanchon/ArduinoJson
   * https://github.com/Imroy/pubsubclient
2. Upload the sketch
3. After the first boot keep the GPIO0 button press for 5 seconds
4. A new wireless AP named ESP - <sn> will appear 
5. After first connection upload the files into DATA folder using: upload.bat 172.31.31.1
6. Open http://172.31.31.1 in your browser - default username/password is: admin/admin
7. Goto wireless setup, select your WIFI AP name and type your password
8. Press the commit button
9. After reboot ESP will connect top your WIFI - check Network neighborhood to find your ESP
10. Upload new sketch using OTA

## Configuration 
1. To change the username, password, WIFI and AP name please connect to http://esp_ip/
2. To edit files stored in FS http://esp_ip/edit

#### File gpio.txt 
Default values for Itead Sonoff https://www.itead.cc/sonoff-wifi-wireless-switch.html :

[

	{"gpio":  "0","name": "Button GPIO 0","mode": "3","active": "AL","min": "0","max": "1"},
	{"gpio": "12","name": "Relay","mode": "1", "active": "AH"},
	{"gpio": "13","name": "Led","mode": "1", "active": "AL"}
	
]

* gpio: GPIO nummber
* name: friendly name for web interface
* mode: 0 - input, 1 - output, 3 - input display grafic
* active: AL - active low, AH - active high (for output mode only)
* min: minimum display value - for mode 3 only
* max: maximum display value - for mode 3 only

#### File ssdp.txt :
{

	"Name": "ESP",
	"ModelName": "Your model here",
	"ModelNumber": "001",
	"ModelURL": "http://www.example.com/model",
	"Manufacturer": "Your name Here",
	"ManufacturerURL": "http://www.example.com"

}

#### File mqtt.txt :
{

	"Base": "easyESP",
	"Server": "test.mosquitto.org",
	"Port": "1883",
	"User": "",
	"Pass": ""

}

The mosquitto server is public and open. Change the "Base" to your unique name to avoid interferances with other users.

The base must be the same with the one specified in index.html from web folder.

#### WEB folder :

Contains the pages used to control the ESP's from internet. 
On mobile devices works fine on Opera and very recent version of Chrome mobile.
On computer works fine on Chrome.

### To do:

1. Add Thingspeak support https://thingspeak.com/
2. Create mobile aplication to control ESP's




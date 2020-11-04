#include <PubSubClient.h> 


/* sample json file
 *  {"id": "Master_Switch", "ip": "192.168.1.110", "port": 1883, "inTopic": "LaCima/OP_STATION_1/Input", 
 *  "heartBeat": "LaCima/OP_STATION_1/Heat_Beat", "reboot": "LaCima/OP_STATION_1/Reboot", "button_push": "LaCima/OP_STATION_1/Button_Push", "sensors": "LaCima/OP_STATION_1/Sensors"}
 */

String heartBeat;
String reboot;
String button_push;
String sensors;

void callback(char* topic, byte* payload, unsigned int length) {

  ;
}


WiFiClient wifiClient;
PubSubClient client(wifiClient);

void mqtt_setup(void)
{
  int i;
  // read mqtt setup data from sd card  
  // display mqtt setup data
  const char *id;
  const char *ip;
  int port;
  const char *intopic;
  
 
   File file = SD.open("mqtt.jsn");
   if( file == false )
   {
       lcd_flash("No mqtt.jsn",2000,500);
   }
   DeserializationError err = deserializeJson(doc, file);
   if (err) 
   {
      lcd_flash("JSON Error",2000,500);
   }
   id = doc["id"].as<char*>();
   ip = doc["ip"].as<char*>();
   port = doc["port"].as<int>();
   intopic = doc["inTopic"].as<char*>();
   heartBeat = String(doc["heartBeat"].as<char*>());
   reboot = String(doc["reboot"].as<char*>());
   button_push = String(doc["button_push"].as<char*>());
   sensors = String(doc["sensors"].as<char*>());
  

  client.setServer(ip, port);
  client.setCallback(callback);
  tft.fillScreen(TFT_BLACK); //Black background
  
  tft.drawString("Connecting to MQTT",5,40);
  delay(1000);
  for(i =0; i<5;i++)
  {
    if (client.connect(id))
    {
      
      // Once connected, publish an announcement...
      client.publish(reboot.c_str(), "Hello from Wio Terminal");
      // ... and resubscribe
      client.subscribe(intopic);
      tft.fillScreen(TFT_BLACK); //Black background
      tft.drawString("MQTT Connection",5,40);
      delay(1000);
      tft.fillScreen(TFT_BLACK); //Black background
      tft.drawString(reboot,5,40);
      tft.drawString(heartBeat,5,80);
      tft.drawString(button_push,5,120);
      tft.drawString(sensors,5,160);
      return;
    }
    else
    {
       tft.fillScreen(TFT_BLACK); //Black background
       delay(1000);
      tft.drawString("No MQTT Connection",5,40);
      delay(1000);
    }
  }

   
  
   NVIC_SystemReset();
 }

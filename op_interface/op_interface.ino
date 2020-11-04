
/* Things to do
 *  Inirialization
 *     0. Initial Scan  -- Record Data
 *     1. Initialize Wifi
 *     2. Initialize Mqtt
 *     3. Start Thread Watch dog
 *     4. Start WD Thread 
 *  
 *  
 *    
 *  . 
 *  
 *  
 *  
 *  
 */
 
#include <Seeed_Arduino_FreeRTOS.h>
#include "display_handler.h"



#include <ArduinoJson.h>
#include "cmp.h"





#include "sd_file.h"
#include "wifi.h"
//#include "mqtt_functions.h"
#include "task_management.h"
#include "redis_functions.h"
#include "op_button.h"
#include "screen_button.h"
#include "server_commands.h"



#define MY_TIME_TICK 20 // delay 20 ms 50 HZ



void system_reset(void)
{
   NVIC_SystemReset();
  
}



void setup()
{
    
    Serial.begin(115200);
    //while(!Serial);
    //Serial.println("Starting");
    
    initialize_display();
    initialize_sd();
    start_wifi();
    //mqtt_setup();
    setup_redis_service();
    create_tasks();
    
   

}





void loop() 
{
  
   
   while(1)
   {
      
      delay(MY_TIME_TICK); 
      
   }
}
  

 

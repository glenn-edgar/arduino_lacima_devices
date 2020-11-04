

#include "common_setup.h"
#include <Seeed_Arduino_FreeRTOS.h>
extern "C"{
  #include "wdtFunctions.h"
}
#include "task_management.h"
#include "display_handler.h"
#include "wifi.h"
#include "redis_functions.h"

#include "op_button.h"
#include "screen_button.h"
#include "server_commands.h"
#include "internal_sensors.h"

#define BASIC_STACK_SIZE 1024
static StackType_t display_stack[BASIC_STACK_SIZE];
static StackType_t wd_stack[BASIC_STACK_SIZE];
static StackType_t op_interface_stack[BASIC_STACK_SIZE];
static StackType_t internal_sensor_stack[BASIC_STACK_SIZE];
static StackType_t wifi_stack[BASIC_STACK_SIZE];
static StackType_t server_interface_stack[BASIC_STACK_SIZE];


StaticTask_t Handle_Wd_Task;
StaticTask_t Handle_Display_Task;
StaticTask_t Handle_Op_Interface_Task;
StaticTask_t Handle_Internal_Sensor_Task;
StaticTask_t Handle_wifi_Task;
StaticTask_t Handle_server_interface_Task;

static int send_reboot(int count)
{
    count = count+1;
    if(count == 60)
    {
        count = 0;
        redis_message_send_heart_beat();
    }
    return count;

  
}

static void task_check_for_wifi_connection(void* pvParameters)
{
  Serial.println("wifi chech connection");



 while(1)   
   {
      vTaskDelay( pdMS_TO_TICKS( 1000 ) );
      check_wifi_connection();
   }
    
  
}




static void task_internal_sensor(void* pvParameters)
{
  Serial.println("intenal_sensor_task");

 initialize_internal_sensors();

 while(1)   
   {
      vTaskDelay( pdMS_TO_TICKS( 10 ) );
      process_internal_sensors(10); 
   }
    
  
}

static void task_op_display(void* pvParameters)
{
  Serial.println("op_display");
  initialize_op_button();

  while(1)   
 {
      vTaskDelay( pdMS_TO_TICKS( 50 ) );
       
     process_op_button(50);
       
   }
}

static void task_server_display(void* pvParameters)
{
  int count;
  count = 0;
  initialize_server_commands();
  initialize_op_button();

  while(1)   
 {
     
      vTaskDelay( pdMS_TO_TICKS( 1000 ) );
      count = send_reboot(count); 
      process_server_commands(1000);
      process_op_button(1000);   
   }
}


static void taskDisplay(void* pvParameters)
{
   
   initialize_screen_button();
   while(1)   
   {
      vTaskDelay( pdMS_TO_TICKS( 50 ) );
  
     process_screen_button(50);
    
       
   }
    
   
}



static void taskWdt(void* pvParameters)
{
   int count;
   redis_message_send_heart_beat();
   wdtInit();
   count = 0;
   while(1)   
   {
       
       wdtClear();
       vTaskDelay(1000000 / portTICK_PERIOD_US); //delay 1 second

       wdtClear();
   }
    
}    

    /*
 TaskHandle_t xTaskCreateStatic( TaskFunction_t pvTaskCode,
 const char * const pcName,
 uint32_t ulStackDepth,
 void *pvParameters,
 UBaseType_t uxPriority,
 StackType_t * const puxStackBuffer,
 StaticTask_t * const pxTaskBuffer );   
 */  



void create_tasks(void)
{
    Serial.println("create tasks");
   xTaskCreateStatic(task_internal_sensor,     "Internal Sensors",       BASIC_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5, internal_sensor_stack, &Handle_Internal_Sensor_Task);
   
    //xTaskCreateStatic(task_op_display,     "PUSH_TO_SERVER",       BASIC_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, op_interface_stack, &Handle_Op_Interface_Task);
    xTaskCreateStatic(taskDisplay,     "Internal Display",       BASIC_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, display_stack, &Handle_Display_Task);  // not redis
    //xTaskCreateStatic(task_check_for_wifi_connection,     "WIFI_CHECK",       BASIC_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, wifi_stack, &Handle_wifi_Task);
     xTaskCreateStatic(task_server_display,     "SERVER_INTERFACE",       BASIC_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2,  server_interface_stack, &Handle_server_interface_Task);  // redis  
    xTaskCreateStatic(taskWdt,     "Task WD",       BASIC_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, wd_stack, &Handle_Wd_Task);
    
    //vTaskStartScheduler(); because wifi functions have started scheduler
}

//process_op_button(MY_TIME_TICK);
      //process_ev_sensors(MY_TIME_TICK);
      //process_server_commands(MY_TIME_TICK);
      //process_screen_button(MY_TIME_TICK);

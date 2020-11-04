#include "common_setup.h"
#include "server_commands.h"
#include "redis_functions.h"
#include "msgpack_rx_handler.h"

#define COMMAND_SIZE 50
static uint32_t command_size = COMMAND_SIZE;
static char command[COMMAND_SIZE];
static void process_message_pack_data(String msgpack_data);

static String command_string = "command";

static void process_send_alert_command( char *data_ptr, int length);
static bool find_command( String command, char *buffer, int buffer_length);
static bool find_string(String field, char *buffer, int buffer_length, char *messsage_field, int length);
static bool find_integer(String field, char *buffer, int buffer_length, uint32_t *duration);
  
void initialize_server_commands(void)
{
   Serial.println("Buzzer init");
   pinMode(WIO_BUZZER, OUTPUT);
    analogWrite(WIO_BUZZER, 0);
    
}

void process_server_commands(int time_tick)
{
    int length;
    int valid_flag;
    String msgpack_data;   
    if(1) //if( redis_job_queue_length(SERVER_QUEUE)>0)
    {
      
      msgpack_data = redis_job_queue_pop(SERVER_QUEUE, &valid_flag);
      if( valid_flag == 0 )
      {
        ; //Serial.println("invalid data");
      }
      else
      {
        
        process_message_pack_data(msgpack_data);
      }
    }
    else
    {
      ;//Serial.println("no data");
      
    }
       
}

void process_message_pack_data(String msgpack_data)
{
   int length;
   cmp_ctx_t ctx;
   char *data_ptr;
   uint32_t map_size;
   length = msgpack_data.length();
   data_ptr =(char *)msgpack_data.c_str();

   if(length < 25)
   {
    return;
   }
   Serial.println("commamd Received");
   if( find_command("SEND_ALERT" ,data_ptr,length))
   {
      Serial.println("Send alert detected");
      process_send_alert_command(data_ptr,length);
   }
   
}


static void process_send_alert_command( char *data_ptr, int length)
{
  char message_field[100];
  uint32_t duration;
  if( find_string("message",data_ptr,length, &message_field[0],100))
  {
    
    if(find_integer("duration",data_ptr,length, &duration ))
    {
        lcd_setFreeFont(18);
        lcd_clear_screen();
        lcd_drawString(message_field,5,50);
        
        analogWrite(WIO_BUZZER, 128);
        vTaskDelay( pdMS_TO_TICKS( duration ) );
        analogWrite(WIO_BUZZER, 0);
    }
  }

}  

static bool find_command( String command,char *read_buffer, int read_size)
{

   char command_name[50];
   memset(command_name,0,50);
   if( find_string("command", read_buffer, read_size,command_name,50))
   {
     Serial.println(command_name);
     if( strcmp (command.c_str(), command_name)==0)
     {
      return true;
     }
   }
   return false;
}

static bool find_string(String field, char *read_buffer, int read_size, char *message_field, int length)
{
   cmp_ctx_t ctx;
   uint32_t size;
   size = length;
   msgpack_rx_handler_init(&ctx, read_buffer,  read_size);
   if( msgpack_rx_handler_find_string(&ctx,(char *)field.c_str(),message_field,  &size ))
   {
    return true;
   }
   return false;
}

static bool find_integer(String field, char *read_buffer, int read_size, uint32_t *duration)
{  cmp_ctx_t ctx;
   msgpack_rx_handler_init(&ctx, read_buffer,  read_size);
   return msgpack_rx_handler_find_unsigned(&ctx,(char *)field.c_str(), duration);

}




//payload["command"] = "SEND_ALERT"
//payload["message"] = "Water On"

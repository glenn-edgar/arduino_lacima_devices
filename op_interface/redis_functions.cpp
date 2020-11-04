#include "common_setup.h"
#include "redis_functions.h"

#include "Redis.h"
//#include “FreeRTOS.h”
//#include “semphr.h”
static SemaphoreHandle_t  redis_mutex;
#define JOB_QUEUE_PUSH_LENGTH 100
#define STR_LEN 100
static StaticJsonDocument<500> doc; //setup is store in static document
static StaticJsonDocument<1000> doc1; //setup is store in static document
static const char *ip;
static int port;
static int db;
static const char *password;

static const char *node_id;
static const char *reboot_key;
static const char *heart_beat_key;
static const char *output_key;
static const char *output_topic;
static char *input_key;
static char *internal_key;
static char *external_key;

static char Input_Queue[STR_LEN];
static char Internal_Stream[STR_LEN];
static char External_Stream[STR_LEN];



#include <AtWiFi.h>



static WiFiClient redisConn;
Redis redis(redisConn);

void display_redis_config(void)
{
  lcd_clear_screen();
  lcd_setFreeFont(9);
  lcd_drawString("Redis Config",5,20);
  lcd_drawString("IP: ",5,40);
  lcd_drawString(ip,100,40);
  lcd_drawString("Port: ",5,60);
  lcd_drawString(String(port),100,60);
  lcd_drawString("DB: ",5,80);
  lcd_drawString(String(db),100,80);
  
}

void display_redis_keys(void)
{
  lcd_clear_screen();
  lcd_setFreeFont(9);
  lcd_drawString("Redis Keys",5,20);
  lcd_drawString("Out Key: ",5,40);
  lcd_drawString(output_key,100,40);
  lcd_drawString("Out Topic: ",5,60);
  lcd_drawString(output_topic,100,60);
  lcd_drawString("Reboot Key: ",5,80);
  lcd_drawString(reboot_key,100,80);
  lcd_drawString("HB Key ",5,100);
  lcd_drawString(heart_beat_key,100,100);
  lcd_drawString("Input ",5,120);
  lcd_drawString(input_key,100,120);
  lcd_drawString("Internal ",5,140);
  lcd_drawString(internal_key,100,140);
  lcd_drawString("External ",5,160);
  lcd_drawString(external_key,100,160);
}

static void read_server_file(void)
{
   File file = SD.open("redis.jsn");
   if( file == false )
   {
       lcd_flash("No redis.jsn",2000,500);
   }
   DeserializationError err = deserializeJson(doc, file);
   if (err) 
   {
      lcd_flash("JSON Error",2000,500);
   }
   ip = doc["ip"].as<char*>();
   port = doc["port"].as<int>();
   db = doc["db"].as<int>();
   password   =doc["password"].as<char*>();
   
   File file1 = SD.open("ap_reds.jsn");
   if( file1 == false )
   {
       lcd_flash("No ap_reds.jsn",2000,500);
   }
   DeserializationError err1 = deserializeJson(doc1, file1);
   if (err1) 
   {
      lcd_flash("JSON Error",2000,500);
   }
   ip = doc["ip"].as<char*>();
   port = doc["port"].as<int>();
   db = doc["db"].as<int>();
   password   =doc["password"].as<char*>();   

   output_topic = doc1["out_topic"].as<char*>();
   output_key = doc1["output"].as<char*>();
   reboot_key = doc1["reboot"].as<char*>();
   node_id    = doc1["node_id"].as<char*>();
   heart_beat_key = doc1["heart_beat"].as<char*>();
   strcpy(Input_Queue,node_id);
   strcat(Input_Queue,"/INPUT");
   input_key = &Input_Queue[0];
   strcpy(Internal_Stream,"INTERNAL_STREAM");
  
   internal_key = &Internal_Stream[0];
   strcpy(External_Stream,"EXTERNAL_STREAM");

   external_key = &External_Stream[0];


  
}

//"node_id": "OP_1", "reboot": "REBOOT", "heart_beat": "HEART_BEAT", "output": "DEV_OUT", "out_topic": "MASTER_VALVE"}


static void redis_connect(void )
{


    redis_mutex = xSemaphoreCreateMutex();
  
    if (!redisConn.connect(ip, port))
    {
        lcd_flash("Redis Connect Err",2000,500);
        return;
    }

    
    auto connRet = redis.authenticate(password);
    if (connRet == RedisSuccess)
    {
        Serial.println("Connected to the Redis server!");
    } 
    else 
    {
        lcd_flash("Redis Auth Err",2000,500);
        return;
    }
    lcd_clear_screen();
    lcd_setFreeFont(12);
    lcd_drawString("Redis Connected",5,20);
    delay(1000);
    // set to db
    // send to reboot queue
}

void test_function(void)
{
  Serial.println("test hash");
  Serial.println(redis.hlen("test_hash"));
  
}


void redis_send_internal_stream(int length, char **keys, double *values)
{
   char     float_string[20];
   char      write_buffer[200];
   char      *write_ptr;
   char      *buffer_ptr;
   write_ptr = &write_buffer[0];
   buffer_ptr = &write_buffer[0];
   cmp_ctx_t ctx;
   memset(write_buffer,0,sizeof(write_buffer));
   msgpack_dict_stream_init(&ctx, write_ptr, sizeof(write_buffer));
   cmp_write_map(&ctx,length+1);
   write_ptr = "Node_Id";
   cmp_write_str(&ctx,write_ptr ,strlen(write_ptr));
   cmp_write_str(&ctx,node_id ,strlen(node_id));
   for(int i=0;i<length;i++)
   {
      write_ptr = *keys;
      Serial.println(write_ptr);
     
      keys++;
      cmp_write_str(&ctx,write_ptr ,strlen(write_ptr));
      memset(float_string,0,sizeof(float_string));
      snprintf (float_string, sizeof(float_string), "%f", *values);
       Serial.println(float_string);
       cmp_write_str(&ctx,float_string ,strlen(float_string));
      values++;
      
   }
     Serial.println(buffer_ptr);
     
   
   redis_stream_add(internal_key,buffer_ptr);

  
}

void redis_message_send_push_message(void)
{
   char      write_buffer[200];
   char      *write_ptr;
   char      *buffer_ptr;
   write_ptr = &write_buffer[0];
   buffer_ptr = &write_buffer[0];
   cmp_ctx_t ctx;
   memset(write_buffer,0,sizeof(write_buffer));
   msgpack_dict_stream_init(&ctx, write_ptr, sizeof(write_buffer));
   cmp_write_map(&ctx,2);
   write_ptr = "NODE_ID";
   cmp_write_str(&ctx,write_ptr ,strlen(write_ptr));
   write_ptr = (char *)node_id;
   cmp_write_str(&ctx,write_ptr ,strlen(write_ptr));
  
   write_ptr = "TOPIC";
   cmp_write_str(&ctx,write_ptr ,strlen(write_ptr));
   write_ptr = (char *)output_topic;
   cmp_write_str(&ctx,write_ptr ,strlen(write_ptr));
   
   
   redis.job_queue_push(output_key,buffer_ptr,JOB_QUEUE_PUSH_LENGTH);

  
}

void redis_message_send_heart_beat(void)
{
   char      write_buffer[200];
   char      *write_ptr;
   char      *buffer_ptr;
   write_ptr = &write_buffer[0];
   buffer_ptr = &write_buffer[0];
   cmp_ctx_t ctx;
   memset(write_buffer,0,sizeof(write_buffer));
   msgpack_dict_stream_init(&ctx, write_ptr, sizeof(write_buffer));
   cmp_write_map(&ctx,2);
   write_ptr = "NODE_ID";
   cmp_write_str(&ctx,write_ptr ,strlen(write_ptr));
   write_ptr = (char *)node_id;
   cmp_write_str(&ctx,write_ptr ,strlen(write_ptr));
   write_ptr = "HEAP";
   cmp_write_str(&ctx,write_ptr ,strlen(write_ptr));
   cmp_write_int(&ctx,xPortGetFreeHeapSize());
   redis_stream_add(heart_beat_key,buffer_ptr);


  
}

void setup_redis_service(void)
{
   char      write_buffer[100];
   char      *write_ptr;
   char      *buffer_ptr;
   write_ptr = &write_buffer[0];
   buffer_ptr = &write_buffer[0];
   cmp_ctx_t ctx;

   read_server_file();
   redis_connect();
   redis.select(11);
   memset(write_buffer,0,sizeof(write_buffer));
   msgpack_dict_stream_init(&ctx, write_ptr, sizeof(write_buffer));
   cmp_write_map(&ctx,1);
   write_ptr = "NODE_ID";
   cmp_write_str(&ctx,write_ptr ,strlen(write_ptr));
   write_ptr = (char *)node_id;
   cmp_write_str(&ctx,write_ptr ,strlen(write_ptr));
   
   redis_stream_add(reboot_key,buffer_ptr);
   
   
}

void redis_stream_add(const char *key, char *data )
{
      if( xSemaphoreTake( redis_mutex,  pdMS_TO_TICKS(1000))  == pdTRUE)
      { 
         redis.stream_add(key,data);
         xSemaphoreGive(redis_mutex );
      }
     else
     {
      system_reset();
     }
}
 


void redis_job_queue_push(const char *key, char *data,int queue_length)
{
      if( xSemaphoreTake( redis_mutex,  pdMS_TO_TICKS(2000))  == pdTRUE)
      { 
         redis.job_queue_push(key,data,queue_length);
         xSemaphoreGive(redis_mutex );
      }
     else
     {
      system_reset();
     }
}


int redis_job_queue_length(int queue_type)
{
      String temp_value;
      int    return_value;
      if( xSemaphoreTake( redis_mutex,  pdMS_TO_TICKS(2000) ) == pdTRUE)
      { 
         if( queue_type == SERVER_QUEUE)
         {
            temp_value = redis.llen(input_key);
            if( temp_value.indexOf("-") == -1)
            {
              return_value = temp_value.toInt();
            }
            else
            {
              return_value = -1;
            }
         }
         xSemaphoreGive(redis_mutex );
      }
     else
     {
      system_reset();
     }
     
     return return_value;
  
}


String redis_job_queue_pop(int queue_type, int *valid_flag)
{
      String return_value;
      if( xSemaphoreTake( redis_mutex,  pdMS_TO_TICKS(2000) ) == pdTRUE)
      { 
         if( queue_type == SERVER_QUEUE)
         {
            return_value = redis.job_queue_pop(input_key);
       
           
            if( return_value == "__INTERNAL_ERROR__")
            {
              *valid_flag = 0;
            
            }
            else
            {
              *valid_flag = 1;
             
            }
         
         }
         xSemaphoreGive(redis_mutex );
      }
     else
     {
      system_reset();
     }
     
     return return_value;
  
}



    
    
    

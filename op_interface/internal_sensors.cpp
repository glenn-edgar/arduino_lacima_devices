#include "common_setup.h"
#include "internal_sensors.h"
#include "redis_functions.h"
#include "math.h"
#include"LIS3DHTR.h"

#define ACCUMULATION_LIMIT_MS 15000
#define FIELD_NUMBER 5

static double accumulated_values[FIELD_NUMBER];
static unsigned long accumulated_time;
static unsigned long counts;
static bool initial = false;
static double previous_average[FIELD_NUMBER] = {0,0,0,0,0};
static double new_average[FIELD_NUMBER] = {0,0,0,0,0};

static double sensor_values[FIELD_NUMBER];
static double ref_values[FIELD_NUMBER];
static const char *sensor_names[FIELD_NUMBER] = {"LIGHT_SENSOR","MICROPHONE","ACCELEROMETER_X","ACCELEROMETER_Y","ACCELEROMETER_Z"};
static LIS3DHTR<TwoWire> lis;

static inline void initalize_light_sensor(void);
static inline void initialize_microphone(void);
static inline void initialize_acceleronmeters(void);
static void process_light_sensor(int index);
static void process_microphone(int index);
static void process_acceleronmeter_x(int index);
static void process_acceleronmeter_y(int index);
static void process_acceleronmeter_z(int index);
static double accumulate_light_sensor(int index ,long counts);
static double accumulate_microphone(int index, long counts);
static double accumulate_acceleronmeter_x(int index,long counts);
static double accumulate_acceleronmeter_y(int index,long counts);
static double accumulate_acceleronmeter_z(int index,long counts);
static char* field_values[FIELD_NUMBER] = {"LIGHT","MICROPHONE","ACC_X","ACC_Y","ACC_Z"};

void display_internal_sensors(void)
{
  lcd_clear_screen();
  lcd_drawString("Internal Sensor Values",5,20);
  for(int i=0;i<FIELD_NUMBER;i++)
  {
    lcd_drawString(sensor_names[i],5,i*40+40);
    lcd_drawString(String(accumulated_values[i]),5,i*40+60);
  }
}

void initialize_internal_sensors(void)
{
    analogReadResolution(16);

    counts = 0;
    accumulated_time = 0;
    initalize_light_sensor();
    initialize_microphone();
    initialize_acceleronmeters();
    for(int i = 0; i < FIELD_NUMBER;i++)
    {
   
      sensor_values[i] =0;
      new_average[i] = 0;

    }
}


void process_internal_sensors(int time_tick_ms)
{
    
    process_light_sensor(0);
    process_microphone(1);
    process_acceleronmeter_x(2);
    process_acceleronmeter_y(3);
    process_acceleronmeter_z(4);
    counts = counts +1;
    accumulated_time = accumulated_time + time_tick_ms;
    if( accumulated_time >= ACCUMULATION_LIMIT_MS)
    {
       
       accumulated_values[0] = accumulate_light_sensor(0,counts);
       previous_average[0] = new_average[0]/counts;
      
       accumulated_values[1] = accumulate_microphone(1,counts);
       previous_average[1] = accumulated_values[1];
       previous_average[1] = new_average[1]/counts;
       accumulated_values[2] = accumulate_acceleronmeter_x(2,counts);
       previous_average[2] = new_average[2]/counts;
       accumulated_values[3] = accumulate_acceleronmeter_x(3,counts);
       previous_average[3] = new_average[3]/counts;
       accumulated_values[4] = accumulate_acceleronmeter_x(4,counts);
       previous_average[4] = new_average[4]/counts;
       
       if(initial == false)
       {
          initial = true;
       }
       else
       {
         ;// send message to redis
       }
       #if 0
       Serial.println(accumulated_values[0]);
       Serial.println(accumulated_values[1]);
       Serial.println(accumulated_values[2]);
       Serial.println(accumulated_values[3]);
       Serial.println(accumulated_values[4]);
       #endif
       initialize_internal_sensors();
       redis_send_internal_stream(FIELD_NUMBER, &field_values[0], &accumulated_values[0]);
    }
}


static inline void initalize_light_sensor(void)
{
   pinMode(WIO_LIGHT, INPUT);
}


static void process_light_sensor(int index)
{
  sensor_values[index] += analogRead(WIO_LIGHT);
  new_average[index] += analogRead(WIO_LIGHT);
}

static double accumulate_light_sensor(int index, long counts)
{
   return sensor_values[index]/(double)counts;
}

static inline void initialize_microphone(void)
{
  pinMode(WIO_MIC, INPUT);
  
}
static void process_microphone(int index)
{
  sensor_values[index] += fabs((analogRead(WIO_MIC)- previous_average[index]));
  new_average[index] += analogRead(WIO_MIC);
   
}
static double accumulate_microphone(int index, long counts)
{
  
  return sensor_values[index]/(double)counts;
}


static inline void initialize_acceleronmeters(void)
{
    lis.begin(Wire1); 
    lis.setOutputDataRate(LIS3DHTR_DATARATE_100HZ); // Setting output data rage to 25Hz, can be set up tp 5kHz 
    lis.setFullScaleRange(LIS3DHTR_RANGE_2G); // Setting scale range to 2g, select from 2,4,8,16g
    lis.begin(Wire1);
 
     if (!lis) {
    lcd_flash("Acc Not Init",2500,500);
    } 
}

static void process_acceleronmeter_x(int index)
{
    double temp;
    temp = lis.getAccelerationX();
    sensor_values[index] += fabs(temp- previous_average[index]);
    new_average[index] += temp;
}
 

static double accumulate_acceleronmeter_x(int index, long count)
{
    return sensor_values[index]/count;
  
}

static void process_acceleronmeter_y(int index)
{
    double temp;
    temp = lis.getAccelerationY();
    sensor_values[index] += fabs(temp- previous_average[index]);
    new_average[index] += temp;
}
 

static double accumulate_acceleronmeter_y(int index, long count)
{
    return sensor_values[index]/count;
  
}
static void process_acceleronmeter_z(int index)
{
    double temp;
    temp = lis.getAccelerationZ();
    sensor_values[index] += fabs(temp- previous_average[index]);
    new_average[index] += temp;
}
 

static double accumulate_acceleronmeter_z(int index, long count)
{
    return sensor_values[index]/count;
  
}

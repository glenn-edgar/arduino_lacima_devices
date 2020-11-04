#include "common_setup.h"
#include "screen_button.h"

#include "display_handler.h"
#include "wifi.h"
#include "redis_functions.h"
#include "internal_sensors.h"

#define BASIC_STACK_SIZE 1024

#define MAX_SCREEN_COUNT 5


static int screen_number;
static int state;


#define WAITING_FOR_PRESS 0
#define WAITING_FOR_RELEASE 1

static void display_screen( int count);


void initialize_screen_button(void)
{
     Serial.println("initialize sreen");
     pinMode(WIO_KEY_A, INPUT_PULLUP);
     
     screen_number = 0;
     state = WAITING_FOR_PRESS;
     display_screen(screen_number);
     
}

void process_screen_button(int time_tick)
{
    switch(state)
    {
       case WAITING_FOR_PRESS:
       
       if (digitalRead(WIO_KEY_A) == LOW) 
       {
          state = WAITING_FOR_RELEASE;
       }
       break;

       case WAITING_FOR_RELEASE:
       
        if (digitalRead(WIO_KEY_A) == HIGH) 
       {
          state = WAITING_FOR_PRESS;
          screen_number = screen_number +1;
          if(screen_number >= MAX_SCREEN_COUNT )
          {
            screen_number = 0;
          }
          display_screen(screen_number);
       }
       break;
    }
    
}
void display_release_button_message(void )
{
  lcd_setFreeFont(18);
  lcd_clear_screen();
  lcd_drawString("RELEASE_BUTTON",5,50);
}



static void display_screen( int count)
{
   
   switch(count)
   {
     case 0: display_access_points();
             break;
     case 1: display_wifi_connection();
             break;
     case 2: display_redis_config();
             break;

     case 3: display_redis_keys();
             break;
             
     case 4: display_internal_sensors();
             break;

     case 20: display_release_button_message();
              break;
   }
  
}

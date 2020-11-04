#include "common_setup.h"
#include "op_button.h"
#include "redis_functions.h"

static int state;
#define WAITING_FOR_PRESS 0
#define WAITING_FOR_RELEASE 1

extern void display_release_button_message(void );

void initialize_op_button(void)
{
     Serial.println("initialize sreen");
     pinMode(WIO_KEY_C, INPUT_PULLUP);
     
     
     state = WAITING_FOR_PRESS;
    
     
}

void process_op_button(int time_tick)
{
    switch(state)
    {
       case WAITING_FOR_PRESS:
       
       if (digitalRead(WIO_KEY_C) == LOW) 
       {
          state = WAITING_FOR_RELEASE;
          display_release_button_message();
       }
       break;

       case WAITING_FOR_RELEASE:
       
        if (digitalRead(WIO_KEY_C) == HIGH) 
       {
          state = WAITING_FOR_PRESS;
          redis_message_send_push_message();
          //vTaskDelay(1000000 / portTICK_PERIOD_US); //delay 1 second
       }
       break;
    }
    
}

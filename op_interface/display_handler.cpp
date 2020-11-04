
#include "common_setup.h"
#include "display_handler.h"

#include <TFT_eSPI.h>
#include "TFT_eSPI.h"
#include "Free_Fonts.h" //include the header file
TFT_eSPI tft;

void initialize_display(void)
{
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK); //Black background
  tft.setFreeFont(FS18); //select Free, Serif, 12pt.
  tft.drawString("Rebooting!!!!",5,40);
  delay(1000);
    
    
}


void lcd_flash(const String &message,int on_time,int off_time)
{
  for(int i=0;i<3;i++)
  {
    
    tft.fillScreen(TFT_BLACK); //Black background
    tft.setFreeFont(FS18); //select Free, Serif, 12pt.
    tft.drawString(message,5,40);
    delay(on_time);
    tft.fillScreen(TFT_BLACK); //Black background
    delay(off_time);
  }
  system_reset();
}

void lcd_flash(char *message,int on_time,int off_time)
{
  for(int i=0;i<3;i++)
  {
    
    tft.fillScreen(TFT_BLACK); //Black background
    tft.setFreeFont(FS18); //select Free, Serif, 12pt.
    tft.drawString(message,5,40);
    delay(on_time);
    tft.fillScreen(TFT_BLACK); //Black background
    delay(off_time);
  }
  system_reset();

}
void lcd_clear_screen(void)
{
    
     tft.fillScreen(TFT_BLACK); //Black background
    
}

void lcd_drawString(const String &message, int x_pos, int y_pos)
{
  tft.drawString(message,x_pos,y_pos);  
    
} 
  
void lcd_drawString(char *message,int x_pos,int y_pos)
{
  tft.drawString(message,x_pos,y_pos);  
    
} 

void lcd_setFreeFont(int size)
{
    if(size == 12 )
    {
      tft.setFreeFont(FS12);  
    } 
    else if( size == 9)
    {
      tft.setFreeFont(FS9); 
    }
    else
    {
        tft.setFreeFont(FS18);
    }
    
    
}

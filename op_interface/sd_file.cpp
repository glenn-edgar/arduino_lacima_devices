
#include "common_setup.h"
#include "sd_file.h"

#include <SPI.h>


static bool sd_active = false;

void lcd_flash(String message,int on_time,int off_time);


void initialize_sd(void)
{

  sd_active = SD.begin(SDCARD_SS_PIN, SDCARD_SPI);
  
  if(sd_active == false)
  {
    lcd_flash("No SD Card!!!!",2000,500);
  }
  lcd_clear_screen();
  lcd_setFreeFont(12);  
  lcd_drawString("SD CARD Found",5,40);
  delay(1000);
  
}

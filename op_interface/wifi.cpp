#include "common_setup.h"

#include "wifi.h"
#include <AtWiFi.h>

#define MAX_APP 4

static StaticJsonDocument<500> doc;

static String ssid[MAX_APP];
static String rssi[MAX_APP];
static String wifi_setup_buffer[2];

static int wifi_ap_points;


const char *actual_ap;
const char *actual_password;
static String local_ip;
static char myIPAddress[20];
static String ap_string;


void system_reset(void);
void lcd_flash(String message,int on_time,int off_time);

static void initialize_wifi(void)
{

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  
       
}


char * getIpReadable(IPAddress ipAddress)
{

  unsigned char octet[4] = { 0,0,0,0};
  for( int i=0;i<4;i++)
  {
      octet[i] = (ipAddress >>(i*8))&0xff;
     
  }
  sprintf(myIPAddress,"%d.%d.%d.%d\0",octet[0],octet[1],octet[2],octet[3]);
  return myIPAddress;
}


void display_wifi_connection(void)
{
    
    local_ip = (String)getIpReadable(WiFi.localIP());
    lcd_clear_screen();
    lcd_setFreeFont(12); 
   
    lcd_drawString("Wifi Connected",5,40);
    lcd_drawString("Access Point",5,60);
    lcd_drawString(ap_string,5,80);
    lcd_drawString("Local IP",5,100);
    lcd_drawString(local_ip,5,120);    
    
    
    
}

void display_access_points(void )
{
  int i;
  String temp;
  
  lcd_clear_screen();
  lcd_setFreeFont(12); //select Free, Serif, 12pt.
  lcd_clear_screen();
  if(wifi_ap_points == 0)
  {
      lcd_drawString("No Ap Pts Found",5,40);
  }
  else
  {     
     
    lcd_drawString("AP's Found",5,40+(40)*i);  
     lcd_drawString("RSSI",205,40+(40)*i);  
     for( i = 0; i < wifi_ap_points;i++)
     {
       
       lcd_drawString(ssid[i],5,80+(40)*i);
       lcd_drawString("                      ",180,80+(40)*i); 
       lcd_drawString(rssi[i],205,80+(40)*i);  
         
     }  
    
  }  
}



void load_wifi(void)
{
   File file = SD.open("wifi.jsn");
   if( file == false )
   {
       lcd_flash("No wifi.json",2000,500);
   }
   DeserializationError err = deserializeJson(doc, file);
   if (err) 
   {
      lcd_flash("JSON Error",2000,500);
   }
   actual_ap = doc["access_point"].as<char*>();
   ap_string = String(actual_ap);
   //Serial.println(actual_ap);
   actual_password = doc["password"].as<char*>();
   //Serial.println(actual_password);
   
   lcd_clear_screen();
   lcd_setFreeFont(12); 

   
   lcd_drawString("Connecting To",5,20);
   lcd_drawString("SD Access Point",5,40);
   lcd_drawString(actual_ap,5,60);
   delay(1000);
   
   WiFi.begin(actual_ap, actual_password);
   {
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
        lcd_drawString("                        ",5,120);
        delay(500);
        lcd_drawString("Connecting to WiFi..",5,120);
        delay(500);
        count = count +1;
        if( count > 120)
        {
          system_reset();
        }
        
    }
   }
    
   display_wifi_connection(); 
   
}




  



void scan_wifi(void)
{
   int i;
   lcd_clear_screen();
   lcd_setFreeFont(18); 
   lcd_drawString("Scanning for AP's",5,40);
   wifi_ap_points = WiFi.scanNetworks();
   if(wifi_ap_points > 4)
   {
     wifi_ap_points = 4;
   }
   for( i = 0; i< wifi_ap_points;i++)
   {
      ssid[i] = WiFi.SSID(i);
      rssi[i] = String(WiFi.RSSI(i));
   }
   display_access_points(); 
   delay(1000);
}

void start_wifi(void)
{
  initialize_wifi();
  scan_wifi();
  load_wifi();
  delay(1000);
  
  
}

void check_wifi_connection(void)
{

   if(WiFi.status() != WL_CONNECTED)
   {
    system_reset();

   }


}

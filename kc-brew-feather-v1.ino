œ#include <adafruit_feather.h>
#include "global.hpp"

#include "Log.hpp"
#include "LCD.hpp"
#include "Broadcast.hpp"
#include "HttpDaemon.hpp"
#include "Brewhouse.hpp"

BroadcastClass              Broadcast;
LCDClass                    LCD;
LogClass                    Log;
HttpDaemonClass             HttpDaemon;
BrewhouseClass              Brewhouse;

// ---------------------------------------------------------------------------- loop

void loop() 
{
}

// ---------------------------------------------------------------------------- setup

void setup() 
{
  serial_setup();
  wifi_setup();

  Log.setup();
  LCD.setup();
  
  Brewhouse.setup();
  
  Broadcast.setup();
  HttpDaemon.setup();
  
  digitalWrite(BOARD_LED_PIN, HIGH);
}

void serial_setup()
{  
  Serial.begin(115200);
}

void wifi_setup()
{ 
  while(!Feather.connected())
  {
    Serial.print("Please wait while connecting to: '" WLAN_SSID "' ... ");
    
    if (Feather.connect(WLAN_SSID, WLAN_PASS))
    {
      Serial.println("Connected!");
      Feather.printNetwork();
    }
    else
    {
      Serial.printf("Failed! %s (%d)\n", Feather.errstr(), Feather.errno());
    }
    
    Serial.println();
  }
}


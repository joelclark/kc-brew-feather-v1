#ifndef CONFIG_H__
#define CONFIG_H__


//-------------------------------------------------------------------------------------------//


#define WLAN_SSID                     "CESSPOOL3"
#define WLAN_PASS                     "foobar00"
#define ELEMENT_MAX_CONCURRENCY       2


//-------------------------------------------------------------------------------------------//


#define BOARD_LED_PIN                 PA15
#define UDP_PORT                      2016
#define UDP_INDICATOR_ON_TIME         25

#define ONE_WIRE_BUS                  PA1
#define TEMPERATURE_PROBE_RESOLUTION  10
#define TEMPERATURE_PROBE_MAX_COUNT   8
#define VESSEL_MAX_COUNT              5
#define ELEMENT_MAX_COUNT             2


//-------------------------------------------------------------------------------------------//

class BroadcastClass;
extern BroadcastClass Broadcast;

class HttpDaemonClass;
extern HttpDaemonClass HttpDaemon;

class LCDClass;
extern LCDClass LCD;

class LogClass;
extern LogClass Log;

class BrewhouseClass;
extern BrewhouseClass Brewhouse;

#endif

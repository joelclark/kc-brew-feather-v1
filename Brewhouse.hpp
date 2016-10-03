#ifndef BREWHOUSE_HPP__
#define BREWHOUSE_HPP__

#include <adafruit_feather.h>
#include <adafruit_http_server.h>
#include "global.hpp"
#include <DallasTemperature.h>

#define HLIQ    1 
#define HTUBE   2
#define MASH    3
#define MTUBE   4
#define BOIL    5

class Element;

class Vessel
{
private:
  DeviceAddress m_address;
  char* probeAddressToString(DeviceAddress device_address);
  Element* m_elements[ELEMENT_MAX_COUNT];
  String m_name;
  
public:
  Vessel(const char* name, DeviceAddress device_address)
    : m_name(name)
  {
    memcpy(&m_address, device_address, 8);
  }
  
  void loop();
  void createElement(uint8_t pin);
  void setProbeIndex(uint8_t probe);
  void setSetPoint(uint8_t temperature);
  String& name() { return m_name; }
};

class BrewhouseClass 
{
private:
  char* probeAddressToString(DeviceAddress device_address);
  Vessel* m_HotLiquorTank;
  Vessel* m_HotLiquorTankRecircTube;
  Vessel* m_MashLauterTun;
  Vessel* m_MashLauterTunRecircTube;
  Vessel* m_BoilKettle;
public:
  void setup();
  void setupVessels();
  void setupProbes();
  
  void createHotLiquorTank(DeviceAddress probe_address) { m_HotLiquorTank = new Vessel("HLIQ", probe_address); }
  void createHotLiquorTankRecircTube(DeviceAddress probe_address) { m_HotLiquorTankRecircTube = new Vessel("HTUBE", probe_address); }
  void createMashLauterTun(DeviceAddress probe_address) { m_MashLauterTun = new Vessel("MASH", probe_address); }
  void createMashLauterTunRecircTube(DeviceAddress probe_address) { m_MashLauterTunRecircTube = new Vessel("MTUBE", probe_address); }
  void createBoilKettle(DeviceAddress probe_address) { m_BoilKettle = new Vessel("BOIL", probe_address); }
  
  Vessel* hotLiquorTank() { return m_HotLiquorTank; }
  Vessel* hotLiquorTankRecircTube() { return m_HotLiquorTankRecircTube; }
  Vessel* mashLauterTun() { return m_MashLauterTun; }
  Vessel* mashLauterTunRecircTube() { return m_MashLauterTunRecircTube; }
  Vessel* boilKettle() { return m_BoilKettle; } 
  void loop();
  void toJson(AdafruitHTTPServer &response); // don't judge, the API is a little dopey
};


class Element
{
private:
  uint16_t m_on_time;
  uint16_t m_off_time;
  uint8_t m_percentage;
public:
  void loop();
  void setDutyCycle(uint8_t percentage);
};


#endif

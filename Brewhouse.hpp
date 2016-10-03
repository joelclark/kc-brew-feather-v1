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

class Element
{
private:
  uint16_t m_on_time;
  uint16_t m_off_time;
  uint8_t m_percentage;
  uint8_t m_pin;
  String m_name;
  
public:
  Element(const char* name, uint8_t pin)
    : m_on_time(0)
    , m_off_time(0)
    , m_percentage(0)
    , m_name(name)
    , m_pin(pin)
  {}
  void loop();
  void setDutyCycle(uint8_t percentage);
  
  String& name() { return m_name; }
  uint8_t pin() { return m_pin; }
};

class Vessel
{
private:
  DeviceAddress m_probe;
  Element* m_elements[ELEMENT_MAX_COUNT];
  
  String m_name;
  uint8_t m_element_count;
  uint8_t m_probe_index;
  float m_current_temperature;
  
  char* probeAddressToString(DeviceAddress device_address);
  
public:
  Vessel(const char* name, DeviceAddress device_address)
    : m_name(name)
    , m_element_count(0)
    , m_current_temperature(0)
  {
    memcpy(&m_probe, device_address, 8);
    memset(&m_elements, 0, sizeof(Element*) * ELEMENT_MAX_COUNT);
  }
  
  void setProbeIndex(uint8_t index) { m_probe_index = index; }
  void createElement(uint8_t pin);
  void setSetPoint(uint8_t temperature);
  void updateProbeTemperatureF(float f);
  
  void loop();
  
  String& name() { return m_name; }
  DeviceAddress* probe() { return &m_probe; }
  uint8_t probeIndex() { return m_probe_index; }
  Element* elementAt(uint8_t offset) { return m_elements[offset]; }
  float currentTemperature() { return m_current_temperature; }
};

class BrewhouseClass 
{
private:

  Vessel* m_HotLiquorTank;
  Vessel* m_HotLiquorTankRecircTube;
  Vessel* m_MashLauterTun;
  Vessel* m_MashLauterTunRecircTube;
  Vessel* m_BoilKettle;

  void setupVessels();
  void setupProbes();
  
  void createVessel(const char* name, Vessel **target_member, DeviceAddress probe_address);
  char* probeAddressToString(DeviceAddress *device_address);

public:

  void setup();
  void loop();
  
  Vessel* hotLiquorTank() { return m_HotLiquorTank; }
  Vessel* hotLiquorTankRecircTube() { return m_HotLiquorTankRecircTube; }
  Vessel* mashLauterTun() { return m_MashLauterTun; }
  Vessel* mashLauterTunRecircTube() { return m_MashLauterTunRecircTube; }
  Vessel* boilKettle() { return m_BoilKettle; } 
  
  void toJson(AdafruitHTTPServer &response); // don't judge, the API is a little dopey
  void vesselToJson(Vessel *vessel, String &out, bool first);
};




#endif

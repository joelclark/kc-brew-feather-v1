#include "Brewhouse.hpp"


DeviceAddress               HLT_PROBE_ADDRESS   = { 0x28, 0xD0, 0x80, 0x64, 0x07, 0x00, 0x00, 0xF5 };
DeviceAddress               HTUBE_PROBE_ADDRESS = { 0x28, 0x87, 0x3A, 0x63, 0x07, 0x00, 0x00, 0x2A };
DeviceAddress               RIMS_PROBE_ADDRESS  = { 0x28, 0x1A, 0xA4, 0x64, 0x07, 0x00, 0x00, 0x42 };
DeviceAddress               BK_PROBE_ADDRESS    = { 0x28, 0x83, 0xD4, 0x62, 0x07, 0x00, 0x00, 0x01 };
DeviceAddress               MLT_PROBE_ADDRESS   = { 0x28, 0x5A, 0x1A, 0x62, 0x07, 0x00, 0x00, 0xD3 };

OneWire                     one_wire(ONE_WIRE_BUS);
DallasTemperature           sensors(&one_wire);
DeviceAddress               probe_addresses[TEMPERATURE_PROBE_MAX_COUNT];
uint16_t                    temperatures[TEMPERATURE_PROBE_MAX_COUNT]; //todo: make this a moving average?
uint8_t                     probe_count;
uint16_t                    probe_delay;
unsigned long               last_temperature_requested_at;

Vessel*                     vessels[VESSEL_MAX_COUNT];

Element*                    elements[ELEMENT_MAX_COUNT];
uint16_t                    element_duty_period_length = 1000;
uint8_t                     element_fire_round_robin_indicator;
  
char* BrewhouseClass::probeAddressToString(DeviceAddress device_address)
{
  static char addr_buffer[17];
  char *addr = addr_buffer;
  
  memset(addr, 0, sizeof(addr_buffer));

  for (int i = 0; i < 8; i++)
  {
      sprintf(addr++, "%02X", device_address[i]);
      addr++;
  }    

  return addr_buffer;
}

void BrewhouseClass::setup()
{
  setupVessels();
  setupProbes();
}

void BrewhouseClass::setupVessels()
{
  createHotLiquorTank(HLT_PROBE_ADDRESS);
  createHotLiquorTankRecircTube(HTUBE_PROBE_ADDRESS);
  createMashLauterTun(RIMS_PROBE_ADDRESS);
  createMashLauterTunRecircTube(BK_PROBE_ADDRESS);
  createBoilKettle(MLT_PROBE_ADDRESS);  
}

void BrewhouseClass::setupProbes()
{
  sensors.begin();
  probe_count = min(TEMPERATURE_PROBE_MAX_COUNT, sensors.getDeviceCount());
  probe_delay = 750 / (1 << (12 - TEMPERATURE_PROBE_RESOLUTION));
    
  memset(&temperatures, 0, sizeof(uint16_t) * TEMPERATURE_PROBE_MAX_COUNT);
  for (int i = 0; i < probe_count; ++i)
  {   
      sensors.getAddress(probe_addresses[i], i);
      sensors.setResolution(probe_addresses[i], TEMPERATURE_PROBE_RESOLUTION);

      Serial.print("Probe found: ");
      Serial.println(probeAddressToString(probe_addresses[i]));
  }

  sensors.setWaitForConversion(false);
  last_temperature_requested_at = 0;    
}

void BrewhouseClass::loop()
{
  uint16_t t;
  
  if (millis() > (last_temperature_requested_at + probe_delay))
  {        
    for (int i = 0; i < probe_count; ++i)
    {
        t = (uint16_t)(sensors.getTempFByIndex(i) + 0.5);

        if (t != temperatures[i]) {
            temperatures[i] = t;
        }
    }

    sensors.requestTemperatures(); 
    last_temperature_requested_at = millis(); 
  }
}

void vesselToJson(Vessel *vessel, String &out)
{
  out.concat("\n\t\"");
  out.concat(vessel->name());
  out.concat("\": {}");
}

void BrewhouseClass::toJson(AdafruitHTTPServer &response)
{
  String out;
  
  out.concat("{\n");
  
  // section: misc
  out.concat("\"misc\": {");
  out.concat("\n\t\"probe_count\": ");
  out.concat(probe_count);
  out.concat("\n\t,\"probe_delay\": ");
  out.concat(probe_delay);
  
  out.concat("\n},");

  // section: probes
  out.concat("\"probes\": [\n");
  for (int i = 0; i < probe_count; ++i)
  {
    out.concat("\t");
    if (i > 0) out.concat(",");
    out.concat("\"");
    out.concat(probeAddressToString(probe_addresses[i]));
    out.concat("\"");
    out.concat("\n");
  }
  out.concat("\n]");

  // section: vessels
  out.concat("\"vessels\": [");

  vesselToJson(hotLiquorTank(), out);
  
  out.concat("\n]"); 


  // end of structure
  out.concat("\n}");
}

void Element::setDutyCycle(uint8_t percentage)
{
  m_on_time = max(min(percentage, 100), 0) / 100.0 * element_duty_period_length;
  m_off_time = element_duty_period_length - m_on_time;
  m_percentage = (m_off_time < 1) ? 100 : round(m_on_time / m_off_time);

  // reduce until the minimum portion of the cycle is about 150ms
  while (m_on_time >= 165 && m_off_time >= 165) {
    m_off_time = round(m_off_time * 0.9);
    m_on_time = round(m_on_time * 0.9);
  }
}



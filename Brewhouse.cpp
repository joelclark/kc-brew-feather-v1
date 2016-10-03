#include "Brewhouse.hpp"

#define                     DEVICE_ADDRESS_SIZE sizeof(DeviceAddress)

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
Vessel*                     vessels_by_probe_index[VESSEL_MAX_COUNT];
uint8_t                     vessel_count = 0;

Element*                    elements[ELEMENT_MAX_COUNT * VESSEL_MAX_COUNT];
uint8_t                     element_count = 0;
uint16_t                    element_duty_period_length = 1000;
uint8_t                     element_fire_round_robin_indicator;
  

// -----------------------------------------------------------------------
// -------------------------------------------------- BREWHOUSE SETUP ----
// -----------------------------------------------------------------------

void BrewhouseClass::setup()
{
  setupProbes();
  setupVessels();
}

void BrewhouseClass::setupVessels()
{
  createVessel("HLIQ", &m_HotLiquorTank, HLT_PROBE_ADDRESS);
  createVessel("HTUBE", &m_HotLiquorTankRecircTube, HTUBE_PROBE_ADDRESS);
  createVessel("MASH", &m_MashLauterTun, MLT_PROBE_ADDRESS);
  createVessel("MTUBE", &m_MashLauterTunRecircTube, RIMS_PROBE_ADDRESS);
  createVessel("BOIL", &m_BoilKettle, BK_PROBE_ADDRESS);  

  hotLiquorTankRecircTube()->createElement(PB4);
  
  mashLauterTunRecircTube()->createElement(PB5);  
  
  boilKettle()->createElement(PA2);  
  boilKettle()->createElement(PA3);
}

void BrewhouseClass::createVessel(const char* name, Vessel **target_member, DeviceAddress probe_address)
{
  Vessel *vessel = *target_member = new Vessel(name, probe_address);

  for (uint8_t i = 0; i < probe_count; i++)
  {
    if (memcmp(probe_addresses[i], probe_address, DEVICE_ADDRESS_SIZE) == 0)
    {
      vessel->setProbeIndex(i);
      vessels_by_probe_index[i] = vessel;
    }
  }
  
  vessels[vessel_count] = vessel;
  vessel_count++;
}

void BrewhouseClass::setupProbes()
{
  sensors.begin();
  probe_count = min(TEMPERATURE_PROBE_MAX_COUNT, sensors.getDeviceCount());
  probe_delay = (750 / (1 << (12 - TEMPERATURE_PROBE_RESOLUTION)));
    
  memset(&temperatures, 0, sizeof(uint16_t) * TEMPERATURE_PROBE_MAX_COUNT);
  for (int i = 0; i < probe_count; ++i)
  {   
      sensors.getAddress(probe_addresses[i], i);
      sensors.setResolution(probe_addresses[i], TEMPERATURE_PROBE_RESOLUTION);

      Serial.print("Probe found: ");
      Serial.println(probeAddressToString(&probe_addresses[i]));
  }

  sensors.setWaitForConversion(false);
  last_temperature_requested_at = millis(); 
}


// -----------------------------------------------------------------------
// -------------------------------------------------- BREWHOUSE OPS ------
// -----------------------------------------------------------------------


void BrewhouseClass::loop()
{
  uint16_t t;
  
  if (millis() > (last_temperature_requested_at + probe_delay))
  {        
    for (int i = 0; i < probe_count; ++i)
    {
        t = (uint16_t)round(sensors.getTempF(probe_addresses[i])*10);

        if (t != temperatures[i]) {
          temperatures[i] = t;
          vessels_by_probe_index[i]->updateProbeTemperatureF(t/10.0);
        }
    }

    sensors.requestTemperatures(); 
    last_temperature_requested_at = millis(); 
  }
}


// -----------------------------------------------------------------------
// -------------------------------------------------- JSON SERIALIZATION -
// -----------------------------------------------------------------------


void BrewhouseClass::vesselToJson(Vessel *vessel, String &out, bool first)
{
  out.concat("\n\t");
  if (!first) out.concat(','); else out.concat(' ');
  out.concat("\"");
  out.concat(vessel->name());
  out.concat("\": {");

  out.concat("\n\t\t \"probe\": \"");
  out.concat(probeAddressToString(vessel->probe()));
  out.concat("\"");

  out.concat("\n\t\t \"probe_index\": \"");
  out.concat(vessel->probeIndex());
  out.concat("\"");

  out.concat("\n\t\t \"current_temperature\": \"");
  out.concat(vessel->currentTemperature());
  out.concat("\"");
  
  out.concat("\n\t\t,\"elements\": {");
  for (uint8_t offset = 0; offset < ELEMENT_MAX_COUNT; offset++)
  {
    Element *element = vessel->elementAt(offset);
    if (element == NULL) break;
    out.concat("\n\t\t\t");
    if (offset > 0) out.concat(','); else out.concat(' ');
    out.concat("\"");
    out.concat(element->name());
    out.concat("\": {");
    out.concat("\n\t\t\t\t\"pin\": ");
    out.concat(element->pin());
    out.concat("\n\t\t\t }");
  }
  
  out.concat("\n\t\t }");
  
  out.concat("\n\t}");
}

void BrewhouseClass::toJson(AdafruitHTTPServer &response)
{
  String out;
  
  out.concat("{\n");

  out.concat("\n \"name\": \"");
  out.concat(BREWHOUSE_NAME);
  out.concat("\"\n,\"url\": \"");
  out.concat(BREWHOUSE_URL);
  out.concat("\"\n");
  
  
  // section: misc
  out.concat("\n,\"misc\": {");
  out.concat("\n\t \"probe_count\": ");
  out.concat(probe_count);
  out.concat("\n\t,\"probe_delay\": ");
  out.concat(probe_delay);
  
  out.concat("\n}\n");

  // section: probes
  out.concat("\n,\"probes\": [\n");
  for (int i = 0; i < probe_count; ++i)
  {
    out.concat("\t");
    if (i > 0) out.concat(','); else out.concat(' ');
    out.concat("\"");
    out.concat(probeAddressToString(&probe_addresses[i]));
    out.concat("\"");
    out.concat("\n");
  }
  out.concat("\n]\n");

  // section: vessels
  out.concat("\n,\"vessels\": {");

  for (uint8_t i = 0; i < vessel_count; i++)
  {
    vesselToJson(vessels[i], out, i == 0);
  }
  
  out.concat("\n}"); 

  // end of structure
  out.concat("\n\n}");

  response.print(out);
}

char* BrewhouseClass::probeAddressToString(DeviceAddress *device_address)
{
  static char addr_buffer[17];
  char *addr = addr_buffer;
  
  memset(addr, 0, sizeof(addr_buffer));

  for (int i = 0; i < DEVICE_ADDRESS_SIZE; i++)
  {
      sprintf(addr++, "%02X", (*device_address)[i]);
      addr++;
  }    

  return addr_buffer;
}

// -----------------------------------------------------------------------
// -------------------------------------------------- VESSEL -------------
// -----------------------------------------------------------------------

void Vessel::createElement(uint8_t pin)
{
  m_element_count++;
  if (m_element_count > ELEMENT_MAX_COUNT) return;
  
  String name(this->name());
  name.concat(m_element_count);
  
  m_elements[m_element_count-1] = new Element(name.c_str(), pin);

  // maintain global list for ease of iteration
  elements[element_count] = m_elements[m_element_count-1];
  element_count++;
}

void Vessel::updateProbeTemperatureF(float f)
{
  m_current_temperature = f;
}

// -----------------------------------------------------------------------
// -------------------------------------------------- ELEMENT ------------
// -----------------------------------------------------------------------


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

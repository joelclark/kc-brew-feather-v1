#ifndef BROADCAST_H__
#define BROADCAST_H__

#include <adafruit_feather.h>
#include "global.hpp"

class BroadcastClass {
public:
  void setup();
  void loop();
  void addClient(uint8_t ip);
  void sendPing();
  void sendTemperatureChanged(uint8_t probe, uint16_t value);
  void sendElementStateChanged(uint8_t element, bool on);
  void sendError(const String& error);
  void sendComment(const String& comment);
};

#endif



#ifndef LOG_H__
#define LOG_H__

#include <adafruit_feather.h>
#include "global.hpp"

class LogClass
{
  public:
    void setup() { info("MCU up, logging enabled."); }
    void info(const String& message) {}
    void error(const String& message) {}
};

#endif

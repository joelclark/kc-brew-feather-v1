#ifndef PERIODIC_OPERATION_HPP__
#define PERIODIC_OPERATION_HPP__

#include <adafruit_feather.h>
#include "global.hpp"

class PeriodicOperation 
{
private:
  uint32_t m_interval;
  unsigned long m_last_fired_at;
  
public:
  PeriodicOperation(uint32_t interval)
    : m_interval(interval)
    , m_last_fired_at(0)
  { }

  void loop() { 
    if (millis() > (m_last_fired_at + m_interval)) {
      doWork();
    }
  }
  
  virtual void doWork() = 0;
};

#endif


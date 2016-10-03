#include "Broadcast.hpp"

#define MSG_TOKEN (_buffer[0])
#define ERR_PREFIX "!ERR:"

class UdpIndicator
{
private:
public:
  UdpIndicator() { }
  void loop() { }
  void pulse() { }
};


AdafruitUDP     _udp;
bool            _debug = true;
bool            _echo = true;

void            udp_broadcast_message(const String& message);

UdpIndicator    send_indicator;

void BroadcastClass::setup()
{
}

void BroadcastClass::loop()
{
  send_indicator.loop();
}

void BroadcastClass::sendPing()
{
}

void BroadcastClass::sendError(const String& error)
{
  String out(ERR_PREFIX);
  out.concat(error);
  udp_broadcast_message(out);
}

void BroadcastClass::sendComment(const String& comment)
{
  String out("#REM:");
  out.concat(comment);
}

void udp_send_message(const IPAddress& ip, uint16_t port, const String& message)
{
  send_indicator.pulse();
  
  _udp.beginPacket(ip, port);
  _udp.write(message.c_str());
  _udp.endPacket();
}

void udp_broadcast_message(const String& message)
{
  send_indicator.pulse();
  
  // ??
}



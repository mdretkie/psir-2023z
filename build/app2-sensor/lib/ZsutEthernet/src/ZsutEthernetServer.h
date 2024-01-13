#ifndef Zsut_ethernetserver_h
#define Zsut_ethernetserver_h

#include "Server.h"

class ZsutEthernetClient;

class ZsutEthernetServer : 
public Server {
private:
  uint16_t _port;
  void accept();
public:
  ZsutEthernetServer(uint16_t);
  ZsutEthernetClient available();
  virtual void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  using Print::write;
};

#endif

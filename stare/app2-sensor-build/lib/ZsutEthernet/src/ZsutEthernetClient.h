#ifndef Zsut_ethernetclient_h
#define Zsut_ethernetclient_h
#include "Arduino.h"	
#include "Print.h"
#include "ZsutClient.h"
#include "ZsutIPAddress.h"

class ZsutEthernetClient : public ZsutClient {

public:
  ZsutEthernetClient();
  ZsutEthernetClient(uint8_t sock);

  uint8_t status();
  virtual int connect(ZsutIPAddress ip, uint16_t port);
  virtual int connect(const char *host, uint16_t port);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, size_t size);
  virtual int peek();
  virtual void flush();
  virtual void stop();
  virtual uint8_t connected();
  virtual operator bool();
  virtual bool operator==(const bool value) { return bool() == value; }
  virtual bool operator!=(const bool value) { return bool() != value; }
  virtual bool operator==(const ZsutEthernetClient&);
  virtual bool operator!=(const ZsutEthernetClient& rhs) { return !this->operator==(rhs); };
  uint8_t getSocketNumber();

  friend class ZsutEthernetServer;
  
  using Print::write;

private:
  static uint16_t _srcport;
  uint8_t _sock;
};

#endif

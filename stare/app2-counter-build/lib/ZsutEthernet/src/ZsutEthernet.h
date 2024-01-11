#ifndef Zsut_ethernet_h
#define Zsut_ethernet_h

#include <inttypes.h>
//#include "w5100.h"
#include "ZsutIPAddress.h"
#include "ZsutEthernetClient.h"
#include "ZsutEthernetServer.h"
#include "ZsutDhcp.h"

#define MAX_SOCK_NUM 4

class ZsutEthernetClass {
private:
  ZsutIPAddress _dnsServerAddress;
  ZsutDhcpClass* _dhcp;
public:
  static uint8_t _state[MAX_SOCK_NUM];
  static uint16_t _server_port[MAX_SOCK_NUM];
  // Initialise the Ethernet shield to use the provided MAC address and gain the rest of the
  // configuration through DHCP.
  // Returns 0 if the DHCP configuration failed, and 1 if it succeeded
  int begin(uint8_t *mac_address, unsigned long timeout = 60000, unsigned long responseTimeout = 4000);
  void begin(uint8_t *mac_address, ZsutIPAddress local_ip);
  void begin(uint8_t *mac_address, ZsutIPAddress local_ip, ZsutIPAddress dns_server);
  void begin(uint8_t *mac_address, ZsutIPAddress local_ip, ZsutIPAddress dns_server, ZsutIPAddress gateway);
  void begin(uint8_t *mac_address, ZsutIPAddress local_ip, ZsutIPAddress dns_server, ZsutIPAddress gateway, ZsutIPAddress subnet);
  int maintain();

  ZsutIPAddress localIP();
  ZsutIPAddress subnetMask();
  ZsutIPAddress gatewayIP();
  ZsutIPAddress dnsServerIP();

  friend class ZsutEthernetClient;
  friend class ZsutEthernetServer;
};

extern ZsutEthernetClass ZsutEthernet;

#endif

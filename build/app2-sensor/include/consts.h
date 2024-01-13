#ifndef INCLUDED_CONSTS_H
#define INCLUDED_CONSTS_H

#include "arduino.h"

#ifdef PSIR_ARDUINO
    #define SERVER_ADDRESS ZsutIPAddress(127, 0, 0, 1)
#endif

#define SERVER_PORT 12344

#endif

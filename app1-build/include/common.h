#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "arduino.h"

char* alloc_string(char const* string);
char const* formatted_timestamp();
int random_in_range(int min, int max);
void sleep_ms(unsigned ms);
char* serialise_u32(char* buffer, uint32_t value);
char const* deserialise_u32(char const* buffer, void* value);

#ifndef PSIR_ARDUINO

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
char const* address_to_text(struct sockaddr_in address);

#endif


#endif

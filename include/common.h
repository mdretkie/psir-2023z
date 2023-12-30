#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

char* alloc_string(char const* string);
char const* formatted_timestamp();
char const* address_to_text(struct sockaddr_in address);
int random_in_range(int min, int max);
void sleep_ms(unsigned ms);

#endif

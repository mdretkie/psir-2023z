#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "protocol.h"


typedef struct InboundMessage {
    Message message;
    struct sockaddr sender_address;
} InboundMessage;

typedef struct OutboundMessage {
    Message message;
    struct sockaddr_in receiver_address;
} OutboundMessage;

void send_and_free_message(OutboundMessage message, int so);
InboundMessage receive_message_blocking(int so);


#endif

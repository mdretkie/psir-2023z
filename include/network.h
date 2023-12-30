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
    struct sockaddr receiver_address;
} OutboundMessage;

typedef enum AckStatus {
    ack_received, ack_lost,
} AckStatus;

AckStatus send_and_free_message(OutboundMessage message, int so);
InboundMessage receive_message_blocking(int so);


#endif

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


typedef struct InboundBuffer {
    struct sockaddr sender_address;
    char* buffer;
    size_t buffer_length;
} InboundBuffer;

InboundBuffer inbound_buffer_new(struct sockaddr sender_address);
void inbound_buffer_free(InboundBuffer* inbound_buffer);

bool inbound_buffer_has_complete_message(InboundBuffer* inbound_buffer);
/* Nadmiarowe dane (z następnej przychodzącej wiadomości) są pozostawione w buforze. */
InboundMessage inbound_buffer_take_complete_message(InboundBuffer* inbound_buffer);
void inbound_buffer_push_data(InboundBuffer* inbound_buffer, char const* data, size_t data_length);


typedef struct Network {
    InboundBuffer* inbound_buffers;
    size_t inbound_buffer_count;
} Network;

Network network_new();
void network_free(Network* network);

void network_append_inbound_buffer(Network* network, struct sockaddr sender_address);
void network_push_inbound_data(Network* network, char const* data, size_t data_length, struct sockaddr sender_address);
bool network_take_any_complete_message(Network* network, InboundMessage* inbound_message_result);
InboundMessage network_receive_message_blocking(Network* network, int so);
void network_send_and_free_message(Network* network, int so, OutboundMessage message);





AckStatus send_and_free_message(OutboundMessage message, int so);
InboundMessage receive_message_blocking(int so);


#endif

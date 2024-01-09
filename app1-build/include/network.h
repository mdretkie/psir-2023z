#ifndef NETWORK_H
#define NETWORK_H

#include "protocol.h"
#include "arduino.h"

#ifdef PSIR_ARDUINO
#define SOCKADDR char
#define SOCKADDR_IN char
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define SOCKADDR struct sockaddr
#define SOCKADDR_IN struct sockaddr_in
#endif


typedef struct InboundMessage {
    Message message;
    SOCKADDR sender_address;
} InboundMessage;

typedef struct OutboundMessage {
    Message message;
    SOCKADDR receiver_address;
} OutboundMessage;


typedef struct InboundBuffer {
    SOCKADDR sender_address;
    char* buffer;
    size_t buffer_length;
} InboundBuffer;

InboundBuffer inbound_buffer_new(SOCKADDR sender_address);
void inbound_buffer_free(InboundBuffer inbound_buffer);

bool inbound_buffer_has_complete_message(InboundBuffer const* inbound_buffer);
/* Nadmiarowe dane (z następnej przychodzącej wiadomości) są pozostawione w buforze. */
InboundMessage inbound_buffer_take_complete_message(InboundBuffer* inbound_buffer);
void inbound_buffer_push_data(InboundBuffer* inbound_buffer, char const* data, size_t data_length);


typedef struct Network {
    int so;
    InboundBuffer* inbound_buffers;
    size_t inbound_buffer_count;
} Network;

Network network_new(int so);
void network_free(Network network);

void network_append_inbound_buffer(Network* network, SOCKADDR sender_address);
void network_push_inbound_data(Network* network, char const* data, size_t data_length, SOCKADDR sender_address);
bool network_take_any_complete_message(Network* network, InboundMessage* inbound_message_result);
InboundMessage network_receive_message_blocking(Network* network);
void network_send_and_free_message(Network* network, OutboundMessage message);
void network_send_and_free_message_no_ack(Network* network, OutboundMessage message);


#endif

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"

InboundBuffer inbound_buffer_new(struct sockaddr sender_address) {
    InboundBuffer inbound_buffer = {
        .sender_address = sender_address,
        .buffer = NULL,
        .buffer_length = 0,
    };
    return inbound_buffer;
}


void inbound_buffer_free(InboundBuffer inbound_buffer) {
    free(inbound_buffer.buffer);
}


bool inbound_buffer_has_complete_message(InboundBuffer* inbound_buffer) {
    if (inbound_buffer->buffer_length < sizeof(uint32_t)) {
        return false;
    }

    uint32_t message_length = *(uint32_t*)(inbound_buffer->buffer);
    return inbound_buffer->buffer_length >= sizeof(uint32_t) + message_length;
}


InboundMessage inbound_buffer_take_complete_message(InboundBuffer* inbound_buffer) {
    if (!inbound_buffer_has_complete_message(inbound_buffer)) {
        puts("Error: No complete message in buffer");
        exit(EXIT_FAILURE);
    }

    InboundMessage inbound_message = {
        .message = message_deserialise_no_free(inbound_buffer->buffer + sizeof(uint32_t)),
        .sender_address = inbound_buffer->sender_address,
    };

    uint32_t message_length = *(uint32_t*)(inbound_buffer->buffer);

    size_t new_buffer_length = inbound_buffer->buffer_length - sizeof(uint32_t) - message_length;
    char* new_buffer = malloc(new_buffer_length);
    memcpy(new_buffer, inbound_buffer->buffer + sizeof(uint32_t) + message_length, new_buffer_length);
    free(inbound_buffer->buffer);
    inbound_buffer->buffer = new_buffer;
    inbound_buffer->buffer_length = new_buffer_length;

    return inbound_message;
}


void inbound_buffer_push_data(InboundBuffer* inbound_buffer, char const* data, size_t data_length) {
    inbound_buffer->buffer = realloc(inbound_buffer->buffer, inbound_buffer->buffer_length + data_length);
    memcpy(inbound_buffer->buffer + inbound_buffer->buffer_length, data, data_length);
    inbound_buffer->buffer_length += data_length;
}


Network network_new() {
    Network network = {
        .inbound_buffer_count = 0,
        .inbound_buffers = NULL,
    };

    return network;
}

void network_free(Network network) {
    for (size_t i = 0; i < network.inbound_buffer_count; ++i) {
        inbound_buffer_free(network.inbound_buffers[i]);
    }
    free(network.inbound_buffers);
}

void network_append_inbound_buffer(Network* network, struct sockaddr sender_address) {
    network->inbound_buffer_count += 1;
    network->inbound_buffers = realloc(network->inbound_buffers, network->inbound_buffer_count * sizeof(InboundBuffer));
    network->inbound_buffers[network->inbound_buffer_count - 1] = inbound_buffer_new(sender_address);
}

void network_push_inbound_data(Network* network, char const* data, size_t data_length, struct sockaddr sender_address) {
    for (size_t i = 0; i < network->inbound_buffer_count; ++i) {
        if (memcmp(&network->inbound_buffers[i].sender_address, &sender_address, sizeof(struct sockaddr)) == 0) {
            inbound_buffer_push_data(&network->inbound_buffers[i], data, data_length);
            return;
        }
    }

    network_append_inbound_buffer(network, sender_address);
    inbound_buffer_push_data(&network->inbound_buffers[network->inbound_buffer_count - 1], data, data_length);
}

bool network_take_any_complete_message(Network* network, InboundMessage* inbound_message_result) {
    for (size_t i = 0; i < network->inbound_buffer_count; ++i) {
        if (inbound_buffer_has_complete_message(&network->inbound_buffers[i])) {
            *inbound_message_result = inbound_buffer_take_complete_message(&network->inbound_buffers[i]);
            return true;
        }
    }
    return false;
}


InboundMessage network_receive_message_blocking(Network* network, int so) {
    char buffer[64];

    for(;;) {
        struct sockaddr sender_address;
        socklen_t sender_address_length = sizeof(sender_address);

        ssize_t received_bytes;
        if((received_bytes = recvfrom(so, buffer, sizeof(buffer), 0, &sender_address, &sender_address_length)) < 0) {
            perror("in recvfrom_all(): recvfrom() error");
            exit(EXIT_FAILURE);
        }

        network_push_inbound_data(network, buffer, received_bytes, sender_address);

        InboundMessage inbound_message;
        if (network_take_any_complete_message(network, &inbound_message)) {
            return inbound_message;
        }
    }
}


static void sendto_all(int so, char* buffer, size_t buffer_size, struct sockaddr_in receiver_address) {
    size_t sent_bytes = 0;

    while(sent_bytes < buffer_size) {
	ssize_t result;
	if((result = sendto(so, buffer + sent_bytes, buffer_size - sent_bytes, 0, (struct sockaddr*)&receiver_address, sizeof(struct sockaddr))) < 0) {
	    perror("in sendto_all(): sendto() error");
	    exit(EXIT_FAILURE);
	}

	sent_bytes += result;
    }
}


AckStatus network_send_and_free_message(Network* network, int so, OutboundMessage message) {
    (void)network;
    printf("DEBUG sending:\n");
    message_println(message.message);
    uint32_t bytes_length = message_serialised_length(message.message);
    char* bytes = message_serialise_and_free(message.message);
    sendto_all(so, (char*)&bytes_length, sizeof(bytes_length), *(struct sockaddr_in*)(&message.receiver_address));
    sendto_all(so, bytes, bytes_length, *(struct sockaddr_in*)(&message.receiver_address));
    free(bytes);
    return ack_received;
}





/*

static void recvfrom_all(int so, char* buffer, size_t buffer_size, struct sockaddr* sender_address, socklen_t* sender_address_length) {
    size_t received_bytes = 0;

    while(received_bytes < buffer_size) {
	ssize_t result;
	if((result = recvfrom(so, buffer + received_bytes, buffer_size - received_bytes, 0, sender_address, sender_address_length)) < 0) {
	    perror("in recvfrom_all(): recvfrom() error");
	    exit(EXIT_FAILURE);
	}

	received_bytes += result;
    }
}


*/



/*
static void send_and_free_message_no_ack(OutboundMessage message, int so) {
    uint32_t bytes_length = message_serialised_length(message.message);
    char* bytes = message_serialise_and_free(message.message);
    sendto_all(so, (char*)&bytes_length, sizeof(bytes_length), *(struct sockaddr_in*)(&message.receiver_address));
    sendto_all(so, bytes, bytes_length, *(struct sockaddr_in*)(&message.receiver_address));
    free(bytes);
}

static InboundMessage receive_message_blocking_no_ack(int so) {
    uint32_t bytes_length = 0;
    struct sockaddr sender_address;
    socklen_t sender_address_length = sizeof(sender_address);

    recvfrom_all(so, (char*)&bytes_length, sizeof(bytes_length), &sender_address, &sender_address_length);

    char* bytes = malloc(bytes_length);
    recvfrom_all(so, bytes, bytes_length, NULL, NULL);

    Message message = message_deserialise_and_free(bytes);


    InboundMessage inbound_message = {
        .message = message, 
        .sender_address = sender_address
    };

    return inbound_message;
}




static void ack(int so, InboundMessage inbound_message) {
    Message message = {
        .id = message_next_id(),
        .type = message_ack,
        .data = {
            .ack = {
                .message_id = inbound_message.message.id,
            },
        },
    };

    OutboundMessage outbound_message = {
        .message = message,
        .receiver_address = inbound_message.sender_address,
    };

    send_and_free_message_no_ack(outbound_message, so);
}

static AckStatus receive_ack(int so, OutboundMessage outbound_message) {
    InboundMessage inbound_message = receive_message_blocking_no_ack(so);
    if (inbound_message.message.type == message_ack && inbound_message.message.data.ack.message_id == outbound_message.message.id) {
        return ack_received;
    } else {
        return ack_lost;
    }
}



// DEBUG ack
AckStatus send_and_free_message(OutboundMessage message, int so) {
    send_and_free_message_no_ack(message, so);
    //return receive_ack(so, message);
    return ack_received;
}


// DEBUG ack
InboundMessage receive_message_blocking(int so) {
    InboundMessage inbound_message = receive_message_blocking_no_ack(so);
    //ack(so, inbound_message);
    return inbound_message;
}
*/

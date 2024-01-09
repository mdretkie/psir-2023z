#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"


InboundBuffer inbound_buffer_new(SOCKADDR sender_address) {
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


bool inbound_buffer_has_complete_message(InboundBuffer const* inbound_buffer) {
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
    char* new_buffer = (char*)malloc(new_buffer_length);
    memcpy(new_buffer, inbound_buffer->buffer + sizeof(uint32_t) + message_length, new_buffer_length);
    free(inbound_buffer->buffer);
    inbound_buffer->buffer = new_buffer;
    inbound_buffer->buffer_length = new_buffer_length;

    return inbound_message;
}


void inbound_buffer_push_data(InboundBuffer* inbound_buffer, char const* data, size_t data_length) {
    inbound_buffer->buffer = (char*)realloc(inbound_buffer->buffer, inbound_buffer->buffer_length + data_length);
    memcpy(inbound_buffer->buffer + inbound_buffer->buffer_length, data, data_length);
    inbound_buffer->buffer_length += data_length;
}


Network network_new(int so) {
    Network network = {
        .so = so,
        .inbound_buffers = NULL,
        .inbound_buffer_count = 0,
    };

    return network;
}

void network_free(Network network) {
    for (size_t i = 0; i < network.inbound_buffer_count; ++i) {
        inbound_buffer_free(network.inbound_buffers[i]);
    }
    free(network.inbound_buffers);
}

void network_append_inbound_buffer(Network* network, SOCKADDR sender_address) {
    network->inbound_buffer_count += 1;
    network->inbound_buffers = (InboundBuffer*)realloc(network->inbound_buffers, network->inbound_buffer_count * sizeof(InboundBuffer));
    network->inbound_buffers[network->inbound_buffer_count - 1] = inbound_buffer_new(sender_address);
}

void network_push_inbound_data(Network* network, char const* data, size_t data_length, SOCKADDR sender_address) {
    for (size_t i = 0; i < network->inbound_buffer_count; ++i) {
        if (memcmp(&network->inbound_buffers[i].sender_address, &sender_address, sizeof(SOCKADDR)) == 0) {
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


static void network_ack_inbound_message(Network* network, InboundMessage const* inbound_message) {
    Message ack_message = {
        .id = message_next_id(),
        .type = message_ack,
        .data = {
            .ack = {
                .message_id = inbound_message->message.id,
            },
        },
    };

    OutboundMessage outbound_message = {
        .message = ack_message,
        .receiver_address = inbound_message->sender_address,
    };

    network_send_and_free_message(network, outbound_message);
}


static void network_receive_ack_for_outbound_message(Network* network, OutboundMessage const* outbound_message) {
    InboundMessage inbound_message = network_receive_message_blocking(network);

    if (inbound_message.message.type == message_ack && inbound_message.message.data.ack.message_id == outbound_message->message.id) {
        return;
    } else {
	#ifdef PSIR_ARDUINO
        printf("ACK for message with id: %u has been lost\n", (short)outbound_message->message.id);
	#else
        printf("ACK for message with id: %u has been lost\n", outbound_message->message.id);
	#endif
    }
}

#ifdef PSIR_ARDUINO

InboundMessage network_receive_message_blocking(Network* network) { }

#else

InboundMessage network_receive_message_blocking(Network* network) {
    char buffer[1024];

    for(;;) {
        SOCKADDR sender_address;
        socklen_t sender_address_length = sizeof(sender_address);

        ssize_t received_bytes;
        if((received_bytes = recvfrom(network->so, buffer, sizeof(buffer), 0, &sender_address, &sender_address_length)) < 0) {
            perror("in recvfrom_all(): recvfrom() error");
            exit(EXIT_FAILURE);
        }

        network_push_inbound_data(network, buffer, received_bytes, sender_address);

        InboundMessage inbound_message;

        if (network_take_any_complete_message(network, &inbound_message)) {
            if (inbound_message.message.type != message_ack) {
                network_ack_inbound_message(network, &inbound_message);
            }

            return inbound_message;
        }
    }
}

#endif



#ifdef PSIR_ARDUINO

static void sendto_all(int so, char* buffer, size_t buffer_size, SOCKADDR_IN receiver_address) { }

#else

static void sendto_all(int so, char* buffer, size_t buffer_size, SOCKADDR_IN receiver_address) {
    size_t sent_bytes = 0;

    while(sent_bytes < buffer_size) {
	ssize_t result;
	if((result = sendto(so, buffer + sent_bytes, buffer_size - sent_bytes, 0, (SOCKADDR*)&receiver_address, sizeof(SOCKADDR))) < 0) {
	    perror("in sendto_all(): sendto() error");
	    exit(EXIT_FAILURE);
	}

	sent_bytes += result;
    }
}

#endif


void network_send_and_free_message_no_ack(Network* network, OutboundMessage message) {
    uint32_t bytes_length = message_serialised_length(&message.message);
    char* bytes = message_serialise_and_free(message.message);
    sendto_all(network->so, (char*)&bytes_length, sizeof(bytes_length), *(SOCKADDR_IN*)(&message.receiver_address));
    sendto_all(network->so, bytes, bytes_length, *(SOCKADDR_IN*)(&message.receiver_address));
    free(bytes);
}


void network_send_and_free_message(Network* network, OutboundMessage message) {
    network_send_and_free_message_no_ack(network, message);

    if (message.message.type != message_ack) {
        network_receive_ack_for_outbound_message(network, &message);
    }
}

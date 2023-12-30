#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "network.h"


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


void send_and_free_message(OutboundMessage message, int so) {
    size_t bytes_length = message_serialised_length(message.message);
    char* bytes = message_serialise_and_free(message.message);
    sendto_all(so, (char*)&bytes_length, sizeof(bytes_length), message.receiver_address);
    sendto_all(so, bytes, bytes_length, message.receiver_address);
    free(bytes);
}

InboundMessage receive_message_blocking(int so) {
    size_t bytes_length = 0;
    struct sockaddr sender_address;
    socklen_t sender_address_length;

    recvfrom_all(so, (char*)&bytes_length, sizeof(bytes_length), &sender_address, &sender_address_length);

    char* bytes = malloc(bytes_length);
    recvfrom_all(so, bytes, bytes_length, &sender_address, &sender_address_length);

    Message message = message_deserialise_and_free(bytes);

    InboundMessage inbound_message = {
        .message = message, 
        .sender_address = sender_address
    };

    return inbound_message;
}

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





static void send_and_free_message_no_ack(OutboundMessage message, int so) {
    uint32_t bytes_length = message_serialised_length(message.message);
    char* bytes = message_serialise_and_free(message.message);
    printf("DEBUG sending first byte %d\n", (int)bytes[0]);
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
    printf("DEBUG receiving first byte %d\n", (int)bytes[0]);

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



AckStatus send_and_free_message(OutboundMessage message, int so) {
    send_and_free_message_no_ack(message, so);
    //return receive_ack(so, message);
    return ack_received;
}


InboundMessage receive_message_blocking(int so) {
    InboundMessage inbound_message = receive_message_blocking_no_ack(so);
    //ack(so, inbound_message);
    return inbound_message;
}

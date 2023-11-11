/*
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>


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


*/

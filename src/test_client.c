#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "test_client.h"
#include "protocol.h"
#include "network.h"
#include "common.h"

void test_client_main() {
    printf("%s Test client started\n", formatted_timestamp());

    struct in_addr server_address_;
    if (inet_pton(AF_INET, "127.0.0.1", &server_address_) == 0) {
	perror("in main(): inet_aton() error");
	exit(EXIT_FAILURE);
    }

    struct sockaddr server_address;
    memset(&server_address, 0, sizeof(server_address));
    ((struct sockaddr_in*)(&server_address))->sin_family = AF_INET;
    ((struct sockaddr_in*)(&server_address))->sin_port = htons(12345);
    ((struct sockaddr_in*)(&server_address))->sin_addr = server_address_;

    int so = socket(AF_INET, SOCK_DGRAM, 0);

    printf("%s Socket created\n", formatted_timestamp());

    MessageData message_data;
    message_data.tuple_space_insert_request.tuple = 
        tuple_new(
            6,
            tuple_int, 5,
            tuple_string, "abcd",
            tuple_int_template,
            tuple_float, 3.14,
            tuple_string_template,
            tuple_float_template
        );


    Message message = {
        .id = message_next_id(),
        .type = message_tuple_space_insert_request,
        .data = message_data,
    };


    OutboundMessage outbound_message = {
        .message = message,
        .receiver_address = server_address,
    };

    send_and_free_message(outbound_message, so);

    printf("%s Message sent\n", formatted_timestamp());



    printf("%s Test client finished\n", formatted_timestamp());
}

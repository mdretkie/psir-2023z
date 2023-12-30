#include "server.h"
#include <stdio.h>
#include "common.h"
#include "tuple_space.h"
#include "network.h"

void server_main() {
    printf("%s Server started\n", formatted_timestamp());

    TupleSpace tuple_space = tuple_space_new();

    printf("%s Tuple space created\n", formatted_timestamp());

    int so = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in listen_address;
    memset(&listen_address, 0, sizeof(listen_address));
    listen_address.sin_family = AF_INET;
    listen_address.sin_port = htons(12345);
    listen_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(so, (struct sockaddr*)&listen_address, sizeof(listen_address)) < 0) {
        perror("in main(): bind() error");
        exit(EXIT_FAILURE);
    }

    printf("%s Listening for messages\n", formatted_timestamp());


    for(;;) {
        InboundMessage inbound_message = receive_message_blocking(so);
        printf("%s Received message from %s:\n", formatted_timestamp(), address_to_text(*(struct sockaddr_in*)(&inbound_message.sender_address)));
        message_println(inbound_message.message);
    }

    tuple_space_free(tuple_space);

    printf("%s Tuple space destroyed\n", formatted_timestamp());

    printf("%s Server finished\n", formatted_timestamp());
}


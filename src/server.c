#include "server.h"
#include <stdio.h>
#include "common.h"
#include "tuple_space.h"
#include "network.h"


void handle_inbound_message(int so, InboundMessage inbound_message, TupleSpace* tuple_space) {
    switch (inbound_message.message.type) {
        case message_ack: 
        case message_tuple_space_get_reply:
            printf("%s Error: unexpected inbound message type\n", formatted_timestamp());
            break;

        case message_tuple_space_insert_request:
            tuple_space_insert(tuple_space, inbound_message.message.data.tuple_space_insert_request.tuple);
            break;

        case message_tuple_space_get_request:
            (void)0;
            TupleSpaceOperationResult result = tuple_space_get(
                tuple_space, 
                inbound_message.message.data.tuple_space_get_request.tuple_template, 
                inbound_message.message.data.tuple_space_get_request.blocking_mode, 
                inbound_message.message.data.tuple_space_get_request.remove_policy
            );

            Message message = {
                .id = message_next_id(),
                .type = message_tuple_space_get_reply,
                .data = {
                    .tuple_space_get_reply = {
                        .result = result,
                    },
                },
            };

            OutboundMessage outbound_message = {
                .message = message,
                .receiver_address = inbound_message.sender_address,
            };

            if (send_and_free_message(outbound_message, so) == ack_lost) {
                printf("%s Error: ACK lost\n", formatted_timestamp());
            }
            break;
    }
}


void server_main() {
    printf("%s Server started\n", formatted_timestamp());

    TupleSpace tuple_space = tuple_space_new();

    printf("%s Tuple space created\n", formatted_timestamp());

    int so = socket(AF_INET, SOCK_DGRAM, 0);
    if (so == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

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

    Network network = network_new();


    for(;;) {
        InboundMessage inbound_message = network_receive_message_blocking(&network, so);

        printf("%s Received message from %s:\n", formatted_timestamp(), address_to_text(*(struct sockaddr_in*)(&inbound_message.sender_address)));
        message_println(inbound_message.message);

        handle_inbound_message(so, inbound_message, &tuple_space);
    }

    tuple_space_free(tuple_space);

    printf("%s Tuple space destroyed\n", formatted_timestamp());

    printf("%s Server finished\n", formatted_timestamp());
}


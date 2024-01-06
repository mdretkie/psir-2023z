#include "server.h"
#include <stdio.h>
#include "common.h"
#include "tuple_space.h"
#include "network.h"
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>



Server server_new() {
    Server server;

    server.tuple_space = tuple_space_new();

    server.so = socket(AF_INET, SOCK_DGRAM, 0);
    if (server.so == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in listen_address;
    memset(&listen_address, 0, sizeof(listen_address));
    listen_address.sin_family = AF_INET;
    listen_address.sin_port = htons(12345);
    listen_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server.so, (struct sockaddr*)&listen_address, sizeof(listen_address)) < 0) {
        perror("in main(): bind() error");
        exit(EXIT_FAILURE);
    }


    server.network = network_new();

    server.blocked_get_requests = NULL;
    server.blocked_get_request_count = 0;

    return server;
}

void server_free(Server server) {
    free(server.blocked_get_requests);
    network_free(server.network);
    close(server.so);
    tuple_space_free(server.tuple_space);
}

void server_run(Server* server) {
    for(;;) {
        InboundMessage inbound_message = network_receive_message_blocking(&server->network, server->so);
        printf("%s Received message from %s: %s\n", formatted_timestamp(), address_to_text(*(struct sockaddr_in*)(&inbound_message.sender_address)), message_to_string_short(inbound_message.message));

        server_handle_inbound_message_nonblocking(server, inbound_message);
        server_process_blocked_get_requests(server);

    }
}

void server_handle_inbound_message_nonblocking(Server* server, InboundMessage inbound_message) {
    if (inbound_message.message.type == message_tuple_space_get_request && inbound_message.message.data.tuple_space_get_request.blocking_mode == tuple_space_blocking) {
        server_push_blocked_get_request(server, inbound_message);
        return;
    }

    switch (inbound_message.message.type) {
        case message_ack: 
        case message_tuple_space_get_reply:
            printf("%s Error: unexpected inbound message type\n", formatted_timestamp());
            break;

        case message_tuple_space_insert_request:
            tuple_space_insert(&server->tuple_space, inbound_message.message.data.tuple_space_insert_request.tuple);
            break;

        case message_tuple_space_get_request:
            /* Tu zawsze będzie nonblocking. */
            (void)0;

            TupleSpaceOperationResult result = tuple_space_get(
                &server->tuple_space, 
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

            network_send_and_free_message(&server->network, server->so, outbound_message);
            break;
    }
}


void server_push_blocked_get_request(Server* server, InboundMessage inbound_message) {
    server->blocked_get_requests = realloc(server->blocked_get_requests, (server->blocked_get_request_count + 1) * sizeof(InboundMessage));
    server->blocked_get_request_count += 1; 
    server->blocked_get_requests[server->blocked_get_request_count - 1] = inbound_message;
}


void server_process_blocked_get_requests(Server* server) {
    size_t i = 0;
    for(; i < server->blocked_get_request_count; ++i) {
        TupleSpaceOperationResult result = tuple_space_get(
            &server->tuple_space, 
            server->blocked_get_requests[i].message.data.tuple_space_get_request.tuple_template, 
            tuple_space_nonblocking,
            server->blocked_get_requests[i].message.data.tuple_space_get_request.remove_policy
        );

        if (result.status == tuple_space_success) {
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
                .receiver_address = server->blocked_get_requests[i].sender_address,
            };

            network_send_and_free_message(&server->network, server->so, outbound_message);

            server->blocked_get_requests[i] = server->blocked_get_requests[server->blocked_get_request_count - 1];
            server->blocked_get_requests = realloc(server->blocked_get_requests, (server->blocked_get_request_count - 1) * sizeof(InboundMessage));
            server->blocked_get_request_count -= 1; 
        } else {
            i += 1;
            continue;
        }
    }
}



/*
void handle_inbound_message(Network* network, int so, InboundMessage inbound_message, TupleSpace* tuple_space) {
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
            printf("DEBUG calling tuple_space_get()()\n");

            TupleSpaceOperationResult result = tuple_space_get(
                tuple_space, 
                inbound_message.message.data.tuple_space_get_request.tuple_template, 
                inbound_message.message.data.tuple_space_get_request.blocking_mode, 
                inbound_message.message.data.tuple_space_get_request.remove_policy
            );

            printf("DEBUG replying in handle_()\n");

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

            if (network_send_and_free_message(network, so, outbound_message) == ack_lost) {
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

        handle_inbound_message(&network, so, inbound_message, &tuple_space);
        printf("DEBUG handled\n");
    }

    network_free(network);
    tuple_space_free(tuple_space);

    printf("%s Tuple space destroyed\n", formatted_timestamp());

    printf("%s Server finished\n", formatted_timestamp());
}

*/

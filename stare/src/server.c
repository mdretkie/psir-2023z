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

    int so;
    so = socket(AF_INET, SOCK_DGRAM, 0);
    if (so == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in listen_address;
    memset(&listen_address, 0, sizeof(listen_address));
    listen_address.sin_family = AF_INET;
    listen_address.sin_port = htons(12344);
    listen_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(so, (struct sockaddr*)&listen_address, sizeof(listen_address)) < 0) {
        perror("in main(): bind() error");
        exit(EXIT_FAILURE);
    }


    server.network = network_new(so);

    server.blocked_get_requests = NULL;
    server.blocked_get_request_count = 0;

    return server;
}

void server_free(Server server) {
    free(server.blocked_get_requests);
    network_free(server.network);
    close(server.network.so);
    tuple_space_free(server.tuple_space);
}

void server_run(Server* server) {
    for(;;) {
        InboundMessage inbound_message = network_receive_message_blocking(&server->network);

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
        case message_tuple_space_get_reply:
            printf("%s Error: unexpected inbound message type\n", formatted_timestamp());
            break;

        case message_ack: 
            break;

        case message_tuple_space_insert_request:
            tuple_space_insert(&server->tuple_space, inbound_message.message.data.tuple_space_insert_request.tuple);

            printf("%s Tuple space: %s\n", formatted_timestamp(), tuple_space_to_string(&server->tuple_space));
            break;

        case message_tuple_space_get_request:
            /* Tu zawsze będzie nonblocking. */
            (void)0;

            TupleSpaceOperationResult result = tuple_space_get(
                &server->tuple_space, 
                &inbound_message.message.data.tuple_space_get_request.tuple_template, 
                inbound_message.message.data.tuple_space_get_request.blocking_mode, 
                inbound_message.message.data.tuple_space_get_request.remove_policy
            );

            printf("%s Tuple space: %s\n", formatted_timestamp(), tuple_space_to_string(&server->tuple_space));

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

            network_send_and_free_message_no_ack(&server->network, outbound_message);
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
            &server->blocked_get_requests[i].message.data.tuple_space_get_request.tuple_template, 
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

            network_send_and_free_message_no_ack(&server->network, outbound_message);

            printf("%s Tuple space: %s\n", formatted_timestamp(), tuple_space_to_string(&server->tuple_space));

            server->blocked_get_requests[i] = server->blocked_get_requests[server->blocked_get_request_count - 1];
            server->blocked_get_requests = realloc(server->blocked_get_requests, (server->blocked_get_request_count - 1) * sizeof(InboundMessage));
            server->blocked_get_request_count -= 1; 
        } else {
            i += 1;
            continue;
        }
    }
}



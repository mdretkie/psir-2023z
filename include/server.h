#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "tuple_space.h"
#include "network.h"


typedef struct Server {
    TupleSpace tuple_space;
    int so;
    Network network;
    InboundMessage* blocked_get_requests;
    size_t blocked_get_request_count;
} Server;

Server server_new();
void server_free(Server server);

void server_run(Server* server);
void server_handle_inbound_message_nonblocking(Server* server, InboundMessage inbound_message);
void server_push_blocked_get_request(Server* server, InboundMessage inbound_message);
void server_process_blocked_get_requests(Server* server);


#endif

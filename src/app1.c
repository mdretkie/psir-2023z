#include "app1.h"
#include <stdio.h>
#include <strings.h>
#include "common.h"
#include "network.h"
#include "protocol.h"


typedef struct Args {
    int server_socket;
    struct sockaddr server_address;
} Args;


void master_create_task(Args args, int i) {
    Tuple tuple = 
        tuple_new(
            2,
            tuple_string, "is prime",
            tuple_int, i
        );

    Message message = {
        .id = message_next_id(),
        .type = message_tuple_space_insert_request,
        .data = {
            .tuple_space_insert_request = {
                .tuple = tuple,
            },
        },
    };

    OutboundMessage outbound_message = {
        .message = message,
        .receiver_address = args.server_address,
    };

    printf("%s [M]  Creating task for n = %d\n", formatted_timestamp(), i);
    if (send_and_free_message(outbound_message, args.server_socket) != ack_received) {
        printf("%s [M]  Error: ACK lost\n", formatted_timestamp());
    }
}


typedef struct MasterResult {
    bool retrieved;
    int i;
} MasterResult;


MasterResult master_query_result(Args args, char const* query) {
    Tuple tuple_template = 
        tuple_new(
            2,
            tuple_string, query,
            tuple_int_template
        );

    Message message = {
        .id = message_next_id(),
        .type = message_tuple_space_get_request,
        .data = {
            .tuple_space_get_request = {
                .tuple_template = tuple_template,
                .blocking_mode = tuple_space_nonblocking,
                .remove_policy = tuple_space_remove,
            },
        },
    };

    OutboundMessage outbound_message = {
        .message = message,
        .receiver_address = args.server_address,
    };

    if (send_and_free_message(outbound_message, args.server_socket) == ack_lost) {
        printf("%s [M]  Error: ACK lost\n", formatted_timestamp());
    }

    InboundMessage inbound_message = receive_message_blocking(args.server_socket);
    if (inbound_message.message.type != message_tuple_space_get_reply) {
        printf("%s [M]  Error: expected MessageTupleSpaceGetReply\n", formatted_timestamp());
        MasterResult result = {.retrieved = false,};
        return result;
    }

    switch (inbound_message.message.data.tuple_space_get_reply.result.status) {
        case tuple_space_success: {
            MasterResult result = {
                .retrieved = true,
                .i = tuple_get_int(inbound_message.message.data.tuple_space_get_reply.result.tuple, 1),
            };
            return result;
        }
        case tuple_space_failure: {
            MasterResult result = {
                .retrieved = false,
            };
            return result;
        }
        default: __builtin_unreachable();
    }
}


void master_fn(void* args_) {
    Args args = *(Args*)(&args_);

    int n = 32;

    printf("%s [M]  Creating %d tasks\n", formatted_timestamp(), n);
    for(int i = 0; i < n; ++i) {
        master_create_task(args, i);
    }

    printf("%s [M]  Collecting results\n", formatted_timestamp());

    bool* results = malloc(n * sizeof(MasterResult));
    bzero(results, n * sizeof(MasterResult));

    int result_count = 0;

    while (result_count != n) {
        for (int i = 0; i < n; ++i) {
            if 
        master_query_result(args, i
    }

    free(reply_received);
}


void worker_fn(void* args_) {
    Args args = *(Args*)(&args_);

}


void app1_main() {
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

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    Args args = {
        .server_socket = server_socket,
        .server_address = server_address,
    };

}

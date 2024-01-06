#include "app1.h"
#include <stdio.h>
#include <strings.h>
#include "common.h"
#include "network.h"
#include "protocol.h"


typedef struct MasterArgs {
    struct sockaddr server_address;
} MasterArgs;


// https://stackoverflow.com/a/5281794/16766872
bool is_prime(int num)
{
     if (num <= 1) return 0;
     if (num % 2 == 0 && num > 2) return 0;
     for(int i = 3; i < num / 2; i+= 2)
     {
         if (num % i == 0)
             return 0;
     }
     return 1;
}



void master_create_task(Network* network, MasterArgs args, int i) {
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
    network_send_and_free_message(network, outbound_message);
}


typedef struct MasterResult {
    bool retrieved;
    int i;
} MasterResult;


MasterResult master_query_result(Network* network, MasterArgs args, char const* query) {
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

    network_send_and_free_message(network, outbound_message);

    InboundMessage inbound_message = network_receive_message_blocking(network);
    if (inbound_message.message.type != message_tuple_space_get_reply) {
        printf("%s [M]  Error: expected MessageTupleSpaceGetReply\n", formatted_timestamp());
        MasterResult result = {.retrieved = false,};
        return result;
    }

    switch (inbound_message.message.data.tuple_space_get_reply.result.status) {
        case tuple_space_success: {
            MasterResult result = {
                .retrieved = true,
                .i = tuple_get_int(&inbound_message.message.data.tuple_space_get_reply.result.tuple, 1),
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


int master_fn(void* args_) {
    MasterArgs args = *(MasterArgs*)(args_);

    printf("%s [M]  Master starting\n", formatted_timestamp());

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    Network network = network_new(server_socket);

    int n = 32;

    printf("%s [M]  Creating %d tasks\n", formatted_timestamp(), n);
    for(int i = 0; i < n; ++i) {
        master_create_task(&network, args, i);
    }

    printf("%s [M]  Collecting results\n", formatted_timestamp());

    int result_count = 0;

    while (result_count != n) {
        sleep_ms(500);

        MasterResult result = master_query_result(&network, args, "prime");

        if (result.retrieved) {
            result_count += 1;
            printf("%s [M]  Result %d/%d: %d is prime\n", formatted_timestamp(), result_count, n, result.i);
        } else {
            MasterResult result = master_query_result(&network, args, "not prime");

            if (result.retrieved) {
                result_count += 1;
                printf("%s [M]  Result %d/%d: %d is not prime\n", formatted_timestamp(), result_count, n, result.i);
            } else {
                continue;
            }
        }
    }

    printf("%s [M]  Master finished\n", formatted_timestamp());

    network_free(network);

    return 0;
}


typedef struct WorkerArgs {
    int worker_id;
    struct sockaddr server_address;
} WorkerArgs;

int worker_fn(void* args_) {
    WorkerArgs args = *(WorkerArgs*)(args_);

    printf("%s [W%d] Worker %d starting\n", formatted_timestamp(), args.worker_id, args.worker_id);

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    Network network = network_new(server_socket);


    for(;;) {
        Tuple tuple_template = 
            tuple_new(
                2,
                tuple_string, "is prime",
                tuple_int_template
            );
        
        Message message = {
            .id = message_next_id(),
            .type = message_tuple_space_get_request,
            .data = {
                .tuple_space_get_request = {
                    .tuple_template = tuple_template,
                    .blocking_mode = tuple_space_blocking,
                    .remove_policy = tuple_space_remove,
                },
            },
        };

        OutboundMessage outbound_message = {
            .message = message,
            .receiver_address = args.server_address,
        };

        network_send_and_free_message(&network, outbound_message);

        InboundMessage inbound_message = network_receive_message_blocking(&network);
        if (inbound_message.message.type != message_tuple_space_get_reply) {
            printf("%s [W%d] Error: expected MessageTupleSpaceGetReply, got %s\n", formatted_timestamp(), args.worker_id, message_to_string_short(&inbound_message.message));
            continue;
        }

        int i = tuple_get_int(&inbound_message.message.data.tuple_space_get_reply.result.tuple, 1);

        printf("%s [W%d] Processing request for n = %d\n", formatted_timestamp(), args.worker_id, i);

        bool is_prime_result = is_prime(i);
        int sleep_time = random_in_range(100, 1000);
        sleep_ms(sleep_time);

        printf("%s [W%d] Finished processing request for n = %d after %d ms\n", formatted_timestamp(), args.worker_id, i, sleep_time);


        Tuple tuple = 
            tuple_new(
                2,
                tuple_string, is_prime_result ? "prime" : "not prime",
                tuple_int, i
            );

        Message message2 = {
            .id = message_next_id(),
            .type = message_tuple_space_insert_request,
            .data = {
                .tuple_space_insert_request = {
                    .tuple = tuple,
                },
            },
        };

        OutboundMessage outbound_message2 = {
            .message = message2,
            .receiver_address = args.server_address,
        };

        network_send_and_free_message(&network, outbound_message2);
    }

    printf("%s [W%d] Worker %d finished\n", formatted_timestamp(), args.worker_id, args.worker_id);

    network_free(network);

    return 0;
}


void app1_main() {
    printf("%s App 1 starting\n", formatted_timestamp());

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

    MasterArgs master_args = {
        .server_address = server_address,
    };

    thrd_t master_thrd;
    if (thrd_create(&master_thrd, master_fn, &master_args) != thrd_success) {
        printf("Could not create master thread\n");
        exit(EXIT_FAILURE);
    }


    int worker_count = 8;
    thrd_t* worker_thrds = malloc(worker_count * sizeof(thrd_t));

    WorkerArgs* worker_args = malloc(worker_count * sizeof(WorkerArgs));
    
    for (int i = 0; i < worker_count; ++i) {
        worker_args[i].worker_id = i;
        worker_args[i].server_address = server_address;

        if (thrd_create(&worker_thrds[i], worker_fn, &worker_args[i]) != thrd_success) {
            printf("Could not create worker thread\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < worker_count; ++i) {
        if (thrd_join(worker_thrds[i], NULL) != thrd_success) {
            printf("Could not join with worker thread\n");
            exit(EXIT_FAILURE);
        }

    }

    if (thrd_join(master_thrd, NULL) != thrd_success) {
        printf("Could not join with master thread\n");
        exit(EXIT_FAILURE);
    }

    free(worker_args);
    free(worker_thrds);

    printf("%s App 1 finished\n", formatted_timestamp());
}

/*
#include <stdio.h>
#include "tuple_space.h"
#include "common.h"
#include "protocol.h"
*/
#include "server.h"
#include "test_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid arguments\n");
        exit(EXIT_FAILURE);
    }

    if (!strcmp(argv[1], "server")) {
        server_main();
    } else if (!strcmp(argv[1], "testclient")) {
        test_client_main();
    } else {
        printf("Invalid arguments\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;


    /*
    Tuple t1 = tuple_new(
	    2,
	    tuple_int, 5,
	    tuple_string_template
    );

    Tuple t2 = tuple_new(
	    2,
	    tuple_int, 5,
	    tuple_string, "abd"
    );
    tuple_println(t1);
    tuple_println(t2);
    printf("%d\n", tuple_match(t1, t2));

    tuple_free(t1);
    tuple_free(t2);
    */

    /*
    Tuple t = tuple_new(
        6,
        tuple_int, 5,
        tuple_string, "abcd",
        tuple_int_template,
        tuple_float, 3.14,
        tuple_string_template,
        tuple_float_template
    );

    {
	Message message = {
	    .id = message_next_id(),
	    .type = message_ack,
	    .data.ack.message_id = message_next_id()
	};
	message_println(message);
    }
    {
	Message message = {
	    .id = message_next_id(),
	    .type = message_tuple_space_insert_request,
	    .data.tuple_space_insert_request.tuple = t
	};
	message_println(message);
    }
    {
	Message message = {
	    .id = message_next_id(),
	    .type = message_tuple_space_get_request,
	    .data.tuple_space_get_request.tuple_template = t
	};
	message_println(message);
    }
    {
	Message message = {
	    .id = message_next_id(),
	    .type = message_tuple_space_get_reply,
	    .data.tuple_space_get_reply.result.tuple = t
	};
	message_println(message);
    }

    {
	Message message = {
	    .id = message_next_id(),
	    .type = 1234
	};
	message_println(message);
    }

    tuple_free(t);

    //char* buffer = malloc(tuple_serialised_length(t));
    //tuple_serialise(t, buffer);
    //tuple_println(t);
    //tuple_free(t);
    //Tuple tt;
    //tuple_deserialise(&tt, buffer);
    //free(buffer);
    //tuple_println(tt);
    //tuple_free(tt);
    */

}

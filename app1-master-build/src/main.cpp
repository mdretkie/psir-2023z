#include <Arduino.h>
#include <ZsutFeatures.h>
#include <stdio.h>

#include "tuple.c"
#include "common.c"
#include "protocol.c"
#include "network.c"

#define LED     ZSUT_PIN_D2


void create_task(Network* network, ArduinoNetworkAddress server_address, int32_t number) {
    Tuple tuple = {
	.element_count = 2,
	.elements = (TupleElement*)malloc(2 * sizeof(TupleElement))
    };

    tuple.elements[0].type = tuple_string,
    tuple.elements[0].data.data_string = alloc_string("is prime");

    tuple.elements[1].type = tuple_int,
    tuple.elements[1].data.data_int = number;

    Serial.print(F("New task: "));
    Serial.println(tuple_to_string(&tuple));

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
        .receiver_address = server_address,
    };

    network_send_and_free_message(network, outbound_message);
}


void setup() {
    Network network = network_new(0);

    ArduinoNetworkAddress server_address = {
        .address = ZsutIPAddress(127, 0, 0, 1),
        .port = 12344,
    };

    unsigned task_count = 8;
    for (unsigned i = 0; i < task_count; ++i) {
        create_task(&network, server_address, i);
    }


    network_free(network);

    /*
    Tuple tuple = {
	.element_count = 3,
	.elements = (TupleElement*)malloc(3 * sizeof(TupleElement))
    };

    TupleElement element0 = {
        .type = tuple_int,
        .data = {
            .data_int = 0,
        },
    };

    TupleElement element1 = {
        .type = tuple_string,
        .data = {
            .data_string = alloc_string("bcde"),
        },
    };

    TupleElement element2 = {
        .type = tuple_string,
        .data = {
            .data_string = alloc_string("c"),
        },
    };

    tuple.elements[0] = element0;
    tuple.elements[1] = element1;
    tuple.elements[2] = element2;


    Serial.println(tuple_to_string(&tuple));


    Network network = network_new(0);

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
        .receiver_address = {
            .address = ZsutIPAddress(127, 0, 0, 1),
            .port = 12344,
        },
    };

    network_send_and_free_message(&network, outbound_message);

    network_free(network);

    Serial.println("DONE");
    */
}

void loop() {

}



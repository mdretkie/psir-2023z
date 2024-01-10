#include <Arduino.h>
#include <ZsutFeatures.h>
#include <stdio.h>

#include "tuple.c"
#include "common.c"
#include "protocol.c"
#include "network.c"

#define LED     ZSUT_PIN_D2


void setup() {
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

    ZsutPinMode(LED, OUTPUT);
}

void loop() {
    ZsutDigitalWrite(LED, HIGH);
    delay(100);                    
    ZsutDigitalWrite(LED, LOW);
    delay(100);                    
}



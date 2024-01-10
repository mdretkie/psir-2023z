#include <Arduino.h>
#include <ZsutFeatures.h>
#include <stdio.h>

#include "tuple.c"
#include "common.c"
#include "protocol.c"
#include "network.c"


unsigned read_pin() {
    return (ZsutDigitalRead() & ZSUT_PIN_D13) >> 13;
}

void send_state_change(Network* network, ArduinoNetworkAddress server_address, unsigned new_state) {
    Tuple tuple = {
	.element_count = 3,
	.elements = (TupleElement*)malloc(3 * sizeof(TupleElement))
    };

    tuple.elements[0].type = tuple_string;
    tuple.elements[0].data.data_string = alloc_string("app2");

    tuple.elements[1].type = tuple_string;
    tuple.elements[1].data.data_string = alloc_string("state change");

    tuple.elements[2].type = tuple_int;
    tuple.elements[2].data.data_int = new_state;

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


    ZsutPinMode(ZSUT_PIN_D13, INPUT);
    unsigned previous_pin_state = read_pin();

    for (;;) {
        unsigned current_pin_state = read_pin();

        if (current_pin_state != previous_pin_state) {
            Serial.print(F("Pin state change: "));
            Serial.print(previous_pin_state);
            Serial.print(F(" -> "));
            Serial.println(current_pin_state);

            send_state_change(&network, server_address, current_pin_state);

            previous_pin_state = current_pin_state;
        }
    }

    network_free(network);
}

void loop() {

}



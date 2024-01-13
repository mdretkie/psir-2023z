#include <Arduino.h>
#include <ZsutFeatures.h>
#include <stdio.h>

#include "tuple.c"
#include "common.c"
#include "protocol.c"
#include "network.c"
#include "consts.h"

#define LED     ZSUT_PIN_D2


unsigned receive_state_change(Network* network, ArduinoNetworkAddress server_address) {
    Tuple tuple = {
	.element_count = 3,
	.elements = (TupleElement*)malloc(3 * sizeof(TupleElement))
    };


    tuple.elements[0].type = tuple_string;
    tuple.elements[0].data.data_string = alloc_string("app2");

    tuple.elements[1].type = tuple_string;
    tuple.elements[1].data.data_string = alloc_string("state change");

    tuple.elements[2].type = tuple_int_template;


    Message message = {
        .id = message_next_id(),
        .type = message_tuple_space_get_request,
        .data = {
            .tuple_space_get_request = {
                .tuple_template = tuple,
                .blocking_mode = tuple_space_blocking,
                .remove_policy = tuple_space_remove,
            },
        },
    };

    OutboundMessage outbound_message = {
        .message = message,
        .receiver_address = server_address,
    };

    network_send_and_free_message(network, outbound_message);

    InboundMessage inbound_message = network_receive_message_blocking(network);

    int32_t new_state = tuple_get_int(&inbound_message.message.data.tuple_space_get_reply.result.tuple, 2);

    tuple_free(inbound_message.message.data.tuple_space_get_reply.result.tuple);

    return new_state;
}


void setup() {
    byte mac[]={0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x03};
    ZsutEthernet.begin(mac);
    ZsutEthernetUDP udp;
    udp.begin(0);

    Network network = network_new(udp);

    ArduinoNetworkAddress server_address = {
        .address = SERVER_ADDRESS,
        .port = SERVER_PORT,
    };

    unsigned changes_up_count = 0, changes_down_count = 0;

    for (;;) {
        unsigned new_state = receive_state_change(&network, server_address);

        switch (new_state) {
            case 0: {
                changes_down_count += 1;
                break;
            }

            case 1: {
                changes_up_count += 1;
                break;
            }
        }

        Serial.print(formatted_timestamp());
        Serial.print(F(" Changes from 0 to 1: "));
        Serial.print(changes_up_count);
        Serial.print(F(". Changes from 1 to 0: "));
        Serial.print(changes_down_count);
        Serial.println(F("."));
    }

    network_free(network);
}

void loop() {

}



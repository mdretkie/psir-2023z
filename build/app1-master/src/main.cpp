#include <Arduino.h>
#include <ZsutFeatures.h>
#include <stdio.h>

#include "tuple.c"
#include "common.c"
#include "protocol.c"
#include "network.c"
#include "consts.h"

#define LED     ZSUT_PIN_D2


void create_task(Network* network, ArduinoNetworkAddress server_address, int32_t number) {
    Tuple tuple = {
	.element_count = 3,
	.elements = (TupleElement*)malloc(3 * sizeof(TupleElement))
    };

    tuple.elements[0].type = tuple_string,
    tuple.elements[0].data.data_string = alloc_string("app1");

    tuple.elements[1].type = tuple_string,
    tuple.elements[1].data.data_string = alloc_string("is prime");

    tuple.elements[2].type = tuple_int,
    tuple.elements[2].data.data_int = number;

    Serial.print(formatted_timestamp());
    Serial.print(F(" New task: "));
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


void send_query_message(Network* network, ArduinoNetworkAddress server_address, char const* query) {
    Tuple tuple = {
	.element_count = 3,
	.elements = (TupleElement*)malloc(3 * sizeof(TupleElement))
    };

    tuple.elements[0].type = tuple_string,
    tuple.elements[0].data.data_string = alloc_string("app1");

    tuple.elements[1].type = tuple_string;
    tuple.elements[1].data.data_string = alloc_string(query);

    tuple.elements[2].type = tuple_int_template;

    Message message = {
        .id = message_next_id(),
        .type = message_tuple_space_get_request,
        .data = {
            .tuple_space_get_request = {
                .tuple_template = tuple,
                .blocking_mode = tuple_space_nonblocking,
                .remove_policy = tuple_space_remove,
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
    byte mac[]={0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01};
    ZsutEthernet.begin(mac);
    ZsutEthernetUDP udp;
    udp.begin(0);

    Network network = network_new(udp);

    ArduinoNetworkAddress server_address = {
        .address = SERVER_ADDRESS,
        .port = SERVER_PORT,
    };

    unsigned task_count = 32;
    for (unsigned i = 0; i < task_count; ++i) {
        create_task(&network, server_address, i);
    }

    unsigned collected_result_count = 0;

    while (collected_result_count != task_count) {
        send_query_message(&network, server_address, "prime");

        InboundMessage inbound_message = network_receive_message_blocking(&network);

        switch (inbound_message.message.data.tuple_space_get_reply.result.status) {
            case tuple_space_success: {
                int32_t number = tuple_get_int(&inbound_message.message.data.tuple_space_get_reply.result.tuple, 2);
                (void)number;

                collected_result_count += 1;

                Serial.print(formatted_timestamp());
                Serial.print(F(" Reply: "));
                Serial.println(tuple_to_string(&inbound_message.message.data.tuple_space_get_reply.result.tuple));

                tuple_free(inbound_message.message.data.tuple_space_get_reply.result.tuple);

                break;
            }

            case tuple_space_failure: {
                send_query_message(&network, server_address, "not prime");

                InboundMessage inbound_message = network_receive_message_blocking(&network);

                switch (inbound_message.message.data.tuple_space_get_reply.result.status) {
                    case tuple_space_success: {
                        int32_t number = tuple_get_int(&inbound_message.message.data.tuple_space_get_reply.result.tuple, 2);
                        (void)number;

                        collected_result_count += 1;

                        Serial.print(formatted_timestamp());
                        Serial.print(F(" Reply: "));
                        Serial.println(tuple_to_string(&inbound_message.message.data.tuple_space_get_reply.result.tuple));

                        tuple_free(inbound_message.message.data.tuple_space_get_reply.result.tuple);

                        break;
                    }

                    case tuple_space_failure: {
                        delay(1000);
                        break;
                    }
                }

                break;
            }
        }
    }

    Serial.print(formatted_timestamp());
    Serial.println(F(" Done"));

    network_free(network);
}

void loop() {

}



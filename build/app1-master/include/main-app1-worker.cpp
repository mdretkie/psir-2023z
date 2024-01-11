#include <Arduino.h>
#include <ZsutFeatures.h>
#include <stdio.h>

#include "tuple.c"
#include "common.c"
#include "protocol.c"
#include "network.c"

#define LED     ZSUT_PIN_D2


// https://stackoverflow.com/a/5281794/16766872
bool is_prime(int num)
{
    delay(random(0, 2000));
    if (num <= 1) return 0;
    if (num % 2 == 0 && num > 2) return 0;
    for(int i = 3; i < num / 2; i+= 2)
    {
        if (num % i == 0)
            return 0;
    }
    return 1;
}


int32_t receive_task(Network* network, ArduinoNetworkAddress server_address) {
    Tuple tuple = {
        .element_count = 3,
        .elements = (TupleElement*)malloc(3 * sizeof(TupleElement))
    };

    tuple.elements[0].type = tuple_string,
    tuple.elements[0].data.data_string = alloc_string("app1");

    tuple.elements[1].type = tuple_string;
    tuple.elements[1].data.data_string = alloc_string("is prime");

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

    int32_t number = tuple_get_int(&inbound_message.message.data.tuple_space_get_reply.result.tuple, 2);

    Serial.print(F("Accepting task: "));
    Serial.println(tuple_to_string(&inbound_message.message.data.tuple_space_get_reply.result.tuple));

    tuple_free(inbound_message.message.data.tuple_space_get_reply.result.tuple);

    return number;
}


void send_reply(Network* network, ArduinoNetworkAddress server_address, int32_t number) {
    Tuple tuple = {
        .element_count = 3,
        .elements = (TupleElement*)malloc(3 * sizeof(TupleElement))
    };

    tuple.elements[0].type = tuple_string,
    tuple.elements[0].data.data_string = alloc_string("app1");

    tuple.elements[1].type = tuple_string;
    tuple.elements[1].data.data_string = alloc_string(is_prime(number) ? "prime" : "not prime");

    tuple.elements[2].type = tuple_int;
    tuple.elements[2].data.data_int = number;

    Serial.print(F("Sending reply: "));
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
    byte mac[]={0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01};
    ZsutEthernet.begin(mac);
    ZsutEthernetUDP udp;
    udp.begin(0);

    Network network = network_new(udp);

    ArduinoNetworkAddress server_address = {
        .address = ZsutIPAddress(127, 0, 0, 1),
        .port = 12344,
    };


    for(;;) {
        int32_t number = receive_task(&network, server_address);
        send_reply(&network, server_address, number);
    }

    network_free(network);
}

void loop() {
}



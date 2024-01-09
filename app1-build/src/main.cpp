#include <Arduino.h>
#include <ZsutFeatures.h>
#include <stdio.h>

#include "tuple.c"
#include "common.c"
#include "protocol.c"
#include "network.c"

#define LED     ZSUT_PIN_D2

void setup() {
    ZsutPinMode(LED, OUTPUT);

    Tuple t = tuple_new(
	3,
	tuple_string, "abc",
	tuple_int, 24,
	tuple_string_template
	);

    Network network = network_new(0);

    Message message = {
        .id = message_next_id(),
        .type = message_tuple_space_insert_request,
        .data = {
            .tuple_space_insert_request = {
                .tuple = t,
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
}

void loop() {
    ZsutDigitalWrite(LED, HIGH);
    delay(100);                    
    ZsutDigitalWrite(LED, LOW);
    delay(100);                    
}



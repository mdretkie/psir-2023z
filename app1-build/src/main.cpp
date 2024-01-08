#include <Arduino.h>
#include <ZsutFeatures.h>
#include <stdio.h>

#include "tuple.h"

#define LED     ZSUT_PIN_D2

void setup() {
    ZsutPinMode(LED, OUTPUT);

    Tuple t = tuple_new(
	3,
	tuple_string, "abc",
	tuple_int, 24,
	tuple_string_template
	);

    /*
    Serial.println(tuple_to_string(&t));
    */

}

void loop() {
    ZsutDigitalWrite(LED, HIGH);
    delay(100);                    
    ZsutDigitalWrite(LED, LOW);
    delay(100);                    
}


#include "tuple.c"

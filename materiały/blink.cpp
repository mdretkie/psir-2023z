#include <Arduino.h>
#include <ZsutFeatures.h>

#define LED     ZSUT_PIN_D2

void setup() {
    ZsutPinMode(LED, OUTPUT);
}

void loop() {
    ZsutDigitalWrite(LED, HIGH);
    delay(100);                    
    ZsutDigitalWrite(LED, LOW);
    delay(100);                    
}
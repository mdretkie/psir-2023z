#include <Arduino.h>
#include <inttypes.h>
#include <ZsutFeatures.h>

//method to access to particular fields in new_feature I/O Space
uint32_t ZsutMillis(){
    uint32_t t;
    t=(uint32_t)(_SFR_IO8(0x36))<<24;			//Fetching a time you must start from SFR register at address 0x36!!!
    t=t | (uint32_t)(_SFR_IO8(0x37))<<16;
    t=t | (uint32_t)(_SFR_IO8(0x38))<<8;
    t=t | (uint32_t)(_SFR_IO8(0x39));
    return t;
}

uint16_t ZsutDigitalRead(){
    uint16_t t;
    t=(uint16_t)(_SFR_IO8(0x10));               //0x30
    t=t | ((uint16_t)(_SFR_IO8(0x11)))<<8;      //0x31
    return t;
}

void ZsutPinMode(uint16_t p, int mode){
    if((p & ZSUT_PIN_D0)!=0) pinMode(0, mode);
    if((p & ZSUT_PIN_D1)!=0) pinMode(1, mode);
    if((p & ZSUT_PIN_D2)!=0) pinMode(2, mode);
    if((p & ZSUT_PIN_D3)!=0) pinMode(3, mode);
    if((p & ZSUT_PIN_D4)!=0) pinMode(4, mode);
    if((p & ZSUT_PIN_D5)!=0) pinMode(5, mode);
    if((p & ZSUT_PIN_D6)!=0) pinMode(6, mode);
    if((p & ZSUT_PIN_D7)!=0) pinMode(7, mode);
    if((p & ZSUT_PIN_D8)!=0) pinMode(8, mode);
    if((p & ZSUT_PIN_D9)!=0) pinMode(9, mode);
    if((p & ZSUT_PIN_D10)!=0) pinMode(10, mode);
    if((p & ZSUT_PIN_D11)!=0) pinMode(11, mode);
    if((p & ZSUT_PIN_D12)!=0) pinMode(12, mode);
    if((p & ZSUT_PIN_D13)!=0) pinMode(13, mode);
}

void ZsutDigitalWrite(uint16_t p, int state){ 	
	if((p & ZSUT_PIN_D0)!=0) digitalWrite(0, state);
	if((p & ZSUT_PIN_D1)!=0) digitalWrite(1, state);
	if((p & ZSUT_PIN_D2)!=0) digitalWrite(2, state);
	if((p & ZSUT_PIN_D3)!=0) digitalWrite(3, state);
	if((p & ZSUT_PIN_D4)!=0) digitalWrite(4, state);
	if((p & ZSUT_PIN_D5)!=0) digitalWrite(5, state);
	if((p & ZSUT_PIN_D6)!=0) digitalWrite(6, state);
	if((p & ZSUT_PIN_D7)!=0) digitalWrite(7, state);
	if((p & ZSUT_PIN_D8)!=0) digitalWrite(8, state);
	if((p & ZSUT_PIN_D9)!=0) digitalWrite(9, state);
	if((p & ZSUT_PIN_D10)!=0) digitalWrite(10, state);
	if((p & ZSUT_PIN_D11)!=0) digitalWrite(11, state);
	if((p & ZSUT_PIN_D12)!=0) digitalWrite(12, state);
	if((p & ZSUT_PIN_D13)!=0) digitalWrite(13, state);
}

uint16_t ZsutAnalog0Read(){
    uint16_t t;
    t=(uint16_t)(_SFR_IO8(0x12));               //0x32
    t=t | ((uint16_t)(_SFR_IO8(0x13)))<<8;      //0x33
    return t;
}
uint16_t ZsutAnalog1Read(){
    uint16_t t;
    t=(uint16_t)(_SFR_IO8(0x14));               //0x32
    t=t | ((uint16_t)(_SFR_IO8(0x15)))<<8;      //0x33
    return t;
}
uint16_t ZsutAnalog2Read(){
    uint16_t t;
    t=(uint16_t)(_SFR_IO8(0x16));               //0x32
    t=t | ((uint16_t)(_SFR_IO8(0x17)))<<8;      //0x33
    return t;
}
uint16_t ZsutAnalog3Read(){
    uint16_t t;
    t=(uint16_t)(_SFR_IO8(0x18));               //0x32
    t=t | ((uint16_t)(_SFR_IO8(0x19)))<<8;      //0x33
    return t;
}
uint16_t ZsutAnalog4Read(){
    uint16_t t;
    t=(uint16_t)(_SFR_IO8(0x1a));               //0x32
    t=t | ((uint16_t)(_SFR_IO8(0x1b)))<<8;      //0x33
    return t;
}
uint16_t ZsutAnalog5Read(){
    uint16_t t;
    t=(uint16_t)(_SFR_IO8(0x1c));               //0x32
    t=t | ((uint16_t)(_SFR_IO8(0x1d)))<<8;      //0x33
    return t;
}

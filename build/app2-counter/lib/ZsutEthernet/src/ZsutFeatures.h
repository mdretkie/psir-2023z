#ifndef Zsut_FEATURE_h
#define Zsut_FEATURE_h

#define ZSUT_PIN_D0     0x0001
#define ZSUT_PIN_D1     0x0002
#define ZSUT_PIN_D2     0x0004
#define ZSUT_PIN_D3     0x0008
#define ZSUT_PIN_D4     0x0010
#define ZSUT_PIN_D5     0x0020
#define ZSUT_PIN_D6     0x0040
#define ZSUT_PIN_D7     0x0080
#define ZSUT_PIN_D8     0x0100
#define ZSUT_PIN_D9     0x0200
#define ZSUT_PIN_D10    0x0400
#define ZSUT_PIN_D11    0x0800
#define ZSUT_PIN_D12    0x1000
#define ZSUT_PIN_D13    0x2000

uint32_t ZsutMillis();
uint16_t ZsutDigitalRead();
void ZsutDigitalWrite(uint16_t p, int state);
void ZsutPinMode(uint16_t p, int mode);

uint16_t ZsutAnalog0Read();
uint16_t ZsutAnalog1Read();
uint16_t ZsutAnalog2Read();
uint16_t ZsutAnalog3Read();
uint16_t ZsutAnalog4Read();
uint16_t ZsutAnalog5Read();

#endif
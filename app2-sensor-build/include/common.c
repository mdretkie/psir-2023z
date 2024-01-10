#include <time.h>

#include "common.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include "arduino.h"
#ifndef PSIR_ARDUINO
#include <threads.h>
#endif


char* alloc_string(char const* string) {
    char* buffer = (char*)malloc(strlen(string) + 1);
    memcpy(buffer, string, strlen(string) + 1);
    return buffer;
}


int random_in_range(int min, int max) {
    return rand() % (max - min) + min;
}

char* serialise_u32(char* buffer, uint32_t value) {
    memcpy(buffer, &value, sizeof(uint32_t));
    return buffer + sizeof(uint32_t);
}

char const* deserialise_u32(char const* buffer, void* value) {
    memcpy(value, buffer, sizeof(uint32_t));
    return buffer + sizeof(uint32_t);
}

// https://stackoverflow.com/a/12792056/16766872
bool is_le() {
    volatile uint32_t i=0x01234567;
    // return 0 for big endian, 1 for little endian.
    return (*((uint8_t*)(&i))) == 0x67;
}

#ifndef ARDUINO

void sleep_ms(unsigned ms) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = ms * 1000000;
    nanosleep(&ts, &ts);
}


char const* formatted_timestamp() {
    static thread_local char buffer[256];

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    long long int now_ = now.tv_sec * 1000 + now.tv_nsec / 1000000;
    snprintf(buffer, sizeof(buffer), "[%lld]", now_);

    return buffer;
}


char const* address_to_text(struct sockaddr_in address) {
    static thread_local char buffer[256];

    snprintf(buffer, sizeof(buffer), "%s:%u", inet_ntoa(address.sin_addr), (unsigned)address.sin_port);

    return buffer;
}
#endif

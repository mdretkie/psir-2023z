#include <time.h>

#include "common.h"
#include <threads.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

char* alloc_string(char const* string) {
    char* buffer = malloc(strlen(string) + 1);
    memcpy(buffer, string, strlen(string) + 1);
    return buffer;
}

char const* formatted_timestamp() {
    static thread_local char buffer[256];

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    long long int now_ = now.tv_sec * 1000 + now.tv_nsec / 1000000;
    snprintf(buffer, sizeof(buffer), "[%lld]", now_);
    snprintf(buffer, sizeof(buffer), "[%lld]", 0llu);

    return buffer;
}

char const* address_to_text(struct sockaddr_in address) {
    static thread_local char buffer[256];

    snprintf(buffer, sizeof(buffer), "%s:%d", inet_ntoa(address.sin_addr), address.sin_port);

    return buffer;
}


int random_in_range(int min, int max) {
    return rand() % (max - min) + min;
}

void sleep_ms(unsigned ms) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = ms * 1000000;
    nanosleep(&ts, &ts);
}



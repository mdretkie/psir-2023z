#include <time.h>

#include "common.h"
#include <threads.h>
#include <time.h>
#include <stdio.h>

char* alloc_string(char const* string) {
    char* buffer = malloc(strlen(string) + 1);
    memcpy(buffer, string, strlen(string) + 1);
    return buffer;
}

char const* formatted_timestamp() {
    static thread_local char buffer[256];

    // TODO:
    //struct timespec now;
    //clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    //long long int now_ = now.tv_sec * 1000 + now.tv_nsec / 1000000;
    //snprintf(buffer, sizeof(buffer), "[%lld]", now_);
    snprintf(buffer, sizeof(buffer), "[%lld]", 0llu);

    return buffer;
}

char const* address_to_text(struct sockaddr_in address) {
    static thread_local char buffer[256];

    struct in_addr sin_addr = address.sin_addr;
    snprintf(buffer, sizeof(buffer), "%s", inet_ntoa(sin_addr));

    return buffer;
}


#include "common.h"

char* alloc_string(char const* string) {
    char* buffer = malloc(strlen(string) + 1);
    memcpy(buffer, string, strlen(string) + 1);
    return buffer;
}

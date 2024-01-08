#!/usr/bin/env bash
set -e
INCLUDES=$(dirname "${BASH_SOURCE[0]}")/../include
SOURCES=$(ls $(dirname "${BASH_SOURCE[0]}")/../src/*.c)
gcc -Wall -Wextra -Wpedantic -fsanitize=address,undefined -std=gnu17 -o main -I${INCLUDES} ${SOURCES}

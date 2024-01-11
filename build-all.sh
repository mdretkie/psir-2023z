#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(dirname ${BASH_SOURCE[0]})"

rm -rf "${ROOT_DIR}"/build
mkdir -p "${ROOT_DIR}"/build/{server,app1-master,app1-worker,app2-sensor,app2-counter}
gcc -Wall -Wextra -Wpedantic -fsanitize=address,undefined -std=gnu17 -o "${ROOT_DIR}"/build/server/server -I"${ROOT_DIR}"/include $(find "${ROOT_DIR}"/src/ -type f ! -name 'main-app*')




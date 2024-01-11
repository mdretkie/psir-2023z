#!/usr/bin/env bash
set -exuo pipefail

ROOT_DIR="$(dirname ${BASH_SOURCE[0]})"

rm -rf "${ROOT_DIR}"/build
mkdir -p "${ROOT_DIR}"/build/{server,app1-master,app1-worker,app2-sensor,app2-counter}

echo "Building server"
gcc -Wall -Wextra -Wpedantic -fsanitize=address,undefined -std=gnu17 -o "${ROOT_DIR}"/build/server/server -I"${ROOT_DIR}"/include $(find "${ROOT_DIR}"/src/ -type f ! -name 'main-app*')

for APP in app1-master app1-worker app2-sensor app2-counter; do
    echo "Building ${APP}"

    BUILD_DIR="${ROOT_DIR}"/build/"${APP}"

    mkdir -p "${BUILD_DIR}"/{include,src}
    cp include/* "${BUILD_DIR}"/include
    cp src/* "${BUILD_DIR}"/src
    echo '#define PSIR_ARDUINO' > "${BUILD_DIR}"/include/arduino.h
    mv "${BUILD_DIR}"/src/main-"${APP}".cpp "${BUILD_DIR}"/src/main.cpp
    cp materiały/{emulino.eeprom,platformio.ini} "${BUILD_DIR}"
    (cd "${BUILD_DIR}"; platformio run)
done




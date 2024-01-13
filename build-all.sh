#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(dirname ${BASH_SOURCE[0]})"

# rm -rf "${ROOT_DIR}"/build
mkdir -p "${ROOT_DIR}"/build/{server,app1-master,app1-worker,app2-sensor,app2-counter}

echo "Building server"
gcc -Wall -Wextra -Wpedantic -std=gnu17 -o "${ROOT_DIR}"/build/server/server -I"${ROOT_DIR}"/include $(find "${ROOT_DIR}"/src/ -type f ! -name 'main-app*')

for APP in app1-master app1-worker app2-sensor app2-counter; do
    echo "Building ${APP}"

    BUILD_DIR="${ROOT_DIR}"/build/"${APP}"

    mkdir -p "${BUILD_DIR}"/{include,src,lib}

    cp "${ROOT_DIR}"/include/* "${BUILD_DIR}"/include
    cp "${ROOT_DIR}"/src/* "${BUILD_DIR}"/include
    cp "${ROOT_DIR}"/src/main-"${APP}".cpp "${BUILD_DIR}"/src/main.cpp

    echo '#define PSIR_ARDUINO' > "${BUILD_DIR}"/include/arduino.h

    cp "${ROOT_DIR}"/materiały/{emulino.eeprom,platformio.ini} "${BUILD_DIR}"
    cp -R "${ROOT_DIR}"/materiały/lib/ZsutEthernet "${BUILD_DIR}"/lib/

    touch "${BUILD_DIR}"/infile.txt

    (cd "${BUILD_DIR}"; platformio run > /dev/null)
done

cp "${ROOT_DIR}"/app2-sensor-infile.txt "${ROOT_DIR}"/build/app2-sensor/infile.txt

echo "Done"


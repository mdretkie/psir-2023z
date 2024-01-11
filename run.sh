#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(dirname ${BASH_SOURCE[0]})"

case "$1" in
    server)
        "${ROOT_DIR}"/build/server/server
        ;;

    app1-master | app1-worker | app2-sensor | app2-counter)
        (cd "${ROOT_DIR}"/build/"$1"/; EBSimUnoEthCurses -ip 10.0.2.15 .pio/build/uno/firmware.hex)
        ;;

    *)
        echo "Invalid arguments"
esac

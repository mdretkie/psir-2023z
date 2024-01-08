#!/usr/bin/env bash
set -e
cp ../include/{tuple,common,thread_local,tuple_space,protocol}.h  include/
cp ../src/{tuple,common,protocol}.c src/
rm -r .pio
platformio run

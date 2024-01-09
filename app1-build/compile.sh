#!/usr/bin/env bash
set -e
rm include/*
echo '#define PSIR_ARDUINO' > include/arduino.h
cp ../include/{tuple,common,tuple_space,protocol}.h  include/
cp ../src/{tuple,common,protocol}.c include/
# rm -r .pio
platformio run

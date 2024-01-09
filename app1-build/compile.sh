#!/usr/bin/env bash
set -e
rm include/*
echo '#define PSIR_ARDUINO' > include/arduino.h
cp ../include/{tuple,common,tuple_space,protocol,network}.h  include/
cp ../src/{tuple,common,protocol,network}.c include/
# rm -r .pio
platformio run

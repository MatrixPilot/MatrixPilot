#!/bin/bash

mkdir -p build
cd build

mkdir -p SILSIM
cd SILSIM
make -f ../../makefile DEVICE=SILSIM
cd ..

cd ..

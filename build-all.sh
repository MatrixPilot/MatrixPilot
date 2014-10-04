#!/bin/bash

mkdir -p build
cd build

mkdir -p SILSIM
cd SILSIM
make -f ../../makefile SOURCE_DIR=../.. DEVICE=SILSIM
cd ..

mkdir -p UDB5
cd UDB5
make -f ../../makefile SOURCE_DIR=../.. DEVICE=UDB5 TOOLCHAIN=XC16
cd ..

mkdir -p AUAV3
cd AUAV3
make -f ../../makefile SOURCE_DIR=../.. DEVICE=AUAV3 TOOLCHAIN=XC16
cd ..

mkdir -p UDB4
cd UDB4
make -f ../../makefile SOURCE_DIR=../.. DEVICE=UDB4 TOOLCHAIN=XC16
cd ..

cd ..

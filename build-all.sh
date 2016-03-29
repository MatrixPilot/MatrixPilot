#!/bin/bash

mkdir -p build
cd build

mkdir -p SIL
cd SIL
make -j 8 -f ../../makefile DEVICE=SIL
cd ..

mkdir -p AUAV3
cd AUAV3
make -j 8 -f ../../makefile DEVICE=AUAV3 TOOLCHAIN=XC16
cd ..

mkdir -p UDB5
cd UDB5
make -j 8 -f ../../makefile DEVICE=UDB5 TOOLCHAIN=XC16
cd ..

mkdir -p UDB4
cd UDB4
make -j 8 -f ../../makefile DEVICE=UDB4 TOOLCHAIN=XC16
cd ..

find | grep *.hex

cd ..

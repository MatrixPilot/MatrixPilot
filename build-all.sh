#!/bin/bash

mkdir -p build
cd build

mkdir -p MatrixPilot; cd MatrixPilot
mkdir -p SIL; cd SIL
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=SIL
cd ..; mkdir -p AUAV3; cd AUAV3
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=AUAV3 TOOLCHAIN=XC16 DEFS=USE_OSD=OSD_NATIVE
cd ..; mkdir -p UDB5; cd UDB5
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB5 TOOLCHAIN=XC16 DEFS=USE_OSD=OSD_REMZIBI
cd ..; mkdir -p UDB4; cd UDB4
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB4 TOOLCHAIN=XC16 DEFS=USE_OSD=OSD_MINIM
cd ../..

mkdir -p RollPitchYaw; cd RollPitchYaw
mkdir -p AUAV3; cd AUAV3
make -j 8 -f ../../../makefile TARGET_NAME=RollPitchYaw DEVICE=AUAV3 TOOLCHAIN=XC16
cd ..; mkdir -p UDB5; cd UDB5
make -j 8 -f ../../../makefile TARGET_NAME=RollPitchYaw DEVICE=UDB5 TOOLCHAIN=XC16
cd ..; mkdir -p UDB4; cd UDB4
make -j 8 -f ../../../makefile TARGET_NAME=RollPitchYaw DEVICE=UDB4 TOOLCHAIN=XC16
cd ../..

mkdir -p LedTest; cd LedTest
mkdir -p AUAV3; cd AUAV3
make -j 8 -f ../../../makefile TARGET_NAME=LedTest DEVICE=AUAV3 TOOLCHAIN=XC16
cd ..; mkdir -p UDB5; cd UDB5
make -j 8 -f ../../../makefile TARGET_NAME=LedTest DEVICE=UDB5 TOOLCHAIN=XC16
cd ..; mkdir -p UDB4; cd UDB4
make -j 8 -f ../../../makefile TARGET_NAME=LedTest DEVICE=UDB4 TOOLCHAIN=XC16
cd ../..

mkdir -p FlashOSD; cd FlashOSD
mkdir -p AUAV3; cd AUAV3
make -j 8 -f ../../../makefile TARGET_NAME=FlashOSD DEVICE=AUAV3 TOOLCHAIN=XC16 DEFS=USE_OSD_SPI=1
cd ..; mkdir -p UDB5; cd UDB5
make -j 8 -f ../../../makefile TARGET_NAME=FlashOSD DEVICE=UDB5 TOOLCHAIN=XC16
cd ..; mkdir -p UDB4; cd UDB4
make -j 8 -f ../../../makefile TARGET_NAME=FlashOSD DEVICE=UDB4 TOOLCHAIN=XC16 DEFS=USE_OSD_SPI=1
cd ../..

#exit 0

cd ..
echo
echo "The following hex files now exist:-"
find build | grep hex
echo

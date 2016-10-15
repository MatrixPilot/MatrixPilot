#!/bin/bash

git config --get remote.origin.url && git symbolic-ref --short HEAD

mkdir -p build
cd build

mkdir -p MatrixPilot; cd MatrixPilot
mkdir -p SIL; cd SIL
#make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=SIL CONFIG=Cessna
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=SIL
cd ..; mkdir -p AUAV3; cd AUAV3
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=AUAV3 DEFS=USE_OSD=OSD_NATIVE CONFIG=CloudsFly
cd ..; mkdir -p UDB5; cd UDB5
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB5 DEFS=USE_OSD=OSD_REMZIBI HILSIM=1
cd ..; mkdir -p UDB4; cd UDB4
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB4 DEFS=USE_OSD=OSD_MINIM AIRFRAME_TYPE=AIRFRAME_GLIDER
cd ../..

#exit 0

mkdir -p RollPitchYaw; cd RollPitchYaw
mkdir -p AUAV3; cd AUAV3
make -j 8 -f ../../../makefile TARGET_NAME=RollPitchYaw DEVICE=AUAV3
cd ..; mkdir -p UDB5; cd UDB5
make -j 8 -f ../../../makefile TARGET_NAME=RollPitchYaw DEVICE=UDB5
cd ..; mkdir -p UDB4; cd UDB4
make -j 8 -f ../../../makefile TARGET_NAME=RollPitchYaw DEVICE=UDB4
cd ../..

mkdir -p LedTest; cd LedTest
mkdir -p AUAV3; cd AUAV3
make -j 8 -f ../../../makefile TARGET_NAME=LedTest DEVICE=AUAV3
cd ..; mkdir -p UDB5; cd UDB5
make -j 8 -f ../../../makefile TARGET_NAME=LedTest DEVICE=UDB5
cd ..; mkdir -p UDB4; cd UDB4
make -j 8 -f ../../../makefile TARGET_NAME=LedTest DEVICE=UDB4
cd ../..

mkdir -p FlashOSD; cd FlashOSD
mkdir -p AUAV3; cd AUAV3
make -j 8 -f ../../../makefile TARGET_NAME=FlashOSD DEVICE=AUAV3
cd ..; mkdir -p UDB5; cd UDB5
make -j 8 -f ../../../makefile TARGET_NAME=FlashOSD DEVICE=UDB5
cd ..; mkdir -p UDB4; cd UDB4
make -j 8 -f ../../../makefile TARGET_NAME=FlashOSD DEVICE=UDB4 DEFS=USE_OSD_SPI=1
cd ../..

cd ..
echo
echo "The following output files now exist:"
find build | grep out
find build | grep hex

echo 
echo "$0" complete.
echo

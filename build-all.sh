#!/bin/bash

git config --get remote.origin.url && git symbolic-ref --short HEAD

mkdir -p build
cd build

mkdir -p MatrixPilot; cd MatrixPilot
mkdir -p SIL; cd SIL
make -j 8 -f ../../../makefile TARGET_NAME=MPSIM DEVICE=SIL CONFIG=Cessna DEFS=CONSOLE_UART=0
cd ..; mkdir -p AUAV3; cd AUAV3
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=AUAV3 DEFS=USE_OSD=OSD_NATIVE CONFIG=CloudsFly
cd ..; mkdir -p UDB5; cd UDB5
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB5 DEFS='USE_OSD=OSD_REMZIBI HILSIM=1'
cd ..; mkdir -p UDB5_UDB_EXTRA; cd UDB5_UDB_EXTRA
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB5 DEFS='SERIAL_OUTPUT_FORMAT=SERIAL_UDB_EXTRA MAG_YAW_DRIFT=1 HMC5883L MAG_FORWARDS USE_BAROMETER_ALTITUDE=1'
cd ..; mkdir -p UDB5_LOGO; cd UDB5_LOGO
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB5 DEFS='FLIGHT_PLAN_TYPE=FP_LOGO NORADIO=1 USE_CAMERA_STABILIZATION=1'
cd ..; mkdir -p UDB4; cd UDB4
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB4 DEFS='USE_OSD=OSD_MINIM AIRFRAME_TYPE=AIRFRAME_GLIDER GAINS_VARIABLE=1 ALTITUDE_GAINS_VARIABLE=1'
cd ..; mkdir -p UDB4_NV; cd UDB4_NV
make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB4 DEFS='USE_NV_MEMORY=1 USE_I2C1_DRIVER=1 SERIAL_OUTPUT_FORMAT=SERIAL_MAVLINK'
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

#!/bin/bash
command_failed=false

git config --get remote.origin.url && git symbolic-ref --short HEAD

mkdir -p _build
cd _build
 
mkdir -p XPlane; cd XPlane
if ! make -j 8 -f ../../makefile TARGET_NAME=XPlane DEVICE=HIL
	then
	command_failed=true
fi
cd ..
 
mkdir -p MatrixPilot; cd MatrixPilot

mkdir -p SIL_MAVLINK; cd SIL_MAVLINK
if ! make -j 8 -f ../../../makefile TARGET_NAME=MPSIM DEVICE=SIL CONFIG=Cessna DEFS=CONSOLE_UART=0 
	then
	command_failed=true
fi
cd ..; mkdir -p SIL_UDB_EXTRA; cd SIL_UDB_EXTRA
if ! make -j 8 -f ../../../makefile TARGET_NAME=MPSIM DEVICE=SIL CONFIG=Cessna DEFS='CONSOLE_UART=0 SERIAL_OUTPUT_FORMAT=SERIAL_UDB_EXTRA'
	then
	command_failed=true
fi
cd ..; mkdir -p SIL_NONE; cd SIL_NONE
if ! make -j 8 -f ../../../makefile TARGET_NAME=MPSIM DEVICE=SIL CONFIG=Cessna DEFS='CONSOLE_UART=0 SERIAL_OUTPUT_FORMAT=SERIAL_NONE'
	then
	command_failed=true
fi
cd ..; mkdir -p AUAV3; cd AUAV3
if ! make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB5 CONFIG=Cessna
	then
	command_failed=true
fi
 
cd ..; mkdir -p Grobularis; cd Grobularis
if ! make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB5 CONFIG=Grobularis
	then
	command_failed=true
fi
cd ..; mkdir -p E_Glider; cd E_Glider
if ! make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=AUAV3 CONFIG=E_Glider
	then
	command_failed=true
fi 
cd ..; mkdir -p UDB5; cd UDB5
if ! make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB5 DEFS='USE_OSD=OSD_REMZIBI HILSIM=1' 
	then
	command_failed=true 
fi
cd ..; mkdir -p UDB5_UDB_EXTRA; cd UDB5_UDB_EXTRA
if ! make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB5 DEFS='SERIAL_OUTPUT_FORMAT=SERIAL_UDB_EXTRA MAG_YAW_DRIFT=1 HMC5883L MAG_FORWARDS USE_BAROMETER_ALTITUDE=1'
	then
	command_failed=true
fi
cd ..; mkdir -p UDB5_LOGO; cd UDB5_LOGO
if ! make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB5 DEFS='FLIGHT_PLAN_TYPE=FP_LOGO NORADIO=1 USE_CAMERA_STABILIZATION=1'
	then
	command_failed=true
fi
cd ..; mkdir -p UDB4; cd UDB4
if ! make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB4 DEFS='USE_OSD=OSD_MINIM AIRFRAME_TYPE=AIRFRAME_GLIDER GAINS_VARIABLE=1 ALTITUDE_GAINS_VARIABLE=1'
	then
	command_failed=true
fi
cd ..; mkdir -p UDB4_NV; cd UDB4_NV
if ! make -j 8 -f ../../../makefile TARGET_NAME=MatrixPilot DEVICE=UDB4 DEFS='USE_NV_MEMORY=1 USE_I2C1_DRIVER=1 SERIAL_OUTPUT_FORMAT=SERIAL_MAVLINK'
	then
	command_failed=true
fi
cd ../..
 
mkdir -p RollPitchYaw; cd RollPitchYaw
mkdir -p AUAV3; cd AUAV3
if ! make -j 8 -f ../../../makefile TARGET_NAME=RollPitchYaw DEVICE=AUAV3
	then
	command_failed=true
fi
cd ..; mkdir -p UDB5; cd UDB5
if ! make -j 8 -f ../../../makefile TARGET_NAME=RollPitchYaw DEVICE=UDB5
	then
	command_failed=true
fi
cd ..; mkdir -p UDB4; cd UDB4
if ! make -j 8 -f ../../../makefile TARGET_NAME=RollPitchYaw DEVICE=UDB4
	then
	command_failed=true
fi
cd ../..
 
mkdir -p LedTest; cd LedTest
mkdir -p AUAV3; cd AUAV3
if ! make -j 8 -f ../../../makefile TARGET_NAME=LedTest DEVICE=AUAV3
	then
	command_failed=true
fi
cd ..; mkdir -p UDB5; cd UDB5
if ! make -j 8 -f ../../../makefile TARGET_NAME=LedTest DEVICE=UDB5
	then
	command_failed=true
fi
cd ..; mkdir -p UDB4; cd UDB4
if ! make -j 8 -f ../../../makefile TARGET_NAME=LedTest DEVICE=UDB4
	then
	command_failed=true
fi
cd ../..
 
mkdir -p FlashOSD; cd FlashOSD
mkdir -p AUAV3; cd AUAV3
if ! make -j 8 -f ../../../makefile TARGET_NAME=FlashOSD DEVICE=AUAV3
	then
	command_failed=true
fi
cd ..; mkdir -p UDB5; cd UDB5
if ! make -j 8 -f ../../../makefile TARGET_NAME=FlashOSD DEVICE=UDB5
	then
	command_failed=true
fi
cd ..; mkdir -p UDB4; cd UDB4
if ! make -j 8 -f ../../../makefile TARGET_NAME=FlashOSD DEVICE=UDB4 DEFS=USE_OSD_SPI=1
	then
	command_failed=true
fi
cd ../..
 
cd ..
echo
echo "The following output files now exist:"
find _build | grep hex
find _build | grep "\.out"
find _build | grep xpl
find _build | grep px4

echo 
echo "$0" complete.
echo

if [ $command_failed == "true" ]
then
	echo "***** One of the make commands has failed *****"
	exit 1
else
	echo "All make commands have completed successfully."
	exit 0
fi

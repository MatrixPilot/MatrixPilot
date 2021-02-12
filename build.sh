#!/bin/bash
command_failed=false

git config --get remote.origin.url && git symbolic-ref --short HEAD

mkdir -p _build
cd _build

mkdir -p SILSim; cd SILSim
if ! make -j 8 -f ../../makefile TARGET_NAME=MPSIM DEVICE=SIL CONFIG=Cessna DEFS='CONSOLE_UART=0 SERIAL_OUTPUT_FORMAT=SERIAL_UDB_EXTRA MP_WORDSIZE=64'
	then
	command_failed=true
fi
cd ..

mkdir -p JSBSimSIL; cd JSBSimSIL
#if ! make -j 8 -f ../../makefile TARGET_NAME=MatrixPilot DEVICE=JSB CONFIG=Cessna DEFS='CONSOLE_UART=0 SERIAL_OUTPUT_FORMAT=SERIAL_UDB_EXTRA MP_WORDSIZE=64'
if ! make -j 8 -f ../../makefile TARGET_NAME=JSBSimSIL DEVICE=JSB CONFIG=Cessna DEFS='CONSOLE_UART=0 SERIAL_OUTPUT_FORMAT=SERIAL_UDB_EXTRA MP_WORDSIZE=64'
	then
	command_failed=true
fi
cd ..
  
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

# this file is included from makefile

TOOLCHAIN := GCC
ifeq ($(OS),Windows_NT)
TARGET_TYPE := exe
else
TARGET_TYPE := out
endif
CPU :=

modules += Tools/MatrixPilot-JSB Tools/JSB-SIL
incpath += Tools/JSBSimLib/src
#cfgpath := Config
addlibs := ../Tools/JSBSimLib/Debug/JSBSimLib.lib
defines += JSB_SIL
#defines += USE_MAVLINK=1 USE_TELEMETRY=1

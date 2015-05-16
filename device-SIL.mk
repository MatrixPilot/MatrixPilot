# this file is included from makefile

TOOLCHAIN := GCC
ifeq ($(OS),Windows_NT)
TARGET_TYPE := exe
else
TARGET_TYPE := out
endif
CPU :=

modules += Tools/MatrixPilot-SIL
incpath +=
#cfgpath := Config

#defines += USE_MAVLINK=1 USE_TELEMETRY=1

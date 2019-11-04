# this file is included from makefile

TOOLCHAIN := GCC
ifeq ($(OS),Windows_NT)
TARGET_TYPE := exe
LIBS += -lws2_32
else
TARGET_TYPE := out
LIBS += -lm
endif
CPU :=

modules += Tools/MatrixPilot-SIL
incpath +=
#cfgpath := Config

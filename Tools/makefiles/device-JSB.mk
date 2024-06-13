# this file is included from makefile

TOOLCHAIN := GCC
ifeq ($(OS),Windows_NT)
TARGET_TYPE := exe
LIBS += -lws2_32 -lJSBSim
else
TARGET_TYPE := out
LIBS += -lm -lc -lJSBSim
endif

modules += Tools/MatrixPilot-SIL Tools/JSBSimSIL

DEFINES += -DSIL=1
INCLUDES += -I/usr/local/include/JSBSim
WARN += -Wno-unused-variable -Wno-unused-but-set-variable
#cfgpath := Config

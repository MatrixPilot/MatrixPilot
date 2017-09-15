# this file is included from makefile

TOOLCHAIN := GCC
ifeq ($(OS),Windows_NT)
LIBS += -lws2_32 -lOpenGL32
LFLAGS += -shared -L$(ROOT_DIR)Tools/HILSIM_XPlane/lib -lXPLM
defines += IBM=1 WIN=1
else
LIBS += -lm
LFLAGS += -shared -Llib/XPLM.lib
defines += NIX=1 LIN=1
endif

TARGET_TYPE := xpl
#CPU :=

CFLAGS += -fPIC

modules +=
incpath +=


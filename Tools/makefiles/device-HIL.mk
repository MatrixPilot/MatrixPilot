# this file is included from makefile

TOOLCHAIN := GCC
ifeq ($(OS),Windows_NT)
TARGET_TYPE := dll
LIBS := -lws2_32
else
TARGET_TYPE := xpl
LIBS := -lm
endif
CPU :=

CFLAGS += -fPIC

LFLAGS   =  -shared -Llib/XPLM.lib

modules +=
incpath +=


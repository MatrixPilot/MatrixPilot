# this file is included from makefile

CC := gcc
CPP := g++
ifeq ($(OS),Windows_NT) 
LIBS += -lws2_32 -lstdc++
TARGET_ARCH :=
CFLAGS += -DWIN=1 -DWIN32
CPPFLAGS += -std=c++0x
else
LIBS += -lm -lstdc++
TARGET_ARCH :=
CFLAGS += -DNIX=1
endif

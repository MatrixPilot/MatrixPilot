# this file is included from makefile

CC := gcc
ifeq ($(OS),Windows_NT) 
#LIBS += -lws2_32
TARGET_ARCH :=
CFLAGS += -DWIN=1
else
#LIBS += -lm
TARGET_ARCH :=
CFLAGS += -DNIX=1
endif

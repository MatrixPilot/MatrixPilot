# this file is included from makefile

CC       := pic30-gcc.exe
AR       := pic30-ar.exe
BIN2HEX  := pic30-bin2hex.exe
LIBS     := -legacy-libc 
TARGET_ARCH := -mcpu=$(CPU)
AFLAGS += -Wa,-g,--defsym=PSV_ERRATA=1
CFLAGS += -x c -g -Wall -mlarge-code -mlarge-data -legacy-libc
#LFLAGS += -Wl,-Tp$(CPU).gld,-Map="$(TARGET_MAP)",--report-mem
LFLAGS += -Wl,-Tp$(CPU).gld,-Map="$(TARGET_MAP)"

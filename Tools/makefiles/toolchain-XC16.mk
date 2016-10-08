# this file is included from makefile

CC       := xc16-gcc
AR       := xc16-ar
BIN2HEX  := xc16-bin2hex
LIBS     := -legacy-libc
TARGET_ARCH := -mcpu=$(CPU)
AFLAGS += -Wa,-g,--defsym=PSV_ERRATA=1
CFLAGS += -g -omf=elf -legacy-libc -msmart-io=1 -Wall -msfr-warn=off -mlarge-code -mlarge-data -mno-eds-warn
LFLAGS += -omf=elf -Wl,--script=p$(CPU).gld,--heap=256,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="$(TARGET_MAP)"

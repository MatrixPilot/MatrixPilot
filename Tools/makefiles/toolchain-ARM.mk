# this file is included from makefile

CC      := arm-none-eabi-gcc
ASM     := arm-none-eabi-as
LINK    := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
MKFW    := $(SOURCE_DIR)/libSTM/px_mkfw.py
LSCRIPT := $(SOURCE_DIR)/libSTM/stm32f401xe.ld
#LFLAGS  := -T .\gcc.ld -Wl,-Map,$(BINDIR)\$(TARGET_NAME).map
LFLAGS  := -T $(LSCRIPT) -Wl,-Map,$(TARGET_MAP)
LIBS    := -lgcc -lc -lm -lrdimon
#QT := "
#LINKFLAGS = -T .\$(APP_NAME).ld -o $(BINDIR)\$(APP_NAME).elf -Wl,-Map,$(BINDIR)\$(APP_NAME).map,--cref -lm

# Define optimisation level here
OPT = -O0 
#OPT = -O2 -falign-functions=16 -fno-inline -fomit-frame-pointer

CFLAGS := $(OPT) -gdwarf-2 -Wall -Wstrict-prototypes
CFLAGS += -fverbose-asm -ffunction-sections -fdata-sections
#CFLAGS += -fverbose-asm -ffunction-sections -fdata-sections -Wa,-ahlms=$(<:.c=.lst)

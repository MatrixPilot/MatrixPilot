# this file is included from makefile

CC      := arm-none-eabi-gcc
ASM     := arm-none-eabi-as
LINK    := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
MKFW    := $(SOURCE_DIR)/Tools/px_mkfw.py
//LSCRIPT := $(SOURCE_DIR)/libSTM/Src/stm32f401xe.ld
LSCRIPT := $(SOURCE_DIR)/libSTM/stm32f427.ld

# Specify the input linker script and output map file
LFLAGS  := -T $(LSCRIPT) -Wl,-Map,$(TARGET_MAP)

# Specify the heap and stack sizes
LFLAGS  += -Wl,--defsym=__HEAP_SIZE=0x0000 -Wl,--defsym=__STACK_SIZE=0x0100

# Specify the system libraries to be linked with
LIBS    := -lgcc -lc -lm

# Specify the optimisation options here
OPT = -O0 
#OPT = -O2 -falign-functions=16 -fno-inline -fomit-frame-pointer

# Specify the debug information type/level to be build into the image
DBG_INFO := -gdwarf-2

# Specify the compiler warnings to be enabled or disabled
WARNINGS := -Wall -Wstrict-prototypes -Wno-pointer-sign -Wno-unused-but-set-variable -Wno-implicit-function-declaration -Wno-unused-variable -Wno-unused-function -Wno-char-subscripts -Wno-pointer-sign -Wno-implicit-function-declaration

CFLAGS := $(OPT) $(DBG_INFO) $(WARNINGS)

# Specify additional compiler flags
CFLAGS += -fverbose-asm -ffunction-sections -fdata-sections
#CFLAGS += -Wa,-ahlms=$(<:.c=.lst)

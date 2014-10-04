#  = recursively expanded variable
# := simply expanded variables
#::= simply expanded variables (posix standard)
# ?= conditional variable assignment operator, only has an effect if the variable is not yet defined
#
Q := $(if $(V),,@)

SOURCE_DIR ?= ..
#$(warning source_dir = $(SOURCE_DIR))

DEVICE ?= SILSIM
#$(warning device = $(DEVICE))

TARGET_NAME ?= MatrixPilot

ifeq ($(DEVICE),SILSIM) 
TOOLCHAIN := GCC
TARGET_TYPE := exe
CPU :=
endif

ifeq ($(DEVICE),AUAV3) 
#TOOLCHAIN ?= C30
TOOLCHAIN ?= XC16
TARGET_TYPE := hex
CPU := 33EP512MU810
endif

ifneq (,$(filter $(DEVICE), UDB4 UDB5))
TOOLCHAIN ?= C30
TARGET_TYPE := hex
CPU := 33FJ256GP710A
endif

ifeq ($(DEVICE),PX4) 
TOOLCHAIN ?= ARM
TARGET_TYPE := px4
ARCH := armv7e-m
CPU := cortex-m4
FPU := fpv4-sp-d16
TARGET_ARCH := -mcpu=$(CPU) -march=$(ARCH) -mfpu=$(FPU) -mfloat-abi=hard -mthumb -mlong-calls
#TARGET_ARCH := -mcpu=$(CPU) -march=$(ARCH) -mfpu=$(FPU) -mfloat-abi=hard -mthumb-interwork -mlong-calls
endif

TARGET_NAME := $(TARGET_NAME)-$(DEVICE)-$(TOOLCHAIN)
TARGET_MAP  := $(TARGET_NAME).map
TARGET      := $(TARGET_NAME).$(TARGET_TYPE)
$(warning *******************************************************************************)
$(warning Building $(TARGET))
$(warning *******************************************************************************)
#$(warning target = $(TARGET))

# PIC33 PIC33F PIC33E PIC32MX ARM7 TMS WIN NIX
#PLATFORM := WIN32
#ifeq ($(PLATFORM),PIC33) 
#endif

#include ToolChain-$(TOOLCHAIN).mk

ifeq ($(TOOLCHAIN),GCC) 
LIBS := -lws2_32
TARGET_ARCH :=
CFLAGS += -DWIN=1
endif

ifeq ($(TOOLCHAIN),C30) 
#CC       := "C:\Program Files (x86)\Microchip\mplabc30\v3.31\bin\pic30-gcc.exe"
CC       := pic30-gcc.exe
AR       := pic30-ar.exe
BIN2HEX  := pic30-bin2hex.exe
#MCLIBDIR := "C:\Program Files (x86)\Microchip\MPLAB C30\lib"
LIBS     := -legacy-libc 
TARGET_ARCH := -mcpu=$(CPU)
AFLAGS += -Wa,-g,--defsym=PSV_ERRATA=1
CFLAGS += -x c -g -Wall -mlarge-code -mlarge-data -legacy-libc
#LFLAGS += -Wl,-L"C:\Program Files\Microchip\MPLAB C30\lib",-Tp$(CPU).gld,-Map="$(TARGET_MAP)",--report-mem
LFLAGS += -Wl,-Tp$(CPU).gld,-Map="$(TARGET_MAP)"
endif

ifeq ($(TOOLCHAIN),ARM)
CC      := arm-none-eabi-gcc
ASM     := arm-none-eabi-as
LINK    := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
MKFW    := $(SOURCE_DIR)/libSTM/px_mkfw.py
#LSCRIPT := ../../gcc.ld
LSCRIPT := $(SOURCE_DIR)/libSTM/STM32F427VI_FLASH.ld
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
endif

ifeq ($(TOOLCHAIN),XC16) 
#CC       := "C:\Program Files (x86)\Microchip\xc16\v1.10\bin\xc16-gcc.exe"
CC       := xc16-gcc.exe
AR       := xc16-ar.exe
BIN2HEX  := xc16-bin2hex.exe
LIBS     := -legacy-libc 
TARGET_ARCH := -mcpu=$(CPU)
AFLAGS += -Wa,-g,--defsym=PSV_ERRATA=1
CFLAGS += -g -omf=elf -legacy-libc -msmart-io=1 -Wall -msfr-warn=off -mlarge-code -mlarge-data

#"C:\Program Files (x86)\Microchip\xc16\v1.10\bin\xc16-gcc.exe"
# ../../libDCM/gpsParseCommon.c
# -o build/default/production/_ext/1077471063/gpsParseCommon.o
# -c -mcpu=33EP512MU810
# -MMD -MF "build/default/production/_ext/1077471063/gpsParseCommon.o.d"
# -g -omf=elf
# -legacy-libc
# -mlarge-code -mlarge-data
# -O0 
# -I"../../Config" -I"../../MAVLink/include" -I"../../Microchip" -I"../../Microchip/include"
# -DAUAV3
# -msmart-io=1 -Wall -msfr-warn=off

#LFLAGS += -Wl,-Tp$(CPU).gld,-Map="$(TARGET_MAP)",--report-mem
LFLAGS += -omf=elf -Wl,-script=p$(CPU).gld,--heap=256,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library=q,--no-force-link,--smart-io,-Map="$(TARGET_MAP)"
#LFLAGS += -omf=elf -Wl,-script=p$(CPU).gld,--heap=256,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="$(TARGET_MAP)"
# -omf=elf -legacy-libc -Wl,,,--script=p33EP512MU810.gld,--heap=256,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,
#-Map="dist/default/production/MatrixPilot-AUAV3.X.production.map" 

# -mcpu=33EP512MU810
# -omf=elf -legacy-libc -Wl,,--defsym=__MPLAB_BUILD=1,,--script=p33EP512MU810.gld,--heap=256,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="dist/default/production/MatrixPilot-AUAV3.X.production.map" 
endif

# $(call mkoutdir, dir-list)
mkoutdir = $(shell for %%f in ($(subst /,\,$(subst ../,,$(1)))); do [ -d %%f ] || $(MKDIR) %%f)

# $(call source-to-object, source-file-list)
source-to-object = $(subst $(SOURCE_DIR)/,,$(subst .c,.o,$(filter %.c,$1))) \
                   $(subst $(SOURCE_DIR)/,,$(subst .s,.o,$(filter %.s,$1)))

# $(subdirectory)
subdirectory = $(patsubst $(SOURCE_DIR)/%/module.mk,%, \
                 $(word \
                   $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

# $(call make-library, library-name, source-file-list)
define make-library
  libraries += $1
  sources += $2
  $1: $(call source-to-object,$2)
endef
#  $(AR) $(ARFLAGS) $$@ $$^
#  $(warning make-library $1 $2)

ifeq ($(OS),Windows_NT) 
QT = "
RM = del /Q /F
#MV ?= move /Y
MV ?= mv -f
CP = copy /Y
FIND := C:\MinGW\msys\1.0\bin\find.exe
ifdef ComSpec
SHELL := $(ComSpec)
endif
ifdef COMSPEC
SHELL := $(COMSPEC)
endif
else
QT = '
RM = rm -rf
MV ?= mv -f
CP = cp -f
FIND := find
endif
SED := sed
MKDIR := mkdir
#TEST := [
space = $(empty) $(empty)
comma := ,

$(if $(filter $(MAKE_VERSION),3.80 3.81 3.90 3.92),,\
  $(error This makefile requires one of GNU make version ….))

$(if $(filter $(notdir $(SOURCE_DIR)),$(notdir $(CURDIR))),\
  $(error Please run the makefile from the binary tree.))

# Collect information from each module in these four variables.
# Initialize them here as simple variables.
libraries :=
modules := 
sources :=
defines :=

ifeq ($(DEVICE),SILSIM)
modules := $(SOURCE_DIR)/libCntrl $(SOURCE_DIR)/libDCM $(SOURCE_DIR)/MatrixPilot $(SOURCE_DIR)/MAVLink $(SOURCE_DIR)/Tools/MatrixPilot-SIL
#include_dirs := $(SOURCE_DIR)/Config $(SOURCE_DIR)/libUDB
include_dirs := $(SOURCE_DIR)/Config
endif

ifeq ($(DEVICE),PX4)
#modules := $(SOURCE_DIR)/libDCM $(SOURCE_DIR)/MatrixPilot $(SOURCE_DIR)/Tools/MatrixPilot-SIL
#include_dirs := $(SOURCE_DIR)/Config $(SOURCE_DIR)/Tools/MatrixPilot-SIL
modules := $(SOURCE_DIR)/libSTM $(SOURCE_DIR)/libCntrl $(SOURCE_DIR)/libDCM $(SOURCE_DIR)/MatrixPilot $(SOURCE_DIR)/MAVLink
include_dirs := $(SOURCE_DIR)/Config $(SOURCE_DIR)/libSTM
endif

ifneq (,$(filter $(DEVICE), UDB4 UDB5 AUAV3))
#modules += libDCM MAVLink MatrixPilot
#modules += libUDB libFlashFS libVectorMatrix Microchip
modules := $(subst /module.mk,,$(shell $(FIND) $(SOURCE_DIR) -name module.mk))
#include_dirs := $(SOURCE_DIR)/MatrixPilot $(SOURCE_DIR)/libFlashFS $(SOURCE_DIR)/libVectorMatrix $(SOURCE_DIR)/MAVLink/include $(SOURCE_DIR)/Microchip $(SOURCE_DIR)/Microchip/include
#include_dirs := $(SOURCE_DIR)/Config $(SOURCE_DIR)/libUDB $(SOURCE_DIR)/Microchip $(SOURCE_DIR)/Microchip/include $(SOURCE_DIR)/libVectorMatrix
include_dirs := $(SOURCE_DIR)/Config $(SOURCE_DIR)/Microchip $(SOURCE_DIR)/Microchip/include $(SOURCE_DIR)/libVectorMatrix
endif

#include_dirs := $(modules)
#$(warning modules = $(modules))
#$(warning includes = $(include_dirs))
defines += $(DEVICE)=1
DEFINES += $(addprefix -D,$(defines))

vpath %.c $(SOURCE_DIR)
vpath %.s $(SOURCE_DIR)

#create-output-directories := \
#	$(echo) creating output directories \
#	$(shell for %%f in ($(subst /,\,$(modules))); do [ -d %%f ] || $(MKDIR) %%f)

#	$(shell for %%f in ($(subst /,\,$(modules))); do $(MKDIR) %%f)
#	$(shell for %%f in ($(subst /,\,$(modules))); do $(TEST) -d %%f || $(MKDIR) %%f)

#dummy := $(shell test -d $(OUTPUT_DIRECTORY) || mkdir -p $(OUTPUT_DIRECTORY))
                # This is usually easier than making all files depend on
                # $(OUTPUT_DIRECTORY) and having a rule to make it.
                # Note that you must use := instead of = to force it to
                # execute immediately.


objects = $(call source-to-object,$(sources))
dependencies = $(subst .o,.d,$(objects))

INCLUDES += $(addprefix -I,$(include_dirs))
vpath %.h $(include_dirs)
vpath %.inc $(include_dirs)

all: 

#include $(patsubst %,$(SOURCE_DIR)/%/module.mk,$(modules))
include $(addsuffix /module.mk,$(modules))

create-output-directories := \
	$(Q) $(shell for %%f in ($(subst /,\,$(subst $(SOURCE_DIR)/,,$(modules)))); do [ -d %%f ] || $(MKDIR) %%f)

#create-output-directories := \
#	$(warning creating output directories $(subst $(SOURCE_DIR)/,,$(modules))) \
#	$(shell for %%f in ($(subst /,\,$(subst $(SOURCE_DIR)/,,$(modules)))); do [ -d %%f ] || $(MKDIR) %%f)

.PHONY: all
all: $(TARGET)
#all: $(TARGET) $(TARGET_NAME).bin
#all: $(TARGET) $(TARGET_NAME).elf $(TARGET_NAME).bin

.PHONY: libraries
libraries: $(libraries)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(objects) $(libraries) $(dependencies)

ifneq "$(MAKECMDGOALS)" "clean"
  include $(dependencies)
endif

%.d: %.c
	$(Q) $(CC) $(TARGET_ARCH) $(CFLAGS) $(DEFINES) $(INCLUDES) -M $< | \
	$(Q) $(SED) $(QT)s,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,$(QT) > $@.tmp
	$(Q) $(MV) $@.tmp $@

%.o: %.c
	$(Q) $(CC) $(TARGET_ARCH) -c $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

%.d: %.s
	$(Q) $(CC) $(TARGET_ARCH) $(AFLAGS) $(DEFINES) $(INCLUDES) -M $< | \
	$(Q) $(SED) $(QT)s,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,$(QT) > $@.tmp
	$(Q) $(MV) $@.tmp $@

%.o: %.s
	$(Q) $(CC) $(TARGET_ARCH) -c -o $@ $< $(AFLAGS),$(subst $(space),$(comma),$(INCLUDES))

%.exe: $(objects)
	$(CC) -o $@ $(LFLAGS) $(objects) $(LIBS)

%.cof: $(objects)
	$(CC) $(TARGET_ARCH) -o $@ $(objects) $(LFLAGS) $(LIBS)

%.hex: %.cof
	$(Q) $(BIN2HEX) $<

%.elf: $(objects)
	$(CC) $(TARGET_ARCH) $(objects) $(LFLAGS) $(LIBS) -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.px4: %.bin
	$(MKFW) --prototype $(SOURCE_DIR)/libSTM/target.prototype --image $< --outfile $@
	sleep 3

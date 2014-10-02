#  = recursively expanded variable
# := simply expanded variables
#::= simply expanded variables (posix standard)
# ?= conditional variable assignment operator, only has an effect if the variable is not yet defined
#
Q := $(if $(V),,@)

SOURCE_DIR ?= ..

DEVICE ?= SILSIM

TARGET_NAME ?= MatrixPilot

ifeq ($(DEVICE),SILSIM) 
TOOLCHAIN := GCC
ifeq ($(OS),Windows_NT)
TARGET_TYPE := exe
else
TARGET_TYPE := out
endif
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


TARGET_NAME := $(TARGET_NAME)-$(DEVICE)-$(TOOLCHAIN)
TARGET_MAP := $(TARGET_NAME).map
TARGET := $(TARGET_NAME).$(TARGET_TYPE)
$(warning *******************************************************************************)
$(warning Building $(TARGET))
$(warning *******************************************************************************)


ifeq ($(TOOLCHAIN),GCC) 
ifeq ($(OS),Windows_NT) 
LIBS := -lws2_32
TARGET_ARCH :=
CFLAGS += -DWIN=1
else
LIBS := -lm
TARGET_ARCH :=
CFLAGS += -DNIX=1
endif
endif

ifeq ($(TOOLCHAIN),C30) 
#CC       := "C:\Program Files (x86)\Microchip\mplabc30\v3.31\bin\pic30-gcc.exe"
CC       := pic30-gcc.exe
AR       := pic30-ar.exe
BIN2HEX  := pic30-bin2hex.exe
LIBS     := -legacy-libc 
TARGET_ARCH := -mcpu=$(CPU)
AFLAGS += -Wa,-g,--defsym=PSV_ERRATA=1
CFLAGS += -x c -g -Wall -mlarge-code -mlarge-data -legacy-libc
LFLAGS += -Wl,-Tp$(CPU).gld,-Map="$(TARGET_MAP)",--report-mem
endif

ifeq ($(TOOLCHAIN),XC16) 
CC       := xc16-gcc.exe
AR       := xc16-ar.exe
BIN2HEX  := xc16-bin2hex.exe
LIBS     := -legacy-libc 
TARGET_ARCH := -mcpu=$(CPU)
AFLAGS += -Wa,-g,--defsym=PSV_ERRATA=1
CFLAGS += -g -omf=elf -legacy-libc -msmart-io=1 -Wall -msfr-warn=off -mlarge-code -mlarge-data
LFLAGS += -omf=elf -Wl,-script=p$(CPU).gld,--heap=256,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="$(TARGET_MAP)"
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
MKDIR := mkdir
else
QT = '
RM = rm -rf
MV ?= mv -f
CP = cp -f
FIND := find
MKDIR := mkdir -p
endif
SED := sed
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
modules := $(SOURCE_DIR)/libDCM $(SOURCE_DIR)/MatrixPilot $(SOURCE_DIR)/MAVLink $(SOURCE_DIR)/Tools/MatrixPilot-SIL
include_dirs := $(SOURCE_DIR)/Config
endif

ifneq (,$(filter $(DEVICE), UDB4 UDB5 AUAV3))
modules := $(subst /module.mk,,$(shell $(FIND) $(SOURCE_DIR) -name module.mk))
include_dirs := $(SOURCE_DIR)/Config $(SOURCE_DIR)/Microchip $(SOURCE_DIR)/Microchip/include $(SOURCE_DIR)/libVectorMatrix
endif



defines += $(DEVICE)=1
DEFINES += $(addprefix -D,$(defines))

vpath %.c $(SOURCE_DIR)
vpath %.s $(SOURCE_DIR)

objects = $(call source-to-object,$(sources))
dependencies = $(subst .o,.d,$(objects))

INCLUDES += $(addprefix -I,$(include_dirs))
vpath %.h $(include_dirs)
vpath %.inc $(include_dirs)

all: 

#include $(patsubst %,$(SOURCE_DIR)/%/module.mk,$(modules))
include $(addsuffix /module.mk,$(modules))

ifeq ($(OS),Windows_NT) 
create-output-directories := \
	$(Q) $(shell for %%f in ($(subst /,\,$(subst $(SOURCE_DIR)/,,$(modules)))); do [ -d %%f ] || $(MKDIR) %%f)
else
create-output-directories := \
	$(shell for f in $(subst $(SOURCE_DIR)/,,$(modules)); do [ -d $$f ] || $(MKDIR) $$f; done)
endif

.PHONY: all
all: $(TARGET)

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
	$(SED) $(QT)s,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,$(QT) > $@.tmp
	$(Q) $(MV) $@.tmp $@

%.o: %.c
	$(Q) $(CC) $(TARGET_ARCH) -c $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

%.d: %.s
	$(Q) $(CC) $(TARGET_ARCH) $(AFLAGS) $(DEFINES) $(INCLUDES) -M $< | \
	$(SED) $(QT)s,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,$(QT) > $@.tmp
	$(Q) $(MV) $@.tmp $@

%.o: %.s
	$(Q) $(CC) $(TARGET_ARCH) -c -o $@ $< $(AFLAGS),$(subst $(space),$(comma),$(INCLUDES))

%.exe: $(objects)
	$(CC) -o $@ $(LFLAGS) $(objects) $(LIBS)

%.out: %.exe
	mv $< $@

%.cof: $(objects)
	$(CC) $(TARGET_ARCH) -o $@ $(objects) $(LFLAGS) $(LIBS)

%.hex: %.cof
	$(BIN2HEX) $<

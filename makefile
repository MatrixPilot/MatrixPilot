#  = recursively expanded variable
# := simply expanded variables
#::= simply expanded variables (posix standard)
# ?= conditional variable assignment operator, only has an effect if the variable is not yet defined
#

SOURCE_DIR ?= ..

DEVICE ?= SILSIM

TARGET_NAME ?= MatrixPilot

TARGET_NAME := $(TARGET_NAME)-$(DEVICE)

ifeq ($(DEVICE),SILSIM) 
TOOLCHAIN := GCC
TARGET_TYPE := exe
CPU :=
endif


ifeq ($(DEVICE),AUAV3) 
TOOLCHAIN := C30
TARGET_TYPE := hex
CPU := 33EP512MU810
endif

ifneq (,$(filter $(DEVICE), UDB4 UDB5))
TOOLCHAIN := C30
TARGET_TYPE := hex
CPU := 33FJ256GP710A
endif


TARGET := $(TARGET_NAME).$(TARGET_TYPE)
TARGET_MAP := $(TARGET_NAME).map


ifeq ($(TOOLCHAIN),GCC) 
LIBS := -lws2_32
TARGET_ARCH :=
CFLAGS += -DWIN=1
endif

ifeq ($(TOOLCHAIN),C30) 
CC       := "C:\Program Files (x86)\Microchip\mplabc30\v3.31\bin\pic30-gcc.exe"
AR       := "C:\Program Files (x86)\Microchip\mplabc30\v3.31\bin\pic30-ar.exe"
BIN2HEX  := "C:\Program Files (x86)\Microchip\mplabc30\v3.31\bin\pic30-bin2hex.exe"
LIBS     := -legacy-libc 
TARGET_ARCH := -mcpu=$(CPU)
AFLAGS += -Wa,-g,--defsym=PSV_ERRATA=1
CFLAGS += -x c -g -Wall -mlarge-code -mlarge-data -legacy-libc
LFLAGS += -Wl,-Tp$(CPU).gld,-Map="$(TARGET_MAP)",--report-mem
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
endif
SED := sed
MKDIR := mkdir
#TEST := [
FIND := C:\MinGW\msys\1.0\bin\find.exe
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
modules := $(SOURCE_DIR)/libDCM $(SOURCE_DIR)/MatrixPilot $(SOURCE_DIR)/Tools/MatrixPilot-SIL
include_dirs := $(SOURCE_DIR)/Config $(SOURCE_DIR)/libUDB
endif

ifneq (,$(filter $(DEVICE), UDB4 UDB5 AUAV3))
modules := $(subst /module.mk,,$(shell $(FIND) $(SOURCE_DIR) -name module.mk))
include_dirs := $(SOURCE_DIR)/Config $(SOURCE_DIR)/libUDB $(SOURCE_DIR)/Microchip $(SOURCE_DIR)/Microchip/include $(SOURCE_DIR)/libVectorMatrix
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

create-output-directories := \
	$(shell for %%f in ($(subst /,\,$(subst $(SOURCE_DIR)/,,$(modules)))); do [ -d %%f ] || $(MKDIR) %%f)

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
	$(CC) $(TARGET_ARCH) $(CFLAGS) $(DEFINES) $(INCLUDES) -M $< | \
	$(SED) $(QT)s,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,$(QT) > $@.tmp
	$(MV) $@.tmp $@

%.o: %.c
	$(CC) $(TARGET_ARCH) -c $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

%.d: %.s
	$(CC) $(TARGET_ARCH) $(AFLAGS) $(DEFINES) $(INCLUDES) -M $< | \
	$(SED) $(QT)s,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,$(QT) > $@.tmp
	$(MV) $@.tmp $@

%.o: %.s
	$(CC) $(TARGET_ARCH) -c -o $@ $< $(AFLAGS),$(subst $(space),$(comma),$(INCLUDES))

%.exe: $(objects)
	$(CC) -o $@ $(LFLAGS) $(objects) $(LIBS)

%.cof: $(objects)
	$(CC) $(TARGET_ARCH) -o $@ $(objects) $(LFLAGS) $(LIBS)

%.hex: %.cof
	$(BIN2HEX) $<

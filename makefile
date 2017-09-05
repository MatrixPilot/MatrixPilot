#------------------------------------------------------------------------------
# NOTE: (Windows) this makefile assumes that the MinGW\bin & MinGW\msys\1.0\bin
#                 directories are added to the PATH environment variable.
#
# Top level MatrixPilot makefile
#
# - This make system depends upon a mostly clean source tree.
# - It is designed to be invoked from a command script for multiple targets.
# - With the exception of source code generators, all build output is 
#    directed to a binary tree, which will be built.
#
# EXTENDING
# - to add another library (aka, directory) to the build process simply copy 
#   file 'module.mk' from one of the existing sub-directories and edit to suit.
#
# NOTES:
# - the build system now builds libraries where specified in module.mk
#
#
# SOME SIMPLE MAKE SYNTAX:
#  = recursively expanded variable
# := simply expanded variables
#::= simply expanded variables (posix standard)
# ?= conditional variable assignment operator, only has an effect if the variable is not yet defined
#
# DEFINE 'V' ENVIRONMENT VARIABLE IN ORDERR TO BE VERBOSE: (set V=1, set V= to unset (Windows)) 
Q := $(if $(V),,@)

# Establish some sensible defaults for build system critical variables (if not already defined)
TARGET_NAME ?= MatrixPilot
#SOURCE_DIR ?= ..
DEVICE ?= SIL

#$(if $(filter $(MAKE_VERSION),3.80 3.81 3.90 3.92),,\
#  $(error This makefile requires one of GNU make version ….))

# Fetch the makefile directory with a trailing /
ROOT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

#################################################################
# Do not build targets if make invoked from source tree root
NOMAKE:=show
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NOMAKE))))
$(if $(filter $(ROOT_DIR),$(CURDIR)/),\
  $(error Please run the makefile from the binary tree, or use the build-all script.))
endif

#################################################################
# Establish the host system build support tools

ifeq ($(OS),Windows_NT) 
mkoutdir = $(shell for %%f in ($(subst /,\,$(subst ../,,$(1)))); do [ -d %%f ] || $(MKDIR) %%f)
QT = "
RM = del /Q /F
#MV ?= move /Y
MV ?= mv -f
CP = copy /Y
FIND := C:\MinGW\msys\1.0\bin\find.exe
DOT := "C:\Program Files (x86)\Graphviz2.38\bin\dot.exe"
ifdef ComSpec
SHELL := $(ComSpec)
endif
ifdef COMSPEC
SHELL := $(COMSPEC)
endif
MKDIR := mkdir
else
mkoutdir = $(shell for f in $(subst ../,,$(1)); do [ -d $$f ] || $(MKDIR) $$f; done)
QT = '
RM = rm -rf
MV ?= mv -f
CP = cp -f
FIND := find
DOT := dot
MKDIR := mkdir -p
endif
SED := sed
#TEST := [
space = $(empty) $(empty)
comma := ,
CPP := g++

SMC = java -jar $(SOURCE_DIR)/Tools/Smc.jar
ifeq ($(V),)
SMC_FLAGS = -c
else
SMC_FLAGS = -c -verbose
endif

# add or replace existing objects only if newer
ARFLAGS := -r


################################################################################
# From current directory determine relative path to makefile (source root)

define source-path-rel-dir
  TMP_SRC_DIR += ..
endef
$(foreach l,$(subst /, ,$(subst $(ROOT_DIR),,$(CURDIR))),$(eval $(call source-path-rel-dir,$l)))
SOURCE_DIR ?= $(subst $(space),/,$(TMP_SRC_DIR))
#$(warning SOURCE_DIR: $(SOURCE_DIR))
MKFILES_DIR = $(SOURCE_DIR)/Tools/makefiles

################################################################################
# Collect information from each module in the following variables.
# Initialize them here as simple variables.
#libraries :=
#modules := 
#sources :=
#defines :=
#incpath :=
#cfgpath :=
##defines :=$(DEVICE)=1

################################################################################
# Include the target and device specific makefile to load our variables as above

include $(MKFILES_DIR)/target-$(TARGET_NAME).mk
include $(MKFILES_DIR)/device-$(DEVICE).mk
modules := $(addprefix $(SOURCE_DIR)/,$(modules))
#INCPATH := $(addprefix $(SOURCE_DIR)/,$(cfgpath)) $(addprefix $(SOURCE_DIR)/,$(incpath))

ifneq ($(CONFIG),)
INCPATH += $(addprefix $(SOURCE_DIR)/,$(cfgpath)/$(CONFIG))
endif
INCPATH += $(addprefix $(SOURCE_DIR)/,$(cfgpath))
INCPATH += $(addprefix $(SOURCE_DIR)/,$(incpath))

#$(warning INCPATH: $(INCPATH))

################################################################################
# Determine the full target names and include the toolchain specific makefile

#TARGET_LNAME := $(TARGET_NAME)-$(DEVICE)-$(TOOLCHAIN)
ifneq ($(CONFIG),)
TARGET_LNAME := $(TARGET_NAME)-$(DEVICE)-$(CONFIG)
else
TARGET_LNAME := $(TARGET_NAME)-$(DEVICE)
endif
TARGET_MAP := $(TARGET_LNAME).map
TARGET := $(TARGET_LNAME).$(TARGET_TYPE)

include $(MKFILES_DIR)/toolchain-$(TOOLCHAIN).mk

################################################################################
# Support routines/macros

# $(subdirectory)
subdirectory = $(patsubst $(SOURCE_DIR)/%/module.mk,%, \
                 $(word \
                   $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

# $(call source-to-object, source-file-list)
source-to-object = $(subst $(SOURCE_DIR)/,,$(subst .c,.o,$(filter %.c,$1))) \
                   $(subst $(SOURCE_DIR)/,,$(subst .s,.o,$(filter %.s,$1))) \
                   $(subst $(SOURCE_DIR)/,,$(subst .cpp,.o,$(filter %.cpp,$1)))

# $(call make-library, library-name, source-file-list)
define make-library
  libraries += $1
#  sources   += $2

  $1: $(call source-to-object,$2)
	$(Q) $(AR) $(ARFLAGS) $$@ $$^
endef

# $(call make-target, source-file-list)
define make-target
  sources += $2
  $1: $(call source-to-object,$2)
endef

################################################################################
# As we build in the output tree, let make know where to look for source files

vpath %.c $(SOURCE_DIR)
vpath %.s $(SOURCE_DIR)
vpath %.h $(INCPATH)
vpath %.cpp $(SOURCE_DIR)
vpath %.inc $(INCPATH)
vpath %.dot $(SOURCE_DIR)
vpath %.sm $(SOURCE_DIR)

################################################################################
# The final list of objects and a dependency file for each

objects = $(call source-to-object,$(sources))
dependencies = $(subst .o,.d,$(objects))

all: 
include $(addsuffix /module.mk,$(modules))
#include $(patsubst %,$(SOURCE_DIR)/%/module.mk,$(modules))
#INCPATH += $(incpath)
INCLUDES += $(addprefix -I,$(INCPATH))
DEFINES += $(addprefix -D,$(DEVICE)=1 $(DEFS) $(defines))

ifneq ($(MAKE_RESTARTS),1)
$(warning *******************************************************************************)
$(warning $(PWD))
$(warning Building $(TARGET))
$(warning modules: $(subst $(SOURCE_DIR)/,,$(modules)))
$(warning library: $(subst $(SOURCE_DIR)/,,$(libraries)))
$(warning INCPATH: $(subst $(SOURCE_DIR)/,,$(INCPATH)))
$(warning DEFINES: $(subst -D,,$(DEFINES)))
endif

################################################################################
# Mirror the source tree structure into the build target directory

ifeq ($(OS),Windows_NT) 
create-output-directories := \
	$(Q) $(shell for %%f in ($(subst /,\,$(subst $(SOURCE_DIR)/,,$(modules)))); do [ -d %%f ] || $(MKDIR) %%f)
else
create-output-directories := \
	$(shell for f in $(subst $(SOURCE_DIR)/,,$(modules)); do [ -d $$f ] || $(MKDIR) $$f; done)
endif

################################################################################
# Define makes targets

.PHONY: all
all: $(TARGET)

.PHONY: libraries
libraries: $(libraries)

.SECONDARY: $(TARGET_NAME).cof
.SECONDARY: $(TARGET_NAME).elf
.SECONDARY: $(TARGET_NAME).bin

#.PHONY : clean clean_with_libs
.PHONY: clean
clean:
	$(RM) $(TARGET) $(objects) $(libraries) $(dependencies)

#.PHONY: graph
#graph: MatrixPilot/FlightState_sm.dot MatrixPilot/FlightState_sm.png
#graph: MatrixPilot/AppClass_sm.dot MatrixPilot/AppClass_sm.png

show:
	@echo PROJECT  = $(PROJECT)
	@echo CONF     = $(CONF)
	@echo VPATH    = $(VPATH)
	@echo C_SRCS   = $(C_SRCS)
	@echo CPP_SRCS = $(CPP_SRCS)
	@echo C_OBJS_EXT   = $(C_OBJS_EXT)
	@echo C_DEPS_EXT   = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)

################################################################################
# Make sure not to generate dependencies when doing cleans
NODEPS:=clean cleanall cleanlibs cleandirs
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
ifeq ($(V),)
  -include $(dependencies) # suppress output with -
else
  include $(dependencies)
endif
endif

################################################################################
# Dependency and Object generation rules

# With the GNU C compiler, you may wish to use the ‘-MM’ flag instead of ‘-M’. This omits prerequisites on system header files.

%.d: %.c
	$(Q) $(CC) $(TARGET_ARCH) $(CFLAGS) $(DEFINES) $(INCLUDES) -M $< | \
	$(SED) $(QT)s,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,$(QT) > $@.tmp
	$(Q) $(MV) $@.tmp $@

%.o: %.c
	$(Q) $(CC) $(TARGET_ARCH) -c $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

%.d: %.cpp
	$(Q) $(CPP) $(TARGET_ARCH) $(CFLAGS) $(DEFINES) $(INCLUDES) -M $< | \
	$(SED) $(QT)s,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,$(QT) > $@.tmp
	$(Q) $(MV) $@.tmp $@

%.o: %.cpp
	$(Q) $(CPP) $(TARGET_ARCH) -c $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

%.d: %.s
	$(Q) $(CC) $(TARGET_ARCH) $(AFLAGS) $(DEFINES) $(INCLUDES) -M $< | \
	$(SED) $(QT)s,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,$(QT) > $@.tmp
	$(Q) $(MV) $@.tmp $@

# NOTE: this version supports the arm-none-eabi toolchain
#%.o: %.s
#	$(Q) $(ASM) $(TARGET_ARCH) $(AFLAGS) -c $< -o $@ $(INCLUDES) 

%.o: %.s
	$(Q) $(ASM) $(TARGET_ARCH) -c $< -o $@ $(AFLAGS) $(INCLUDES) 

# NOTE: this version may be needed to support the C8/MC16 microchip assembler
#%.o: %.s
#	$(CC) $(TARGET_ARCH) -c $< -o $@ $(AFLAGS),$(subst $(space),$(comma),$(INCLUDES))

# Original microchip command line (works)
#	$(Q) $(CC) $(TARGET_ARCH) -c -o $@ $< $(AFLAGS),$(subst $(space),$(comma),$(INCLUDES))

################################################################################
# Library generation rules (note: seems to work without them)

%.a: %.o
	$(Q) $(AR) -ru $@ $<

################################################################################
# Windows and *nix target rules

%.exe: $(objects) $(libraries)
	$(Q) $(CC) -o $@ $(LFLAGS) $(objects) $(libraries) $(LIBS)

%.xpl: $(objects) $(libraries)
	$(Q) $(CPP) -o $@ $(LFLAGS) $(objects) $(libraries) $(LIBS)

%.out: %.exe
	$(Q) mv $< $@

################################################################################
# Microchip build tools rules

%.cof: $(objects) $(libraries)
	$(Q) $(CC) $(TARGET_ARCH) -o $@ $(objects) $(libraries) $(LFLAGS) $(LIBS)

%.hex: %.cof
	$(Q) $(BIN2HEX) $<

%.elf: $(objects) $(libraries)
	$(Q) $(CC) $(TARGET_ARCH) $(objects) $(LFLAGS) $(libraries) $(LIBS) -o $@

%.bin: %.elf
	$(Q) $(OBJCOPY) -O binary $< $@

################################################################################
# PixHawk project PX4 loader image generation rules

%.px4: %.bin
	$(Q) $(MKFW) --prototype $(SOURCE_DIR)/libSTM/target.prototype --image $< --outfile $@
	$(Q) sleep 1

################################################################################
# State Machine Compiler (SMC) rules

%_sm.h %_sm.c : %.sm
	$(SMC) $(SMC_FLAGS) $<

%_sm.dot: %.sm
	$(SMC) -graph -glevel 1 $<

%_sm.png: %_sm.dot
	$(DOT) -T png -o $@ $<

%_sm.html: %.sm
	$(SMC) -table $<

################################################################################
#

print-%:
	@echo '$*=$($*)'


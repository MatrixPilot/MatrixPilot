UNAME_S := $(shell uname -s)
$(info Development Environment is $(UNAME_S))
ifeq ($(OS),Windows_NT)
    DEFINES := -DAPL=0 -DIBM=1 -DLIN=0 -DWIN=1
    TARGET  := HILSIM-win
    ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
        DEFINES  += -DMINGW
        FRAMEWORK := 
        RM        := rm -rf
        PLUGINS   := /c/users/$(USER)/'X-Plane 10'/Resources/plugins
        LIBS      := -static-libgcc -lws2_32 -L$(PLUGINS) 
        XPLM      := -lXPLM
        XPLM_64   := -lXPLM_64
        PATH_32   := /mingw32/bin
        PATH_64   := /mingw64/bin
        EXPORTS   := -Wl,--version-script=exports.txt
    else
        RM        := del /Q /F
	$(error This make file must be run under MSYS2 on the Windows platform)
    endif    
else
    RM        := rm -rf
    ifeq ($(UNAME_S),Linux)
        PLUGINS   := ~/'X-Plane 10'/Resources/plugins
        DEFINES   := -DAPL=0 -DIBM=0 -DLIN=1 -DNIX=1
        FRAMEWORK := 
        TARGET    := HILSIM-linux
        LIBS      := -static-libgcc -lm
        EXPORTS   := -Wl,--version-script=exports.txt
    endif
    ifeq ($(UNAME_S),Darwin)
        PLUGINS   := ~/Desktop/'X-Plane 10'/Resources/plugins
        DEFINES   := -DAPL=1 -DIBM=0 -DLIN=0 -DNIX=0
        FRAMEWORK := -framework XPLM
        TARGET    := HILSIM-macos
        LIBS      := -lm -lstdc++ -framework OpenGL -F $(PLUGINS) $(FRAMEWORK)
        EXPORTS   :=
    endif
endif

BUILDDIR :=	./build
SRC_BASE :=	.

SOURCES = \
	HILSIM.cpp SerialIO.cpp Setup.cpp stdafx.cpp utility.cpp UDBSocketUnix.c UDBSocketWin.c


INCLUDES = \
	-I$(SRC_BASE)/include

DEFINES  += -DXPLM200=1 -DXPLM210=1

############################################################################


VPATH = $(SRC_BASE)
	
CSOURCES	:= $(filter %.c, $(SOURCES))
CXXSOURCES	:= $(filter %.cpp, $(SOURCES))

CDEPS		:= $(patsubst %.c, $(BUILDDIR)/obj32/%.cdep, $(CSOURCES))
CXXDEPS		:= $(patsubst %.cpp, $(BUILDDIR)/obj32/%.cppdep, $(CXXSOURCES))
COBJECTS	:= $(patsubst %.c, $(BUILDDIR)/obj32/%.o, $(CSOURCES))
CXXOBJECTS	:= $(patsubst %.cpp, $(BUILDDIR)/obj32/%.o, $(CXXSOURCES))
ALL_DEPS	:= $(sort $(CDEPS) $(CXXDEPS))
ALL_OBJECTS	:= $(sort $(COBJECTS) $(CXXOBJECTS))

CDEPS64			:= $(patsubst %.c, $(BUILDDIR)/obj64/%.cdep, $(CSOURCES))
CXXDEPS64		:= $(patsubst %.cpp, $(BUILDDIR)/obj64/%.cppdep, $(CXXSOURCES))
COBJECTS64		:= $(patsubst %.c, $(BUILDDIR)/obj64/%.o, $(CSOURCES))
CXXOBJECTS64	:= $(patsubst %.cpp, $(BUILDDIR)/obj64/%.o, $(CXXSOURCES))
ALL_DEPS64		:= $(sort $(CDEPS64) $(CXXDEPS64))
ALL_OBJECTS64	:= $(sort $(COBJECTS64) $(CXXOBJECTS64))

WARN   := -Wall -Wextra -Wno-unused-parameter 
CFLAGS := $(DEFINES) $(INCLUDES) -fPIC -fvisibility=hidden $(WARN)

# Phony directive tells make that these are "virtual" targets, even if a file named "clean" exists.
.PHONY: all clean $(TARGET)
# Secondary tells make that the .o files are to be kept - they are secondary derivatives, not just
# temporary build products.
.SECONDARY: $(ALL_OBJECTS) $(ALL_OBJECTS64) $(ALL_DEPS)



# Target rules - these just induce the right .xpl files.

$(TARGET):  $(BUILDDIR)/$(TARGET)/32/$(TARGET)32.xpl \
            $(BUILDDIR)/$(TARGET)/64/$(TARGET)64.xpl
	  
	

$(BUILDDIR)/$(TARGET)/64/$(TARGET)64.xpl: $(ALL_OBJECTS64)
	@echo Linking $@
	mkdir -p $(dir $@)
	export PATH=$(PATH_64):$$PATH; g++ -m64 -shared $(EXPORTS) -o $@ $(ALL_OBJECTS64) $(LIBS)  $(XPLM_64)

$(BUILDDIR)/$(TARGET)/32/$(TARGET)32.xpl: $(ALL_OBJECTS)
	@echo Linking $@
	mkdir -p $(dir $@)
	export PATH=$(PATH_32):$$PATH; g++ -m32 -shared $(EXPORTS) -o $@ $(ALL_OBJECTS) $(LIBS)  $(XPLM)

# Compiler rules

# What does this do?  It creates a dependency file where the affected
# files are BOTH the .o itself and the cdep we will output.  The result
# goes in the cdep.  Thus:
# - if the .c itself is touched, we remake the .o and the cdep, as expected.
# - If any header file listed in the cdep turd is changed, rebuild the .o.

$(BUILDDIR)/obj32/%.o : %.c
	mkdir -p $(dir $@)
	export PATH=$(PATH_32):$$PATH; gcc $(CFLAGS) -m32 -c $< -o $@
	export PATH=$(PATH_32):$$PATH; gcc $(CFLAGS) -MM -MT $@ -o $(@:.o=.cdep) $<

$(BUILDDIR)/obj32/%.o : %.cpp
	mkdir -p $(dir $@)
	export PATH=$(PATH_32):$$PATH; g++ $(CFLAGS) -m32 -c $< -o $@
	export PATH=$(PATH_32):$$PATH; g++ $(CFLAGS) -MM -MT $@ -o $(@:.o=.cppdep) $<

$(BUILDDIR)/obj64/%.o : %.c
	mkdir -p $(dir $@)
	export PATH=$(PATH_64):$$PATH; gcc $(CFLAGS) -m64 -c $< -o $@
	export PATH=$(PATH_64):$$PATH; gcc  $(CFLAGS) -MM -MT $@ -o $(@:.o=.cdep) $<

$(BUILDDIR)/obj64/%.o : %.cpp
	mkdir -p $(dir $@)
	export PATH=$(PATH_64):$$PATH; g++ $(CFLAGS) -m64 -c $< -o $@
	export PATH=$(PATH_64):$$PATH; g++ $(CFLAGS) -MM -MT $@ -o $(@:.o=.cppdep) $<

clean:
	@echo Cleaning out everything.
	rm -rf $(BUILDDIR)

# Include any dependency turds, but don't error out if they don't exist.
# On the first build, every .c is dirty anyway.  On future builds, if the
# .c changes, it is rebuilt (as is its dep) so who cares if dependencies
# are stale.  If the .c is the same but a header has changed, this 
# declares the header to be changed.  If a primary header includes a 
# utility header and the primary header is changed, the dependency
# needs a rebuild because EVERY header is included.  And if the secondary
# header is changed, the primary header had it before (and is unchanged)
# so that is in the dependency file too.
-include $(ALL_DEPS)
-include $(ALL_DEPS64)

install:
	cp $(BUILDDIR)/$(TARGET)/64/$(TARGET)64.xpl $(PLUGINS)
	cp $(BUILDDIR)/$(TARGET)/32/$(TARGET)32.xpl $(PLUGINS)

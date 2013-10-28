CC 	 = g++
CPP      = g++
CFLAGS   = -pipe -Wall -W -O2 -DLIN=1 -fPIC
INCPATH  = -Iinclude -I../../libUDB
#LFLAGS   = -shared -Wl,-rpath  -Llib/XPLM.lib
LFLAGS   =  -shared -rdynamic -nodefaultlibs -undefined_warning -lGL -lGLU
XFLAGS  = -DLIN -m32
RM_FILE  = rm -f

OBJECTS_DIR = ./

TARGET   = HILSIM-linux.xpl

SOURCES = HILSIM.cpp SerialIO.cpp UDBSocketUnix.c Setup.cpp stdafx.cpp utility.cpp

OBJECTS = HILSIM.o SerialIO.o UDBSocketUnix.o Setup.o stdafx.o utility.o 

first: all

.SUFFIXES: .c .o .cpp .cc .cxx .C

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

.cpp.o:
	$(CPP) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: Makefile $(TARGET)

$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC)  
	gcc $(XFLAGS) $(INCPATH) -o $(TARGET) $(SOURCES) $(LFLAGS)

clean:
	-$(RM_FILE) $(OBJECTS)
	-$(RM_FILE) *~ core *.core

install: 
	cp $(TARGET)  ~/'X-Plane 9 Demo'/Resources/plugins/

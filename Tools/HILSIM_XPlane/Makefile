CC       = g++
CFLAGS   = -pipe -Wall -W -O2 -DLIN=1
INCPATH  = -Iinclude 
#LFLAGS   = -shared -Wl,-rpath  -Llib/XPLM.lib
LFLAGS   =  -shared -Wl  -Llib/XPLM.lib
RM_FILE  = rm -f

OBJECTS_DIR = ./

TARGET   = HILSIM_ubuntu_12.04.xpl

HEADERS = 
SOURCES = HILSIM.cpp SerialIOUnix.cpp UDBSocketUnix.c Setup.cpp stdafx.cpp utility.cpp

OBJECTS = HILSIM.o SerialIOUnix.o UDBSocketUnix.o Setup.o stdafx.o utility.o 

first: all

.SUFFIXES: .c .o .cpp .cc .cxx .C

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: Makefile $(TARGET)

$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC)  
	$(CC) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean:
	-$(RM_FILE) $(OBJECTS)
	-$(RM_FILE) *~ core *.core



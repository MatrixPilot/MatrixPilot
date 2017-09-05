
ifeq ($(OS),Windows_NT)
DEFINES  = -DWIN=1
BINARY   = .exe
OSOBJS   = ../UDBSocket/UDBSocketWin.o
OSLIB    = ws2_32
RM       = del /Q /F
else
DEFINES  = -DNIX=1
BINARY   = .out
OSOBJS   = ../UDBSocket/UDBSocketUnix.o
OSLIB    = m
RM       = rm -rf
endif

CC       = gcc
CPP      = g++
CFLAGS   = -pipe -Wall -W -O0 $(DEFINES)
INCPATH  = -I. -I../UDBSocket
LFLAGS   =
LIBS     = -l$(OSLIB)

OBJECTS_DIR = ./

TARGET  = SILcat$(BINARY)

HEADERS =

OBJECTS = \
$(OSOBJS) \
SILcat.o \

first: all

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

%.o: %.cpp $(HEADERS)
	$(CPP) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean:
	-$(RM) $(OBJECTS)
	-$(RM) $(TARGET)


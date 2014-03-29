CC = gcc
INCPATH = -I. -I../../libUDB -I../../libDCM -I../../MatrixPilot -I../../FreeRTOS/include -I../../FreeRTOS/portable/MSVC-MingW
TARGET_NAME = logo

RM_FILE = rm -f
ifeq ($(OS),Windows_NT)
	TARGET_EXTENSION = .exe
	FLAGS = -DWIN=1
	LFLAGS = 
	LIBS =
	QT = "
else
	TARGET_EXTENSION = .out
	FLAGS = -DNIX=1
	LFLAGS = -Wl
	LIBS = -lm
	QT = '
endif
CFLAGS = -pipe -Wall -W -O0 $(FLAGS)

OBJECTS_DIR = ./

TARGET = $(TARGET_NAME)$(TARGET_EXTENSION)

OBJECTS = HILSIM.o SerialIO.o UDBSocketWin.o Setup.o stdafx.o utility.o 

OBJECTS = \
	main.o \
	crc16.o \
	stptok.o \
	logo_parser.o \
	logo_source_gen.o \

first: all

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<
	$(CC) -MM $(CFLAGS) $(INCPATH) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e $(QT)s|.*:|$*.o:|$(QT) < $*.d.tmp > $*.d
	@sed -e $(QT)s/.*://$(QT) -e $(QT)s/\\$$//$(QT) < $*.d.tmp | \
		fmt -1 | sed -e $(QT)s/^ *//$(QT) -e $(QT)s/$$/:/$(QT) >> $*.d
	@rm -f $*.d.tmp

#%.o: %.c

all: Makefile $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(LFLAGS) $(OBJECTS) $(LIBS)

clean:
	-$(RM_FILE) $(OBJECTS)
	-$(RM_FILE) *~ core *.core *.d ../../libDCM/*.d ../../MatrixPilot/*.d


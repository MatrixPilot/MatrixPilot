CC       = gcc
CFLAGS   = -pipe -Wall -W -O0 -DLIN=1
INCPATH  = -Iinclude -I. -I../HILSIM_XPlane -I../../libUDB -I../../libDCM -I../../MatrixPilot
LFLAGS   =  -Wl
RM_FILE  = rm -f

OBJECTS_DIR = ./

TARGET   = MatrixPilot

HEADERS = 

SOURCES = \
SIL-udb.c \
SIL-dsp.c \
SIL-eeprom.c \
SIL-events.c \
../HILSIM_XPlane/UDBSocketUnix.c \
 \
../../libDCM/deadReckoning.c \
../../libDCM/estWind.c \
../../libDCM/estYawDrift.c \
../../libDCM/gpsParseCommon.c \
../../libDCM/gpsParseMTEK.c \
../../libDCM/gpsParseSTD.c \
../../libDCM/gpsParseUBX.c \
../../libDCM/libDCM.c \
../../libDCM/mathlibNAV.c \
../../libDCM/rmat.c \
 \
../../MatrixPilot/airspeedCntrl.c \
../../MatrixPilot/altitudeCntrl.c \
../../MatrixPilot/altitudeCntrlVariable.c \
../../MatrixPilot/behavior.c \
../../MatrixPilot/cameraCntrl.c \
../../MatrixPilot/config_tests.c \
../../MatrixPilot/data_services.c \
../../MatrixPilot/data_storage.c \
../../MatrixPilot/euler_angles.c \
../../MatrixPilot/flightplan-logo.c \
../../MatrixPilot/flightplan-waypoints.c \
../../MatrixPilot/main.c \
../../MatrixPilot/MAVLink.c \
../../MatrixPilot/mode_switch.c \
../../MatrixPilot/mp_osd.c \
../../MatrixPilot/navigate.c \
../../MatrixPilot/nv_memory_table.c \
../../MatrixPilot/parameter_table.c \
../../MatrixPilot/pitchCntrl.c \
../../MatrixPilot/rollCntrl.c \
../../MatrixPilot/servoMix.c \
../../MatrixPilot/servoPrepare.c \
../../MatrixPilot/states.c \
../../MatrixPilot/telemetry.c \
../../MatrixPilot/yawCntrl.c

OBJECTS = \
SIL-udb.o \
SIL-dsp.o \
SIL-eeprom.o \
SIL-events.o \
../HILSIM_XPlane/UDBSocketUnix.o \
 \
../../libDCM/deadReckoning.o \
../../libDCM/estWind.o \
../../libDCM/estYawDrift.o \
../../libDCM/gpsParseCommon.o \
../../libDCM/gpsParseMTEK.o \
../../libDCM/gpsParseSTD.o \
../../libDCM/gpsParseUBX.o \
../../libDCM/libDCM.o \
../../libDCM/mathlibNAV.o \
../../libDCM/rmat.o \
 \
../../MatrixPilot/airspeedCntrl.o \
../../MatrixPilot/altitudeCntrl.o \
../../MatrixPilot/altitudeCntrlVariable.o \
../../MatrixPilot/behavior.o \
../../MatrixPilot/cameraCntrl.o \
../../MatrixPilot/config_tests.o \
../../MatrixPilot/data_services.o \
../../MatrixPilot/data_storage.o \
../../MatrixPilot/euler_angles.o \
../../MatrixPilot/flightplan-logo.o \
../../MatrixPilot/flightplan-waypoints.o \
../../MatrixPilot/main.o \
../../MatrixPilot/MAVLink.o \
../../MatrixPilot/mode_switch.o \
../../MatrixPilot/mp_osd.o \
../../MatrixPilot/navigate.o \
../../MatrixPilot/nv_memory_table.o \
../../MatrixPilot/parameter_table.o \
../../MatrixPilot/pitchCntrl.o \
../../MatrixPilot/rollCntrl.o \
../../MatrixPilot/servoMix.o \
../../MatrixPilot/servoPrepare.o \
../../MatrixPilot/states.o \
../../MatrixPilot/telemetry.o \
../../MatrixPilot/yawCntrl.o



first: all

.SUFFIXES: .c .o

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



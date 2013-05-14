CC       = gcc
CFLAGS   = -pipe -Wall -W -O0
INCPATH  = -Iinclude -I. -I../HILSIM_XPlane -I../../libUDB -I../../libDCM -I../../MatrixPilot
LFLAGS   = -Wl
LIBS     = -lm
RM_FILE  = rm -f

OBJECTS_DIR = ./


MPSIL_TARGET  = matrixpilot

MP_HEADERS = ../../MatrixPilot/options.h ../../MatrixPilot/waypoints.h ../../MatrixPilot/flightplan-logo.h

MPSIL_OBJECTS = \
SIL-udb.o \
SIL-ui-mp-term.o \
SIL-serial.o \
SIL-dsp.o \
SIL-eeprom.o \
SIL-events.o \
../HILSIM_XPlane/UDBSocketUnix.o \
 \
../../libDCM/deadReckoning.o \
../../libDCM/estAltitude.o \
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


MPCAT_TARGET   = silcat

MPCAT_OBJECTS  = ../HILSIM_XPlane/UDBSocketUnix.o SILcat.o


first: all

%.o: %.c $(MP_HEADERS)
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: $(MPSIL_TARGET) $(MPCAT_TARGET)

$(MPSIL_TARGET): $(MPSIL_OBJECTS)
	$(CC) $(LFLAGS) -o $(MPSIL_TARGET) $(MPSIL_OBJECTS) $(LIBS)

$(MPCAT_TARGET): $(MPCAT_OBJECTS)
	$(CC) $(LFLAGS) -o $(MPCAT_TARGET) $(MPCAT_OBJECTS) $(LIBS)

clean:
	-$(RM_FILE) $(MPSIL_OBJECTS) $(MPCAT_OBJECTS)
	-$(RM_FILE) *~ core *.core



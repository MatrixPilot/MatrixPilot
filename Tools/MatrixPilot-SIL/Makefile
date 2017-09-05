
ifeq ($(OS),Windows_NT)
DEFINES  = -DWIN=1
BINARY   = .exe
OSOBJS   = UDBSocketWin.o
OSLIB    = ws2_32
RM       = del /Q /F
else
DEFINES  = -DNIX=1
BINARY   =
OSOBJS   = UDBSocketUnix.o
OSLIB    = m
RM       = rm -rf
endif

CC       = gcc
CFLAGS   = -pipe -Wall -W -O0 $(DEFINES)
INCPATH  = -I. -I../../Config -I../../libUDB -I../../libDCM -I../../MatrixPilot
LFLAGS   =
LIBS     = -l$(OSLIB)

OBJECTS_DIR = ./


MPSIL_TARGET  = matrixpilot$(BINARY)

MP_HEADERS = \
../../Config/options.h \
../../Config/flightplan-waypoints.h \
../../Config/flightplan-logo.h

MPSIL_OBJECTS = \
SIL-udb.o \
SIL-ui-mp-term.o \
SIL-serial.o \
SIL-dsp.o \
SIL-eeprom.o \
SIL-events.o \
$(OSOBJS) \
 \
../../libDCM/deadReckoning.o \
../../libDCM/estAltitude.o \
../../libDCM/estLocation.o \
../../libDCM/estWind.o \
../../libDCM/estYawDrift.o \
../../libDCM/gpsData.o \
../../libDCM/gpsParseCommon.o \
../../libDCM/gpsParseMTEK.o \
../../libDCM/gpsParseSTD.o \
../../libDCM/gpsParseUBX.o \
../../libDCM/hilsim.o \
../../libDCM/libDCM.o \
../../libDCM/mathlibNAV.o \
../../libDCM/rmat.o \
 \
../../MatrixPilot/airspeedCntrl.o \
../../MatrixPilot/altitudeCntrl.o \
../../MatrixPilot/altitudeCntrlVariable.o \
../../MatrixPilot/behavior.o \
../../MatrixPilot/cameraCntrl.o \
../../MatrixPilot/config.o \
../../MatrixPilot/config_tests.o \
../../MatrixPilot/data_services.o \
../../MatrixPilot/data_storage.o \
../../MatrixPilot/euler_angles.o \
../../MatrixPilot/flightplan.o \
../../MatrixPilot/flightplan-logo.o \
../../MatrixPilot/flightplan-waypoints.o \
../../MatrixPilot/helicalTurnCntrl.o \
../../MatrixPilot/main.o \
../../MatrixPilot/minIni.o \
../../MatrixPilot/MAVLink.o \
../../MatrixPilot/MAVParams.o \
../../MatrixPilot/MAVMission.o \
../../MatrixPilot/MAVFlexiFunctions.o \
../../MatrixPilot/MAVUDBExtra.o \
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


MPCAT_TARGET   = silcat$(BINARY)
MPCAT_OBJECTS  = SILcat.o $(OSOBJS)


first: all

%.o: %.c $(MP_HEADERS)
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

%.o: %.cpp $(MP_HEADERS)
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: $(MPSIL_TARGET) $(MPCAT_TARGET)

sil: $(MPSIL_TARGET)

cat: $(MPCAT_TARGET)
$(MPSIL_TARGET): $(MPSIL_OBJECTS)
	$(CC) $(LFLAGS) -o $(MPSIL_TARGET) $(MPSIL_OBJECTS) $(LIBS)

$(MPCAT_TARGET): $(MPCAT_OBJECTS)
	$(CC) $(LFLAGS) -o $(MPCAT_TARGET) $(MPCAT_OBJECTS) $(LIBS)

clean:
	-$(RM) $(MPSIL_OBJECTS) $(MPCAT_OBJECTS)
	-$(RM) $(MPSIL_TARGET) $(MPCAT_TARGET)


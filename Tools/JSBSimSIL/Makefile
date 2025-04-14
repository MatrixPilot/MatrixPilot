#CONFIG = Cessna

ifeq ($(CONFIG),)
$(warning *******************************************************************************)
$(warning No configuration specified, defaulting to ./Config directory)
$(warning To specify an alternate, run make CONFIG='Config subdirectory')
$(warning eg. $ make CONFIG=Cessna)
$(warning *******************************************************************************)
endif

CONFIG  ?= .

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

DEFINES += -DJSB=1

CC       = gcc
CPP      = g++
WARN     = -Wall -Wextra -Wno-unused-parameter -Wno-unused-but-set-variable
CFLAGS   = -pipe -O0 $(DEFINES) $(WARN)
INCPATH  = -I. -I../../Config/$(CONFIG) -I../../Config -I../../libUDB -I../../libDCM -I../../MatrixPilot -I../MatrixPilot-SIL -I/usr/local/include/JSBSim
LFLAGS   =
LIBS     = -l$(OSLIB) -lc -lJSBSim

OBJECTS_DIR = ./


TARGET = matrixpilot$(BINARY)

HEADERS = \
../../Config/$(CONFIG)/options.h \

OBJECTS = \
JSBSim.o \
JSBSimSIL.o \
 \
../MatrixPilot-SIL/SIL-udb.o \
../MatrixPilot-SIL/SIL-ui-mp-term.o \
../MatrixPilot-SIL/SIL-serial.o \
../MatrixPilot-SIL/SIL-dsp.o \
../MatrixPilot-SIL/SIL-eeprom.o \
../MatrixPilot-SIL/SIL-events.o \
../MatrixPilot-SIL/$(OSOBJS) \
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
../../libDCM/mag_drift.o \
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
../../MatrixPilot/parameter_table2.o \
../../MatrixPilot/pitchCntrl.o \
../../MatrixPilot/rollCntrl.o \
../../MatrixPilot/servoMix.o \
../../MatrixPilot/servoPrepare.o \
../../MatrixPilot/states.o \
../../MatrixPilot/telemetry.o \
../../MatrixPilot/telemetry_log.o \
../../MatrixPilot/yawCntrl.o


first: all

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

%.o: %.cpp $(HEADERS)
	$(CPP) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CPP) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean:
	-$(RM) $(OBJECTS)
	-$(RM) $(TARGET)


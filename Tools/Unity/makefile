# ==========================================
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ========================================== 

C_COMPILER = gcc
TARGET_UDB = TestUDB
TARGET_DCM = TestDCM
TARGET_MPX = TestMPX

ifeq ($(OS),Windows_NT)
	TARGET_EXTENSION = .exe
	FLAGS = -DWIN
	LIBS = -lws2_32
else
	TARGET_EXTENSION = .out
	FLAGS = -DNIX
	LIBS =
endif
TEST_UDB = $(TARGET_UDB)$(TARGET_EXTENSION)
TEST_DCM = $(TARGET_DCM)$(TARGET_EXTENSION)
TEST_MPX = $(TARGET_MPX)$(TARGET_EXTENSION)
SYMBOLS = -DTEST -DUNITY_SUPPORT_64 $(FLAGS)

MP_HEADERS = \
../../MatrixPilot/options.h \
../../MatrixPilot/waypoints.h \
../../MatrixPilot/flightplan-logo.h

SRC_TST_FILES = externals.c

SRC_UDB_FILES = \
../MatrixPilot-SIL/SIL-udb.c \
../MatrixPilot-SIL/SIL-ui-mp-term.c \
../MatrixPilot-SIL/SIL-serial.c \
../MatrixPilot-SIL/SIL-dsp.c \
../MatrixPilot-SIL/SIL-eeprom.c \
../MatrixPilot-SIL/SIL-events.c \
../HILSIM_XPlane/UDBSocketWin.c \
$(SRC_TST_FILES)

SRC_DCM_FILES = \
../../libDCM/deadReckoning.c \
../../libDCM/estAltitude.c \
../../libDCM/estWind.c \
../../libDCM/estYawDrift.c \
../../libDCM/gpsParseCommon.c \
../../libDCM/gpsParseMTEK.c \
../../libDCM/gpsParseSTD.c \
../../libDCM/gpsParseUBX.c \
../../libDCM/libDCM.c \
../../libDCM/mathlibNAV.c \
../../libDCM/mathlib.c \
../../libDCM/rmat.c \
$(SRC_UDB_FILES)

SRC_MPX_FILES = \
../../MatrixPilot/airspeedCntrl.c \
../../MatrixPilot/altitudeCntrl.c \
../../MatrixPilot/altitudeCntrlVariable.c \
../../MatrixPilot/behavior.c \
../../MatrixPilot/cameraCntrl.c \
../../MatrixPilot/config.c \
../../MatrixPilot/config_tests.c \
../../MatrixPilot/data_services.c \
../../MatrixPilot/data_storage.c \
../../MatrixPilot/euler_angles.c \
../../MatrixPilot/flightplan-logo.c \
../../MatrixPilot/flightplan-waypoints.c \
../../MatrixPilot/MAVLink.c \
../../MatrixPilot/MAVFlexiFunctions.c \
../../MatrixPilot/MAVMission.c \
../../MatrixPilot/MAVParams.c \
../../MatrixPilot/MAVUDBExtra.c \
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
../../MatrixPilot/yawCntrl.c \
$(SRC_DCM_FILES)

TEST_MPX_FILES = \
	TestMPX.c \
	build/TestMPX_Runner.c \
	src/unity.c

TEST_UDB_FILES = \
	TestUDB.c \
	build/TestUDB_Runner.c \
	src/unity.c

TEST_DCM_FILES = \
	TestDCM.c \
	build/TestDCM_Runner.c \
	src/unity.c

INC_DIRS = \
	-Isrc \
	-I../../Config \
	-I../../libUDB \
	-I../../libDCM \
	-I../../MatrixPilot \
	-I../HILSIM_XPlane \
	-I../MatrixPilot-SIL

ifeq ($(OSTYPE),cygwin)
	CLEANUP = rm -f build/*.o ; rm -f $(TEST_UDB) ; rm -f $(TEST_DCM) ; rm -f $(TEST_MPX) ; mkdir -p build
else ifeq ($(OS),Windows_NT)
	CLEANUP = del /F /Q build\* && del /F /Q $(TEST_UDB) $(TEST_DCM) $(TEST_MPX)
else
	CLEANUP = rm -f build/*.o ; rm -f $(TEST_UDB)  ; rm -f $(TEST_DCM)  ; rm -f $(TEST_MPX) ; mkdir -p build
endif

#%.o: %.c $(MP_HEADERS)
#	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: clean default

ubd:
	ruby auto/generate_test_runner.rb TestUDB.c build/TestUDB_Runner.c
	$(C_COMPILER) $(INC_DIRS) $(SYMBOLS) $(TEST_UDB_FILES) $(SRC_UDB_FILES) mpx_dummy.c dcm_dummy.c $(LIBS) -o $(TEST_UDB)
	./$(TEST_UDB)

dcm:
	ruby auto/generate_test_runner.rb TestDCM.c build/TestDCM_Runner.c
	$(C_COMPILER) $(INC_DIRS) $(SYMBOLS) $(TEST_DCM_FILES) $(SRC_DCM_FILES) mpx_dummy.c $(LIBS) -o $(TEST_DCM)
	./$(TEST_DCM)

mpx:
	ruby auto/generate_test_runner.rb TestMPX.c build/TestMPX_Runner.c
	$(C_COMPILER) $(INC_DIRS) $(SYMBOLS) $(TEST_MPX_FILES) $(SRC_MPX_FILES) $(LIBS) -o $(TEST_MPX)
	./$(TEST_MPX)

default:
	ruby auto/generate_test_runner.rb TestUDB.c build/TestUDB_Runner.c
	ruby auto/generate_test_runner.rb TestDCM.c build/TestDCM_Runner.c
	ruby auto/generate_test_runner.rb TestMPX.c build/TestMPX_Runner.c
	$(C_COMPILER) $(INC_DIRS) $(SYMBOLS) $(TEST_UDB_FILES) $(SRC_UDB_FILES) mpx_dummy.c dcm_dummy.c $(LIBS) -o $(TEST_UDB)
	$(C_COMPILER) $(INC_DIRS) $(SYMBOLS) $(TEST_DCM_FILES) $(SRC_DCM_FILES) mpx_dummy.c $(LIBS) -o $(TEST_DCM)
	$(C_COMPILER) $(INC_DIRS) $(SYMBOLS) $(TEST_MPX_FILES) $(SRC_MPX_FILES) $(LIBS) -o $(TEST_MPX)
	./$(TEST_UDB)
	./$(TEST_DCM)
	./$(TEST_MPX)

clean:
	$(CLEANUP)


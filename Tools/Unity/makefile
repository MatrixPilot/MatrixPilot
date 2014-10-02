# ==========================================
#   MatrixPilot Unit Test Framework makefile
#   created by Robert Dickenson, Feb 2014
#
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ========================================== 

Q := $(if $(V),,@)

CC := gcc
RUBY_GEN := ruby auto/generate_test_runner.rb

TARGET_UDB := TestUDB
TARGET_DCM := TestDCM
TARGET_MPX := TestMPX

ifeq ($(OS),Windows_NT)
	TARGET_EXTENSION := .exe
	FLAGS := -DWIN=1
	LIBS := -lws2_32
else
	TARGET_EXTENSION := .out
	FLAGS := -DNIX
	LIBS :=
endif
TEST_UDB := $(TARGET_UDB)$(TARGET_EXTENSION)
TEST_DCM := $(TARGET_DCM)$(TARGET_EXTENSION)
TEST_MPX := $(TARGET_MPX)$(TARGET_EXTENSION)
SYMBOLS := -DTEST -DUNITY_SUPPORT_64 $(FLAGS)

MP_HEADERS = \
../../Config/options.h \
../../MatrixPilot/waypoints.h \
../../MatrixPilot/flightplan-logo.h

SRC_TST_FILES = externals.c

SRC_UDB_FILES = \
../MatrixPilot-SIL/SIL-udb.c \
../MatrixPilot-SIL/SIL-serial.c \
../MatrixPilot-SIL/SIL-dsp.c \
../MatrixPilot-SIL/SIL-eeprom.c \
../MatrixPilot-SIL/SIL-events.c \
../HILSIM_XPlane/UDBSocketWin.c \
$(SRC_TST_FILES)

SRC_DCM_FILES = \
../../libDCM/deadReckoning.c \
../../libDCM/estLocation.c \
../../libDCM/estAltitude.c \
../../libDCM/estWind.c \
../../libDCM/estYawDrift.c \
../../libDCM/gpsParseCommon.c \
../../libDCM/gpsParseMTEK.c \
../../libDCM/gpsParseSTD.c \
../../libDCM/gpsParseUBX.c \
../../libDCM/gpsData.c \
../../libDCM/libDCM.c \
../../libDCM/hilsim.c \
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
../../MatrixPilot/euler_angles.c \
../../MatrixPilot/flightplan-logo.c \
../../MatrixPilot/flightplan-waypoints.c \
../../MatrixPilot/MAVLink.c \
../../MatrixPilot/MAVFlexiFunctions.c \
../../MatrixPilot/MAVMission.c \
../../MatrixPilot/MAVParams.c \
../../MatrixPilot/MAVUDBExtra.c \
../../MAVLink/MAVFTP.c \
../../MatrixPilot/mode_switch.c \
../../MatrixPilot/mp_osd.c \
../../MatrixPilot/navigate.c \
../../MatrixPilot/parameter_table.c \
../../MatrixPilot/parameter_table2.c \
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

TEST_UDB4_FILES = \
	TestUDB4.c \
	build/TestUDB4_Runner.c \
	src/unity.c

INC_DIRS = \
	-Isrc \
	-I../../Config \
	-I../MatrixPilot-SIL

ifeq ($(OSTYPE),cygwin)
	CLEANUP = rm -f build/*.o ; rm -f $(TEST_UDB) ; rm -f $(TEST_DCM) ; rm -f $(TEST_MPX) ; mkdir -p build
else ifeq ($(OS),Windows_NT)
	CLEANUP = del /F /Q build\* && del /F /Q $(TEST_UDB) $(TEST_DCM) $(TEST_MPX)
else
	CLEANUP = rm -f build/*.o ; rm -f $(TEST_UDB)  ; rm -f $(TEST_DCM)  ; rm -f $(TEST_MPX) ; mkdir -p build
endif

all: clean default

udb:
	$(Q) $(RUBY_GEN) TestUDB.c build/TestUDB_Runner.c
	$(Q) $(CC) $(INC_DIRS) $(SYMBOLS) $(TEST_UDB_FILES) $(SRC_UDB_FILES) mpx_dummy.c dcm_dummy.c $(LIBS) -o $(TEST_UDB)
	./$(TEST_UDB)

dcm:
	$(Q) $(RUBY_GEN) TestDCM.c build/TestDCM_Runner.c
	$(Q) $(CC) $(INC_DIRS) $(SYMBOLS) $(TEST_DCM_FILES) $(SRC_DCM_FILES) mpx_dummy.c $(LIBS) -o $(TEST_DCM)
	./$(TEST_DCM)

mpx:
	$(Q) $(RUBY_GEN) TestMPX.c build/TestMPX_Runner.c
	$(Q) $(CC) $(INC_DIRS) $(SYMBOLS) $(TEST_MPX_FILES) $(SRC_MPX_FILES) $(LIBS) -o $(TEST_MPX)
	./$(TEST_MPX)

default: udb dcm mpx

clean:
	$(Q) $(CLEANUP)

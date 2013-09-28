#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Polaris-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Polaris-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../libDCM/estWind.c ../../libDCM/estYawDrift.c ../../libDCM/gpsParseCommon.c ../../libDCM/gpsParseSTD.c ../../libDCM/gpsParseUBX.c ../../libDCM/mathlibNAV.c ../../libDCM/rmat.c ../../libDCM/libDCM.c ../../libDCM/deadReckoning.c ../../libDCM/gpsParseMTEK.c ../../libDCM/estAltitude.c ../../libUDB/background.c ../../libUDB/libUDB.c ../../libUDB/analog2digital_udb4.c ../../libUDB/servoOut.c ../../libUDB/osd.c ../../libUDB/24LC256.c ../../libUDB/events.c ../../libUDB/analog2digital_auav3.c ../../libUDB/analog2digital_udb5.c ../../libUDB/mpu6000.c ../../libUDB/spiUtils.c ../../libUDB/traps.c ../../libUDB/traps_asm.s ../../libUDB/mcu.c ../../libUDB/barometer.c ../../libUDB/magnetometer.c ../../libUDB/radioIn.c ../../libUDB/serialIO.c ../../libUDB/I2C1.c ../../libUDB/I2C2.c ../../libUDB/read.c ../../libUDB/uart.c ../../libUDB/write.c ../config_tests.c ../behavior.c ../cameraCntrl.c ../main.c ../navigate.c ../pitchCntrl.c ../rollCntrl.c ../servoMix.c ../states.c ../yawCntrl.c ../telemetry.c ../servoPrepare.c ../flightplan-waypoints.c ../flightplan-logo.c ../mp_osd.c ../altitudeCntrl.c ../MAVLink.c ../mode_switch.c ../data_services.c ../data_storage.c ../nv_memory_table.c ../parameter_table.c ../airspeedCntrl.c ../altitudeCntrlVariable.c ../euler_angles.c ../fly_by_datalink.c ../console.c ../stack.S ../telemetry_log.c ../config.c ../preflight.c ../ring_buffer.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1077471063/estWind.o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o ${OBJECTDIR}/_ext/1077471063/rmat.o ${OBJECTDIR}/_ext/1077471063/libDCM.o ${OBJECTDIR}/_ext/1077471063/deadReckoning.o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o ${OBJECTDIR}/_ext/1077471063/estAltitude.o ${OBJECTDIR}/_ext/1077454706/background.o ${OBJECTDIR}/_ext/1077454706/libUDB.o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o ${OBJECTDIR}/_ext/1077454706/servoOut.o ${OBJECTDIR}/_ext/1077454706/osd.o ${OBJECTDIR}/_ext/1077454706/24LC256.o ${OBJECTDIR}/_ext/1077454706/events.o ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o ${OBJECTDIR}/_ext/1077454706/mpu6000.o ${OBJECTDIR}/_ext/1077454706/spiUtils.o ${OBJECTDIR}/_ext/1077454706/traps.o ${OBJECTDIR}/_ext/1077454706/traps_asm.o ${OBJECTDIR}/_ext/1077454706/mcu.o ${OBJECTDIR}/_ext/1077454706/barometer.o ${OBJECTDIR}/_ext/1077454706/magnetometer.o ${OBJECTDIR}/_ext/1077454706/radioIn.o ${OBJECTDIR}/_ext/1077454706/serialIO.o ${OBJECTDIR}/_ext/1077454706/I2C1.o ${OBJECTDIR}/_ext/1077454706/I2C2.o ${OBJECTDIR}/_ext/1077454706/read.o ${OBJECTDIR}/_ext/1077454706/uart.o ${OBJECTDIR}/_ext/1077454706/write.o ${OBJECTDIR}/_ext/1472/config_tests.o ${OBJECTDIR}/_ext/1472/behavior.o ${OBJECTDIR}/_ext/1472/cameraCntrl.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/navigate.o ${OBJECTDIR}/_ext/1472/pitchCntrl.o ${OBJECTDIR}/_ext/1472/rollCntrl.o ${OBJECTDIR}/_ext/1472/servoMix.o ${OBJECTDIR}/_ext/1472/states.o ${OBJECTDIR}/_ext/1472/yawCntrl.o ${OBJECTDIR}/_ext/1472/telemetry.o ${OBJECTDIR}/_ext/1472/servoPrepare.o ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o ${OBJECTDIR}/_ext/1472/flightplan-logo.o ${OBJECTDIR}/_ext/1472/mp_osd.o ${OBJECTDIR}/_ext/1472/altitudeCntrl.o ${OBJECTDIR}/_ext/1472/MAVLink.o ${OBJECTDIR}/_ext/1472/mode_switch.o ${OBJECTDIR}/_ext/1472/data_services.o ${OBJECTDIR}/_ext/1472/data_storage.o ${OBJECTDIR}/_ext/1472/nv_memory_table.o ${OBJECTDIR}/_ext/1472/parameter_table.o ${OBJECTDIR}/_ext/1472/airspeedCntrl.o ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o ${OBJECTDIR}/_ext/1472/euler_angles.o ${OBJECTDIR}/_ext/1472/fly_by_datalink.o ${OBJECTDIR}/_ext/1472/console.o ${OBJECTDIR}/_ext/1472/stack.o ${OBJECTDIR}/_ext/1472/telemetry_log.o ${OBJECTDIR}/_ext/1472/config.o ${OBJECTDIR}/_ext/1472/preflight.o ${OBJECTDIR}/_ext/1472/ring_buffer.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1077471063/estWind.o.d ${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d ${OBJECTDIR}/_ext/1077471063/rmat.o.d ${OBJECTDIR}/_ext/1077471063/libDCM.o.d ${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d ${OBJECTDIR}/_ext/1077471063/estAltitude.o.d ${OBJECTDIR}/_ext/1077454706/background.o.d ${OBJECTDIR}/_ext/1077454706/libUDB.o.d ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d ${OBJECTDIR}/_ext/1077454706/servoOut.o.d ${OBJECTDIR}/_ext/1077454706/osd.o.d ${OBJECTDIR}/_ext/1077454706/24LC256.o.d ${OBJECTDIR}/_ext/1077454706/events.o.d ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d ${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o.d ${OBJECTDIR}/_ext/1077454706/mpu6000.o.d ${OBJECTDIR}/_ext/1077454706/spiUtils.o.d ${OBJECTDIR}/_ext/1077454706/traps.o.d ${OBJECTDIR}/_ext/1077454706/traps_asm.o.d ${OBJECTDIR}/_ext/1077454706/mcu.o.d ${OBJECTDIR}/_ext/1077454706/barometer.o.d ${OBJECTDIR}/_ext/1077454706/magnetometer.o.d ${OBJECTDIR}/_ext/1077454706/radioIn.o.d ${OBJECTDIR}/_ext/1077454706/serialIO.o.d ${OBJECTDIR}/_ext/1077454706/I2C1.o.d ${OBJECTDIR}/_ext/1077454706/I2C2.o.d ${OBJECTDIR}/_ext/1077454706/read.o.d ${OBJECTDIR}/_ext/1077454706/uart.o.d ${OBJECTDIR}/_ext/1077454706/write.o.d ${OBJECTDIR}/_ext/1472/config_tests.o.d ${OBJECTDIR}/_ext/1472/behavior.o.d ${OBJECTDIR}/_ext/1472/cameraCntrl.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1472/navigate.o.d ${OBJECTDIR}/_ext/1472/pitchCntrl.o.d ${OBJECTDIR}/_ext/1472/rollCntrl.o.d ${OBJECTDIR}/_ext/1472/servoMix.o.d ${OBJECTDIR}/_ext/1472/states.o.d ${OBJECTDIR}/_ext/1472/yawCntrl.o.d ${OBJECTDIR}/_ext/1472/telemetry.o.d ${OBJECTDIR}/_ext/1472/servoPrepare.o.d ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d ${OBJECTDIR}/_ext/1472/flightplan-logo.o.d ${OBJECTDIR}/_ext/1472/mp_osd.o.d ${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d ${OBJECTDIR}/_ext/1472/MAVLink.o.d ${OBJECTDIR}/_ext/1472/mode_switch.o.d ${OBJECTDIR}/_ext/1472/data_services.o.d ${OBJECTDIR}/_ext/1472/data_storage.o.d ${OBJECTDIR}/_ext/1472/nv_memory_table.o.d ${OBJECTDIR}/_ext/1472/parameter_table.o.d ${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d ${OBJECTDIR}/_ext/1472/euler_angles.o.d ${OBJECTDIR}/_ext/1472/fly_by_datalink.o.d ${OBJECTDIR}/_ext/1472/console.o.d ${OBJECTDIR}/_ext/1472/stack.o.d ${OBJECTDIR}/_ext/1472/telemetry_log.o.d ${OBJECTDIR}/_ext/1472/config.o.d ${OBJECTDIR}/_ext/1472/preflight.o.d ${OBJECTDIR}/_ext/1472/ring_buffer.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1077471063/estWind.o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o ${OBJECTDIR}/_ext/1077471063/rmat.o ${OBJECTDIR}/_ext/1077471063/libDCM.o ${OBJECTDIR}/_ext/1077471063/deadReckoning.o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o ${OBJECTDIR}/_ext/1077471063/estAltitude.o ${OBJECTDIR}/_ext/1077454706/background.o ${OBJECTDIR}/_ext/1077454706/libUDB.o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o ${OBJECTDIR}/_ext/1077454706/servoOut.o ${OBJECTDIR}/_ext/1077454706/osd.o ${OBJECTDIR}/_ext/1077454706/24LC256.o ${OBJECTDIR}/_ext/1077454706/events.o ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o ${OBJECTDIR}/_ext/1077454706/mpu6000.o ${OBJECTDIR}/_ext/1077454706/spiUtils.o ${OBJECTDIR}/_ext/1077454706/traps.o ${OBJECTDIR}/_ext/1077454706/traps_asm.o ${OBJECTDIR}/_ext/1077454706/mcu.o ${OBJECTDIR}/_ext/1077454706/barometer.o ${OBJECTDIR}/_ext/1077454706/magnetometer.o ${OBJECTDIR}/_ext/1077454706/radioIn.o ${OBJECTDIR}/_ext/1077454706/serialIO.o ${OBJECTDIR}/_ext/1077454706/I2C1.o ${OBJECTDIR}/_ext/1077454706/I2C2.o ${OBJECTDIR}/_ext/1077454706/read.o ${OBJECTDIR}/_ext/1077454706/uart.o ${OBJECTDIR}/_ext/1077454706/write.o ${OBJECTDIR}/_ext/1472/config_tests.o ${OBJECTDIR}/_ext/1472/behavior.o ${OBJECTDIR}/_ext/1472/cameraCntrl.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/navigate.o ${OBJECTDIR}/_ext/1472/pitchCntrl.o ${OBJECTDIR}/_ext/1472/rollCntrl.o ${OBJECTDIR}/_ext/1472/servoMix.o ${OBJECTDIR}/_ext/1472/states.o ${OBJECTDIR}/_ext/1472/yawCntrl.o ${OBJECTDIR}/_ext/1472/telemetry.o ${OBJECTDIR}/_ext/1472/servoPrepare.o ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o ${OBJECTDIR}/_ext/1472/flightplan-logo.o ${OBJECTDIR}/_ext/1472/mp_osd.o ${OBJECTDIR}/_ext/1472/altitudeCntrl.o ${OBJECTDIR}/_ext/1472/MAVLink.o ${OBJECTDIR}/_ext/1472/mode_switch.o ${OBJECTDIR}/_ext/1472/data_services.o ${OBJECTDIR}/_ext/1472/data_storage.o ${OBJECTDIR}/_ext/1472/nv_memory_table.o ${OBJECTDIR}/_ext/1472/parameter_table.o ${OBJECTDIR}/_ext/1472/airspeedCntrl.o ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o ${OBJECTDIR}/_ext/1472/euler_angles.o ${OBJECTDIR}/_ext/1472/fly_by_datalink.o ${OBJECTDIR}/_ext/1472/console.o ${OBJECTDIR}/_ext/1472/stack.o ${OBJECTDIR}/_ext/1472/telemetry_log.o ${OBJECTDIR}/_ext/1472/config.o ${OBJECTDIR}/_ext/1472/preflight.o ${OBJECTDIR}/_ext/1472/ring_buffer.o

# Source Files
SOURCEFILES=../../libDCM/estWind.c ../../libDCM/estYawDrift.c ../../libDCM/gpsParseCommon.c ../../libDCM/gpsParseSTD.c ../../libDCM/gpsParseUBX.c ../../libDCM/mathlibNAV.c ../../libDCM/rmat.c ../../libDCM/libDCM.c ../../libDCM/deadReckoning.c ../../libDCM/gpsParseMTEK.c ../../libDCM/estAltitude.c ../../libUDB/background.c ../../libUDB/libUDB.c ../../libUDB/analog2digital_udb4.c ../../libUDB/servoOut.c ../../libUDB/osd.c ../../libUDB/24LC256.c ../../libUDB/events.c ../../libUDB/analog2digital_auav3.c ../../libUDB/analog2digital_udb5.c ../../libUDB/mpu6000.c ../../libUDB/spiUtils.c ../../libUDB/traps.c ../../libUDB/traps_asm.s ../../libUDB/mcu.c ../../libUDB/barometer.c ../../libUDB/magnetometer.c ../../libUDB/radioIn.c ../../libUDB/serialIO.c ../../libUDB/I2C1.c ../../libUDB/I2C2.c ../../libUDB/read.c ../../libUDB/uart.c ../../libUDB/write.c ../config_tests.c ../behavior.c ../cameraCntrl.c ../main.c ../navigate.c ../pitchCntrl.c ../rollCntrl.c ../servoMix.c ../states.c ../yawCntrl.c ../telemetry.c ../servoPrepare.c ../flightplan-waypoints.c ../flightplan-logo.c ../mp_osd.c ../altitudeCntrl.c ../MAVLink.c ../mode_switch.c ../data_services.c ../data_storage.c ../nv_memory_table.c ../parameter_table.c ../airspeedCntrl.c ../altitudeCntrlVariable.c ../euler_angles.c ../fly_by_datalink.c ../console.c ../stack.S ../telemetry_log.c ../config.c ../preflight.c ../ring_buffer.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

# The following macros may be used in the pre and post step lines
Device=dsPIC33EP512MU810
ProjectDir=/home/markw/MPLABXProjects/MatrixPilot_mw4/MatrixPilot/Polaris-auav3
ConfName=default
ImagePath=dist/default/${IMAGE_TYPE}/Polaris-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
ImageDir=dist/default/${IMAGE_TYPE}
ImageName=Polaris-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

.build-conf:  .pre ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Polaris-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP512MU810
MP_LINKER_FILE_OPTION=,--script=p33EP512MU810.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1077471063/estWind.o: ../../libDCM/estWind.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estWind.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estWind.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estWind.c  -o ${OBJECTDIR}/_ext/1077471063/estWind.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estWind.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estWind.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estYawDrift.o: ../../libDCM/estYawDrift.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estYawDrift.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estYawDrift.c  -o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o: ../../libDCM/gpsParseCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseCommon.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o: ../../libDCM/gpsParseSTD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseSTD.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o: ../../libDCM/gpsParseUBX.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseUBX.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/mathlibNAV.o: ../../libDCM/mathlibNAV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/mathlibNAV.c  -o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/rmat.o: ../../libDCM/rmat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/rmat.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/rmat.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/rmat.c  -o ${OBJECTDIR}/_ext/1077471063/rmat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/rmat.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/rmat.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/libDCM.o: ../../libDCM/libDCM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/libDCM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/libDCM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/libDCM.c  -o ${OBJECTDIR}/_ext/1077471063/libDCM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/libDCM.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/libDCM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/deadReckoning.o: ../../libDCM/deadReckoning.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/deadReckoning.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/deadReckoning.c  -o ${OBJECTDIR}/_ext/1077471063/deadReckoning.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o: ../../libDCM/gpsParseMTEK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseMTEK.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estAltitude.o: ../../libDCM/estAltitude.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estAltitude.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estAltitude.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estAltitude.c  -o ${OBJECTDIR}/_ext/1077471063/estAltitude.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estAltitude.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estAltitude.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/background.o: ../../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/background.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/background.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/background.c  -o ${OBJECTDIR}/_ext/1077454706/background.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/background.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/background.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/libUDB.o: ../../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/libUDB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/libUDB.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/libUDB.c  -o ${OBJECTDIR}/_ext/1077454706/libUDB.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/libUDB.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/libUDB.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o: ../../libUDB/analog2digital_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/servoOut.o: ../../libUDB/servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/servoOut.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/servoOut.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/servoOut.c  -o ${OBJECTDIR}/_ext/1077454706/servoOut.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/servoOut.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/servoOut.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/osd.o: ../../libUDB/osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/osd.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/osd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/osd.c  -o ${OBJECTDIR}/_ext/1077454706/osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/osd.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/24LC256.o: ../../libUDB/24LC256.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/24LC256.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/24LC256.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/24LC256.c  -o ${OBJECTDIR}/_ext/1077454706/24LC256.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/24LC256.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/24LC256.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/events.o: ../../libUDB/events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/events.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/events.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/events.c  -o ${OBJECTDIR}/_ext/1077454706/events.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/events.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/events.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o: ../../libUDB/analog2digital_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_auav3.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o: ../../libUDB/analog2digital_udb5.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_udb5.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/mpu6000.o: ../../libUDB/mpu6000.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mpu6000.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mpu6000.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/mpu6000.c  -o ${OBJECTDIR}/_ext/1077454706/mpu6000.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/mpu6000.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/mpu6000.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/spiUtils.o: ../../libUDB/spiUtils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/spiUtils.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/spiUtils.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/spiUtils.c  -o ${OBJECTDIR}/_ext/1077454706/spiUtils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/spiUtils.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/spiUtils.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/traps.o: ../../libUDB/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/traps.c  -o ${OBJECTDIR}/_ext/1077454706/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/traps.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/mcu.o: ../../libUDB/mcu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mcu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mcu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/mcu.c  -o ${OBJECTDIR}/_ext/1077454706/mcu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/mcu.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/mcu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/barometer.o: ../../libUDB/barometer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/barometer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/barometer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/barometer.c  -o ${OBJECTDIR}/_ext/1077454706/barometer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/barometer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/barometer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/magnetometer.o: ../../libUDB/magnetometer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/magnetometer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/magnetometer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/magnetometer.c  -o ${OBJECTDIR}/_ext/1077454706/magnetometer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/magnetometer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/magnetometer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/radioIn.o: ../../libUDB/radioIn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/radioIn.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/radioIn.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/radioIn.c  -o ${OBJECTDIR}/_ext/1077454706/radioIn.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/radioIn.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/radioIn.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/serialIO.o: ../../libUDB/serialIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/serialIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/serialIO.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/serialIO.c  -o ${OBJECTDIR}/_ext/1077454706/serialIO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/serialIO.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/serialIO.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/I2C1.o: ../../libUDB/I2C1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/I2C1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/I2C1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/I2C1.c  -o ${OBJECTDIR}/_ext/1077454706/I2C1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/I2C1.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/I2C1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/I2C2.o: ../../libUDB/I2C2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/I2C2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/I2C2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/I2C2.c  -o ${OBJECTDIR}/_ext/1077454706/I2C2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/I2C2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/I2C2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/read.o: ../../libUDB/read.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/read.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/read.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/read.c  -o ${OBJECTDIR}/_ext/1077454706/read.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/read.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/read.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/uart.o: ../../libUDB/uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/uart.c  -o ${OBJECTDIR}/_ext/1077454706/uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/uart.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/uart.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/write.o: ../../libUDB/write.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/write.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/write.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/write.c  -o ${OBJECTDIR}/_ext/1077454706/write.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/write.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/write.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/config_tests.o: ../config_tests.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/config_tests.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/config_tests.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../config_tests.c  -o ${OBJECTDIR}/_ext/1472/config_tests.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/config_tests.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/config_tests.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/behavior.o: ../behavior.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/behavior.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/behavior.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../behavior.c  -o ${OBJECTDIR}/_ext/1472/behavior.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/behavior.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/behavior.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/cameraCntrl.o: ../cameraCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/cameraCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/cameraCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../cameraCntrl.c  -o ${OBJECTDIR}/_ext/1472/cameraCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/cameraCntrl.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/cameraCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main.c  -o ${OBJECTDIR}/_ext/1472/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/navigate.o: ../navigate.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/navigate.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/navigate.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../navigate.c  -o ${OBJECTDIR}/_ext/1472/navigate.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/navigate.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/navigate.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/pitchCntrl.o: ../pitchCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/pitchCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/pitchCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../pitchCntrl.c  -o ${OBJECTDIR}/_ext/1472/pitchCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/pitchCntrl.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pitchCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/rollCntrl.o: ../rollCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/rollCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/rollCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../rollCntrl.c  -o ${OBJECTDIR}/_ext/1472/rollCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/rollCntrl.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/rollCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/servoMix.o: ../servoMix.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/servoMix.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/servoMix.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../servoMix.c  -o ${OBJECTDIR}/_ext/1472/servoMix.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/servoMix.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/servoMix.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/states.o: ../states.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/states.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/states.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../states.c  -o ${OBJECTDIR}/_ext/1472/states.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/states.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/states.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/yawCntrl.o: ../yawCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/yawCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/yawCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../yawCntrl.c  -o ${OBJECTDIR}/_ext/1472/yawCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/yawCntrl.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/yawCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/telemetry.o: ../telemetry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../telemetry.c  -o ${OBJECTDIR}/_ext/1472/telemetry.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/telemetry.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/telemetry.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/servoPrepare.o: ../servoPrepare.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/servoPrepare.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/servoPrepare.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../servoPrepare.c  -o ${OBJECTDIR}/_ext/1472/servoPrepare.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/servoPrepare.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/servoPrepare.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/flightplan-waypoints.o: ../flightplan-waypoints.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../flightplan-waypoints.c  -o ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/flightplan-logo.o: ../flightplan-logo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-logo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-logo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../flightplan-logo.c  -o ${OBJECTDIR}/_ext/1472/flightplan-logo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/flightplan-logo.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/flightplan-logo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mp_osd.o: ../mp_osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mp_osd.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mp_osd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mp_osd.c  -o ${OBJECTDIR}/_ext/1472/mp_osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mp_osd.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mp_osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/altitudeCntrl.o: ../altitudeCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../altitudeCntrl.c  -o ${OBJECTDIR}/_ext/1472/altitudeCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/MAVLink.o: ../MAVLink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/MAVLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/MAVLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../MAVLink.c  -o ${OBJECTDIR}/_ext/1472/MAVLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/MAVLink.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/MAVLink.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mode_switch.o: ../mode_switch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mode_switch.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mode_switch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mode_switch.c  -o ${OBJECTDIR}/_ext/1472/mode_switch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mode_switch.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mode_switch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/data_services.o: ../data_services.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/data_services.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/data_services.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../data_services.c  -o ${OBJECTDIR}/_ext/1472/data_services.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/data_services.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/data_services.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/data_storage.o: ../data_storage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/data_storage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/data_storage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../data_storage.c  -o ${OBJECTDIR}/_ext/1472/data_storage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/data_storage.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/data_storage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/nv_memory_table.o: ../nv_memory_table.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/nv_memory_table.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/nv_memory_table.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../nv_memory_table.c  -o ${OBJECTDIR}/_ext/1472/nv_memory_table.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/nv_memory_table.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/nv_memory_table.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/parameter_table.o: ../parameter_table.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/parameter_table.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/parameter_table.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../parameter_table.c  -o ${OBJECTDIR}/_ext/1472/parameter_table.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/parameter_table.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/parameter_table.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/airspeedCntrl.o: ../airspeedCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/airspeedCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../airspeedCntrl.c  -o ${OBJECTDIR}/_ext/1472/airspeedCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o: ../altitudeCntrlVariable.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../altitudeCntrlVariable.c  -o ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/euler_angles.o: ../euler_angles.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/euler_angles.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/euler_angles.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../euler_angles.c  -o ${OBJECTDIR}/_ext/1472/euler_angles.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/euler_angles.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/euler_angles.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/fly_by_datalink.o: ../fly_by_datalink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/fly_by_datalink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/fly_by_datalink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../fly_by_datalink.c  -o ${OBJECTDIR}/_ext/1472/fly_by_datalink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/fly_by_datalink.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/fly_by_datalink.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/console.o: ../console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/console.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/console.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../console.c  -o ${OBJECTDIR}/_ext/1472/console.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/console.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/console.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/telemetry_log.o: ../telemetry_log.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry_log.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry_log.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../telemetry_log.c  -o ${OBJECTDIR}/_ext/1472/telemetry_log.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/telemetry_log.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/telemetry_log.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/config.o: ../config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/config.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/config.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../config.c  -o ${OBJECTDIR}/_ext/1472/config.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/config.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/config.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/preflight.o: ../preflight.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/preflight.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/preflight.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../preflight.c  -o ${OBJECTDIR}/_ext/1472/preflight.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/preflight.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/preflight.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/ring_buffer.o: ../ring_buffer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/ring_buffer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ring_buffer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../ring_buffer.c  -o ${OBJECTDIR}/_ext/1472/ring_buffer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/ring_buffer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ring_buffer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1077471063/estWind.o: ../../libDCM/estWind.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estWind.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estWind.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estWind.c  -o ${OBJECTDIR}/_ext/1077471063/estWind.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estWind.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estWind.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estYawDrift.o: ../../libDCM/estYawDrift.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estYawDrift.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estYawDrift.c  -o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o: ../../libDCM/gpsParseCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseCommon.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o: ../../libDCM/gpsParseSTD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseSTD.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o: ../../libDCM/gpsParseUBX.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseUBX.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/mathlibNAV.o: ../../libDCM/mathlibNAV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/mathlibNAV.c  -o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/rmat.o: ../../libDCM/rmat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/rmat.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/rmat.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/rmat.c  -o ${OBJECTDIR}/_ext/1077471063/rmat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/rmat.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/rmat.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/libDCM.o: ../../libDCM/libDCM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/libDCM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/libDCM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/libDCM.c  -o ${OBJECTDIR}/_ext/1077471063/libDCM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/libDCM.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/libDCM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/deadReckoning.o: ../../libDCM/deadReckoning.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/deadReckoning.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/deadReckoning.c  -o ${OBJECTDIR}/_ext/1077471063/deadReckoning.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o: ../../libDCM/gpsParseMTEK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseMTEK.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estAltitude.o: ../../libDCM/estAltitude.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estAltitude.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estAltitude.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estAltitude.c  -o ${OBJECTDIR}/_ext/1077471063/estAltitude.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estAltitude.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estAltitude.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/background.o: ../../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/background.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/background.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/background.c  -o ${OBJECTDIR}/_ext/1077454706/background.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/background.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/background.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/libUDB.o: ../../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/libUDB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/libUDB.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/libUDB.c  -o ${OBJECTDIR}/_ext/1077454706/libUDB.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/libUDB.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/libUDB.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o: ../../libUDB/analog2digital_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/servoOut.o: ../../libUDB/servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/servoOut.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/servoOut.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/servoOut.c  -o ${OBJECTDIR}/_ext/1077454706/servoOut.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/servoOut.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/servoOut.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/osd.o: ../../libUDB/osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/osd.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/osd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/osd.c  -o ${OBJECTDIR}/_ext/1077454706/osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/osd.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/24LC256.o: ../../libUDB/24LC256.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/24LC256.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/24LC256.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/24LC256.c  -o ${OBJECTDIR}/_ext/1077454706/24LC256.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/24LC256.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/24LC256.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/events.o: ../../libUDB/events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/events.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/events.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/events.c  -o ${OBJECTDIR}/_ext/1077454706/events.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/events.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/events.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o: ../../libUDB/analog2digital_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_auav3.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o: ../../libUDB/analog2digital_udb5.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_udb5.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_udb5.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/mpu6000.o: ../../libUDB/mpu6000.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mpu6000.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mpu6000.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/mpu6000.c  -o ${OBJECTDIR}/_ext/1077454706/mpu6000.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/mpu6000.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/mpu6000.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/spiUtils.o: ../../libUDB/spiUtils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/spiUtils.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/spiUtils.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/spiUtils.c  -o ${OBJECTDIR}/_ext/1077454706/spiUtils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/spiUtils.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/spiUtils.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/traps.o: ../../libUDB/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/traps.c  -o ${OBJECTDIR}/_ext/1077454706/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/traps.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/mcu.o: ../../libUDB/mcu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mcu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mcu.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/mcu.c  -o ${OBJECTDIR}/_ext/1077454706/mcu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/mcu.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/mcu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/barometer.o: ../../libUDB/barometer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/barometer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/barometer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/barometer.c  -o ${OBJECTDIR}/_ext/1077454706/barometer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/barometer.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/barometer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/magnetometer.o: ../../libUDB/magnetometer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/magnetometer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/magnetometer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/magnetometer.c  -o ${OBJECTDIR}/_ext/1077454706/magnetometer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/magnetometer.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/magnetometer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/radioIn.o: ../../libUDB/radioIn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/radioIn.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/radioIn.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/radioIn.c  -o ${OBJECTDIR}/_ext/1077454706/radioIn.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/radioIn.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/radioIn.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/serialIO.o: ../../libUDB/serialIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/serialIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/serialIO.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/serialIO.c  -o ${OBJECTDIR}/_ext/1077454706/serialIO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/serialIO.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/serialIO.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/I2C1.o: ../../libUDB/I2C1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/I2C1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/I2C1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/I2C1.c  -o ${OBJECTDIR}/_ext/1077454706/I2C1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/I2C1.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/I2C1.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/I2C2.o: ../../libUDB/I2C2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/I2C2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/I2C2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/I2C2.c  -o ${OBJECTDIR}/_ext/1077454706/I2C2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/I2C2.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/I2C2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/read.o: ../../libUDB/read.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/read.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/read.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/read.c  -o ${OBJECTDIR}/_ext/1077454706/read.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/read.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/read.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/uart.o: ../../libUDB/uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/uart.c  -o ${OBJECTDIR}/_ext/1077454706/uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/uart.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/uart.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/write.o: ../../libUDB/write.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/write.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/write.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/write.c  -o ${OBJECTDIR}/_ext/1077454706/write.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/write.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/write.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/config_tests.o: ../config_tests.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/config_tests.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/config_tests.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../config_tests.c  -o ${OBJECTDIR}/_ext/1472/config_tests.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/config_tests.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/config_tests.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/behavior.o: ../behavior.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/behavior.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/behavior.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../behavior.c  -o ${OBJECTDIR}/_ext/1472/behavior.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/behavior.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/behavior.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/cameraCntrl.o: ../cameraCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/cameraCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/cameraCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../cameraCntrl.c  -o ${OBJECTDIR}/_ext/1472/cameraCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/cameraCntrl.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/cameraCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main.c  -o ${OBJECTDIR}/_ext/1472/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/navigate.o: ../navigate.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/navigate.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/navigate.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../navigate.c  -o ${OBJECTDIR}/_ext/1472/navigate.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/navigate.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/navigate.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/pitchCntrl.o: ../pitchCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/pitchCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/pitchCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../pitchCntrl.c  -o ${OBJECTDIR}/_ext/1472/pitchCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/pitchCntrl.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pitchCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/rollCntrl.o: ../rollCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/rollCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/rollCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../rollCntrl.c  -o ${OBJECTDIR}/_ext/1472/rollCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/rollCntrl.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/rollCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/servoMix.o: ../servoMix.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/servoMix.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/servoMix.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../servoMix.c  -o ${OBJECTDIR}/_ext/1472/servoMix.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/servoMix.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/servoMix.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/states.o: ../states.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/states.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/states.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../states.c  -o ${OBJECTDIR}/_ext/1472/states.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/states.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/states.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/yawCntrl.o: ../yawCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/yawCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/yawCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../yawCntrl.c  -o ${OBJECTDIR}/_ext/1472/yawCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/yawCntrl.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/yawCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/telemetry.o: ../telemetry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../telemetry.c  -o ${OBJECTDIR}/_ext/1472/telemetry.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/telemetry.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/telemetry.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/servoPrepare.o: ../servoPrepare.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/servoPrepare.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/servoPrepare.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../servoPrepare.c  -o ${OBJECTDIR}/_ext/1472/servoPrepare.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/servoPrepare.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/servoPrepare.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/flightplan-waypoints.o: ../flightplan-waypoints.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../flightplan-waypoints.c  -o ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/flightplan-logo.o: ../flightplan-logo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-logo.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-logo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../flightplan-logo.c  -o ${OBJECTDIR}/_ext/1472/flightplan-logo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/flightplan-logo.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/flightplan-logo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mp_osd.o: ../mp_osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mp_osd.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mp_osd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mp_osd.c  -o ${OBJECTDIR}/_ext/1472/mp_osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mp_osd.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mp_osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/altitudeCntrl.o: ../altitudeCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../altitudeCntrl.c  -o ${OBJECTDIR}/_ext/1472/altitudeCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/MAVLink.o: ../MAVLink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/MAVLink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/MAVLink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../MAVLink.c  -o ${OBJECTDIR}/_ext/1472/MAVLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/MAVLink.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/MAVLink.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mode_switch.o: ../mode_switch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mode_switch.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/mode_switch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mode_switch.c  -o ${OBJECTDIR}/_ext/1472/mode_switch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mode_switch.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mode_switch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/data_services.o: ../data_services.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/data_services.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/data_services.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../data_services.c  -o ${OBJECTDIR}/_ext/1472/data_services.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/data_services.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/data_services.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/data_storage.o: ../data_storage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/data_storage.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/data_storage.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../data_storage.c  -o ${OBJECTDIR}/_ext/1472/data_storage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/data_storage.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/data_storage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/nv_memory_table.o: ../nv_memory_table.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/nv_memory_table.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/nv_memory_table.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../nv_memory_table.c  -o ${OBJECTDIR}/_ext/1472/nv_memory_table.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/nv_memory_table.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/nv_memory_table.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/parameter_table.o: ../parameter_table.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/parameter_table.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/parameter_table.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../parameter_table.c  -o ${OBJECTDIR}/_ext/1472/parameter_table.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/parameter_table.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/parameter_table.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/airspeedCntrl.o: ../airspeedCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/airspeedCntrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../airspeedCntrl.c  -o ${OBJECTDIR}/_ext/1472/airspeedCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o: ../altitudeCntrlVariable.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../altitudeCntrlVariable.c  -o ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/euler_angles.o: ../euler_angles.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/euler_angles.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/euler_angles.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../euler_angles.c  -o ${OBJECTDIR}/_ext/1472/euler_angles.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/euler_angles.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/euler_angles.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/fly_by_datalink.o: ../fly_by_datalink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/fly_by_datalink.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/fly_by_datalink.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../fly_by_datalink.c  -o ${OBJECTDIR}/_ext/1472/fly_by_datalink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/fly_by_datalink.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/fly_by_datalink.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/console.o: ../console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/console.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/console.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../console.c  -o ${OBJECTDIR}/_ext/1472/console.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/console.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/console.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/telemetry_log.o: ../telemetry_log.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry_log.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry_log.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../telemetry_log.c  -o ${OBJECTDIR}/_ext/1472/telemetry_log.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/telemetry_log.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/telemetry_log.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/config.o: ../config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/config.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/config.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../config.c  -o ${OBJECTDIR}/_ext/1472/config.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/config.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/config.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/preflight.o: ../preflight.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/preflight.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/preflight.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../preflight.c  -o ${OBJECTDIR}/_ext/1472/preflight.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/preflight.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/preflight.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/ring_buffer.o: ../ring_buffer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/ring_buffer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ring_buffer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../ring_buffer.c  -o ${OBJECTDIR}/_ext/1472/ring_buffer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/ring_buffer.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"./projectSpecific" -I"../../MAVLink/include" -I".." -I"../../libUDB" -I"../../libDCM" -I"../../libFlashFS" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../Microchip/Include" -msmart-io=1 -Wall -msfr-warn=on
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ring_buffer.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1077454706/traps_asm.o: ../../libUDB/traps_asm.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/traps_asm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/traps_asm.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libUDB/traps_asm.s  -o ${OBJECTDIR}/_ext/1077454706/traps_asm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/1077454706/traps_asm.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/traps_asm.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1077454706/traps_asm.o: ../../libUDB/traps_asm.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/traps_asm.o.d 
	@${RM} ${OBJECTDIR}/_ext/1077454706/traps_asm.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libUDB/traps_asm.s  -o ${OBJECTDIR}/_ext/1077454706/traps_asm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/1077454706/traps_asm.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/traps_asm.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/stack.o: ../stack.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/stack.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/stack.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../stack.S  -o ${OBJECTDIR}/_ext/1472/stack.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/stack.o.d"  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/1472/stack.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/stack.o.d" "${OBJECTDIR}/_ext/1472/stack.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1472/stack.o: ../stack.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/stack.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/stack.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../stack.S  -o ${OBJECTDIR}/_ext/1472/stack.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/stack.o.d"  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/1472/stack.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/stack.o.d" "${OBJECTDIR}/_ext/1472/stack.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Polaris-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../VectorMatrix.X/dist/AUAV3/production/VectorMatrix.X.a ../Microchip_Library_Apps.X/dist/AUAV3/production/Microchip_Library_Apps.X.a  
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Polaris-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}    ../VectorMatrix.X/dist/AUAV3/production/VectorMatrix.X.a ../Microchip_Library_Apps.X/dist/AUAV3/production/Microchip_Library_Apps.X.a  -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--heap=200,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Polaris-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../VectorMatrix.X/dist/AUAV3/production/VectorMatrix.X.a ../Microchip_Library_Apps.X/dist/AUAV3/production/Microchip_Library_Apps.X.a 
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/Polaris-auav3.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}    ../VectorMatrix.X/dist/AUAV3/production/VectorMatrix.X.a ../Microchip_Library_Apps.X/dist/AUAV3/production/Microchip_Library_Apps.X.a  -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--heap=200,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/Polaris-auav3.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
endif

.pre:
	@echo "--------------------------------------"
	@echo "User defined pre-build step: [touch ../../MatrixPilot/console.c; touch ../../MatrixPilot/config_tests.c]"
	@touch ../../MatrixPilot/console.c; touch ../../MatrixPilot/config_tests.c
	@echo "--------------------------------------"

# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif

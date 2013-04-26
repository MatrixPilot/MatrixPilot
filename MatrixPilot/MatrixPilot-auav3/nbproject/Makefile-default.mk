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
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MatrixPilot-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MatrixPilot-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1658199856/cputest.o ${OBJECTDIR}/_ext/1658199856/cputestisr.o ${OBJECTDIR}/_ext/1658199856/cputestsubsets.o ${OBJECTDIR}/_ext/1658199856/read.o ${OBJECTDIR}/_ext/1658199856/uart3.o ${OBJECTDIR}/_ext/1658199856/write.o ${OBJECTDIR}/_ext/1077471063/estWind.o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o ${OBJECTDIR}/_ext/1077471063/rmat.o ${OBJECTDIR}/_ext/1077471063/libDCM.o ${OBJECTDIR}/_ext/1077471063/deadReckoning.o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o ${OBJECTDIR}/_ext/1864667112/AT45D.o ${OBJECTDIR}/_ext/1864667112/FSIO.o ${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o ${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o ${OBJECTDIR}/_ext/1864667112/usb.o ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o ${OBJECTDIR}/_ext/1864667112/usb_device.o ${OBJECTDIR}/_ext/1864667112/usb_function_msd.o ${OBJECTDIR}/_ext/1077454706/background.o ${OBJECTDIR}/_ext/1077454706/libUDB.o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o ${OBJECTDIR}/_ext/1077454706/servoOut.o ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o ${OBJECTDIR}/_ext/1077454706/osd.o ${OBJECTDIR}/_ext/1077454706/24LC256.o ${OBJECTDIR}/_ext/1077454706/events.o ${OBJECTDIR}/_ext/1077454706/I2C_udb4.o ${OBJECTDIR}/_ext/1077454706/mpu6000.o ${OBJECTDIR}/_ext/1077454706/spiUtils.o ${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o ${OBJECTDIR}/_ext/1077454706/traps_asm.o ${OBJECTDIR}/_ext/1077454706/mcu.o ${OBJECTDIR}/_ext/679848855/madd.o ${OBJECTDIR}/_ext/679848855/mmul.o ${OBJECTDIR}/_ext/679848855/mscl.o ${OBJECTDIR}/_ext/679848855/msub.o ${OBJECTDIR}/_ext/679848855/mtrp.o ${OBJECTDIR}/_ext/679848855/vadd.o ${OBJECTDIR}/_ext/679848855/vcon.o ${OBJECTDIR}/_ext/679848855/vcopy.o ${OBJECTDIR}/_ext/679848855/vcor.o ${OBJECTDIR}/_ext/679848855/vdot.o ${OBJECTDIR}/_ext/679848855/vmax.o ${OBJECTDIR}/_ext/679848855/vmin.o ${OBJECTDIR}/_ext/679848855/vmul.o ${OBJECTDIR}/_ext/679848855/vneg.o ${OBJECTDIR}/_ext/679848855/vpow.o ${OBJECTDIR}/_ext/679848855/vscl.o ${OBJECTDIR}/_ext/679848855/vsub.o ${OBJECTDIR}/_ext/679848855/vzpad.o ${OBJECTDIR}/_ext/1472/config_tests.o ${OBJECTDIR}/_ext/1472/behavior.o ${OBJECTDIR}/_ext/1472/cameraCntrl.o ${OBJECTDIR}/_ext/1472/navigate.o ${OBJECTDIR}/_ext/1472/pitchCntrl.o ${OBJECTDIR}/_ext/1472/rollCntrl.o ${OBJECTDIR}/_ext/1472/servoMix.o ${OBJECTDIR}/_ext/1472/states.o ${OBJECTDIR}/_ext/1472/yawCntrl.o ${OBJECTDIR}/_ext/1472/telemetry.o ${OBJECTDIR}/_ext/1472/servoPrepare.o ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o ${OBJECTDIR}/_ext/1472/flightplan-logo.o ${OBJECTDIR}/_ext/1472/mp_osd.o ${OBJECTDIR}/_ext/1472/altitudeCntrl.o ${OBJECTDIR}/_ext/1472/MAVLink.o ${OBJECTDIR}/_ext/1472/mode_switch.o ${OBJECTDIR}/_ext/1472/data_services.o ${OBJECTDIR}/_ext/1472/data_storage.o ${OBJECTDIR}/_ext/1472/nv_memory_table.o ${OBJECTDIR}/_ext/1472/parameter_table.o ${OBJECTDIR}/_ext/1472/airspeedCntrl.o ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o ${OBJECTDIR}/_ext/1472/euler_angles.o ${OBJECTDIR}/_ext/1472/main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1658199856/cputest.o.d ${OBJECTDIR}/_ext/1658199856/cputestisr.o.d ${OBJECTDIR}/_ext/1658199856/cputestsubsets.o.d ${OBJECTDIR}/_ext/1658199856/read.o.d ${OBJECTDIR}/_ext/1658199856/uart3.o.d ${OBJECTDIR}/_ext/1658199856/write.o.d ${OBJECTDIR}/_ext/1077471063/estWind.o.d ${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d ${OBJECTDIR}/_ext/1077471063/rmat.o.d ${OBJECTDIR}/_ext/1077471063/libDCM.o.d ${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d ${OBJECTDIR}/_ext/1864667112/AT45D.o.d ${OBJECTDIR}/_ext/1864667112/FSIO.o.d ${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o.d ${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d ${OBJECTDIR}/_ext/1864667112/usb.o.d ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d ${OBJECTDIR}/_ext/1864667112/usb_device.o.d ${OBJECTDIR}/_ext/1864667112/usb_function_msd.o.d ${OBJECTDIR}/_ext/1077454706/background.o.d ${OBJECTDIR}/_ext/1077454706/libUDB.o.d ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d ${OBJECTDIR}/_ext/1077454706/servoOut.o.d ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d ${OBJECTDIR}/_ext/1077454706/osd.o.d ${OBJECTDIR}/_ext/1077454706/24LC256.o.d ${OBJECTDIR}/_ext/1077454706/events.o.d ${OBJECTDIR}/_ext/1077454706/I2C_udb4.o.d ${OBJECTDIR}/_ext/1077454706/mpu6000.o.d ${OBJECTDIR}/_ext/1077454706/spiUtils.o.d ${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o.d ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d ${OBJECTDIR}/_ext/1077454706/traps_asm.o.d ${OBJECTDIR}/_ext/1077454706/mcu.o.d ${OBJECTDIR}/_ext/679848855/madd.o.d ${OBJECTDIR}/_ext/679848855/mmul.o.d ${OBJECTDIR}/_ext/679848855/mscl.o.d ${OBJECTDIR}/_ext/679848855/msub.o.d ${OBJECTDIR}/_ext/679848855/mtrp.o.d ${OBJECTDIR}/_ext/679848855/vadd.o.d ${OBJECTDIR}/_ext/679848855/vcon.o.d ${OBJECTDIR}/_ext/679848855/vcopy.o.d ${OBJECTDIR}/_ext/679848855/vcor.o.d ${OBJECTDIR}/_ext/679848855/vdot.o.d ${OBJECTDIR}/_ext/679848855/vmax.o.d ${OBJECTDIR}/_ext/679848855/vmin.o.d ${OBJECTDIR}/_ext/679848855/vmul.o.d ${OBJECTDIR}/_ext/679848855/vneg.o.d ${OBJECTDIR}/_ext/679848855/vpow.o.d ${OBJECTDIR}/_ext/679848855/vscl.o.d ${OBJECTDIR}/_ext/679848855/vsub.o.d ${OBJECTDIR}/_ext/679848855/vzpad.o.d ${OBJECTDIR}/_ext/1472/config_tests.o.d ${OBJECTDIR}/_ext/1472/behavior.o.d ${OBJECTDIR}/_ext/1472/cameraCntrl.o.d ${OBJECTDIR}/_ext/1472/navigate.o.d ${OBJECTDIR}/_ext/1472/pitchCntrl.o.d ${OBJECTDIR}/_ext/1472/rollCntrl.o.d ${OBJECTDIR}/_ext/1472/servoMix.o.d ${OBJECTDIR}/_ext/1472/states.o.d ${OBJECTDIR}/_ext/1472/yawCntrl.o.d ${OBJECTDIR}/_ext/1472/telemetry.o.d ${OBJECTDIR}/_ext/1472/servoPrepare.o.d ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d ${OBJECTDIR}/_ext/1472/flightplan-logo.o.d ${OBJECTDIR}/_ext/1472/mp_osd.o.d ${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d ${OBJECTDIR}/_ext/1472/MAVLink.o.d ${OBJECTDIR}/_ext/1472/mode_switch.o.d ${OBJECTDIR}/_ext/1472/data_services.o.d ${OBJECTDIR}/_ext/1472/data_storage.o.d ${OBJECTDIR}/_ext/1472/nv_memory_table.o.d ${OBJECTDIR}/_ext/1472/parameter_table.o.d ${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d ${OBJECTDIR}/_ext/1472/euler_angles.o.d ${OBJECTDIR}/_ext/1472/main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1658199856/cputest.o ${OBJECTDIR}/_ext/1658199856/cputestisr.o ${OBJECTDIR}/_ext/1658199856/cputestsubsets.o ${OBJECTDIR}/_ext/1658199856/read.o ${OBJECTDIR}/_ext/1658199856/uart3.o ${OBJECTDIR}/_ext/1658199856/write.o ${OBJECTDIR}/_ext/1077471063/estWind.o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o ${OBJECTDIR}/_ext/1077471063/rmat.o ${OBJECTDIR}/_ext/1077471063/libDCM.o ${OBJECTDIR}/_ext/1077471063/deadReckoning.o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o ${OBJECTDIR}/_ext/1864667112/AT45D.o ${OBJECTDIR}/_ext/1864667112/FSIO.o ${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o ${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o ${OBJECTDIR}/_ext/1864667112/usb.o ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o ${OBJECTDIR}/_ext/1864667112/usb_device.o ${OBJECTDIR}/_ext/1864667112/usb_function_msd.o ${OBJECTDIR}/_ext/1077454706/background.o ${OBJECTDIR}/_ext/1077454706/libUDB.o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o ${OBJECTDIR}/_ext/1077454706/servoOut.o ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o ${OBJECTDIR}/_ext/1077454706/osd.o ${OBJECTDIR}/_ext/1077454706/24LC256.o ${OBJECTDIR}/_ext/1077454706/events.o ${OBJECTDIR}/_ext/1077454706/I2C_udb4.o ${OBJECTDIR}/_ext/1077454706/mpu6000.o ${OBJECTDIR}/_ext/1077454706/spiUtils.o ${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o ${OBJECTDIR}/_ext/1077454706/traps_asm.o ${OBJECTDIR}/_ext/1077454706/mcu.o ${OBJECTDIR}/_ext/679848855/madd.o ${OBJECTDIR}/_ext/679848855/mmul.o ${OBJECTDIR}/_ext/679848855/mscl.o ${OBJECTDIR}/_ext/679848855/msub.o ${OBJECTDIR}/_ext/679848855/mtrp.o ${OBJECTDIR}/_ext/679848855/vadd.o ${OBJECTDIR}/_ext/679848855/vcon.o ${OBJECTDIR}/_ext/679848855/vcopy.o ${OBJECTDIR}/_ext/679848855/vcor.o ${OBJECTDIR}/_ext/679848855/vdot.o ${OBJECTDIR}/_ext/679848855/vmax.o ${OBJECTDIR}/_ext/679848855/vmin.o ${OBJECTDIR}/_ext/679848855/vmul.o ${OBJECTDIR}/_ext/679848855/vneg.o ${OBJECTDIR}/_ext/679848855/vpow.o ${OBJECTDIR}/_ext/679848855/vscl.o ${OBJECTDIR}/_ext/679848855/vsub.o ${OBJECTDIR}/_ext/679848855/vzpad.o ${OBJECTDIR}/_ext/1472/config_tests.o ${OBJECTDIR}/_ext/1472/behavior.o ${OBJECTDIR}/_ext/1472/cameraCntrl.o ${OBJECTDIR}/_ext/1472/navigate.o ${OBJECTDIR}/_ext/1472/pitchCntrl.o ${OBJECTDIR}/_ext/1472/rollCntrl.o ${OBJECTDIR}/_ext/1472/servoMix.o ${OBJECTDIR}/_ext/1472/states.o ${OBJECTDIR}/_ext/1472/yawCntrl.o ${OBJECTDIR}/_ext/1472/telemetry.o ${OBJECTDIR}/_ext/1472/servoPrepare.o ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o ${OBJECTDIR}/_ext/1472/flightplan-logo.o ${OBJECTDIR}/_ext/1472/mp_osd.o ${OBJECTDIR}/_ext/1472/altitudeCntrl.o ${OBJECTDIR}/_ext/1472/MAVLink.o ${OBJECTDIR}/_ext/1472/mode_switch.o ${OBJECTDIR}/_ext/1472/data_services.o ${OBJECTDIR}/_ext/1472/data_storage.o ${OBJECTDIR}/_ext/1472/nv_memory_table.o ${OBJECTDIR}/_ext/1472/parameter_table.o ${OBJECTDIR}/_ext/1472/airspeedCntrl.o ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o ${OBJECTDIR}/_ext/1472/euler_angles.o ${OBJECTDIR}/_ext/1472/main.o


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

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/MatrixPilot-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP512MU810
MP_LINKER_FILE_OPTION=,--script=p33EP512MU810.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1658199856/cputest.o: ../../libCommon/cputest.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/cputest.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libCommon/cputest.c  -o ${OBJECTDIR}/_ext/1658199856/cputest.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1658199856/cputest.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/cputest.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1658199856/read.o: ../../libCommon/read.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/read.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libCommon/read.c  -o ${OBJECTDIR}/_ext/1658199856/read.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1658199856/read.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/read.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1658199856/uart3.o: ../../libCommon/uart3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/uart3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libCommon/uart3.c  -o ${OBJECTDIR}/_ext/1658199856/uart3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1658199856/uart3.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/uart3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1658199856/write.o: ../../libCommon/write.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/write.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libCommon/write.c  -o ${OBJECTDIR}/_ext/1658199856/write.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1658199856/write.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/write.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estWind.o: ../../libDCM/estWind.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estWind.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estWind.c  -o ${OBJECTDIR}/_ext/1077471063/estWind.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estWind.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estWind.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estYawDrift.o: ../../libDCM/estYawDrift.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estYawDrift.c  -o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o: ../../libDCM/gpsParseCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseCommon.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o: ../../libDCM/gpsParseSTD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseSTD.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o: ../../libDCM/gpsParseUBX.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseUBX.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/mathlibNAV.o: ../../libDCM/mathlibNAV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/mathlibNAV.c  -o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/rmat.o: ../../libDCM/rmat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/rmat.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/rmat.c  -o ${OBJECTDIR}/_ext/1077471063/rmat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/rmat.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/rmat.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/libDCM.o: ../../libDCM/libDCM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/libDCM.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/libDCM.c  -o ${OBJECTDIR}/_ext/1077471063/libDCM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/libDCM.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/libDCM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/deadReckoning.o: ../../libDCM/deadReckoning.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/deadReckoning.c  -o ${OBJECTDIR}/_ext/1077471063/deadReckoning.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o: ../../libDCM/gpsParseMTEK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseMTEK.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/AT45D.o: ../../libFlashFS/AT45D.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/AT45D.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/AT45D.c  -o ${OBJECTDIR}/_ext/1864667112/AT45D.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/AT45D.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/AT45D.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/FSIO.o: ../../libFlashFS/FSIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/FSIO.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/FSIO.c  -o ${OBJECTDIR}/_ext/1864667112/FSIO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/FSIO.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/FSIO.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o: ../../libFlashFS/FSIO_DBG.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/FSIO_DBG.c  -o ${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o: ../../libFlashFS/MDD_AT45D.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/MDD_AT45D.c  -o ${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb.o: ../../libFlashFS/usb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb.c  -o ${OBJECTDIR}/_ext/1864667112/usb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_descriptors.o: ../../libFlashFS/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_descriptors.c  -o ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_device.o: ../../libFlashFS/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_device.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_device.c  -o ${OBJECTDIR}/_ext/1864667112/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_device.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_function_msd.o: ../../libFlashFS/usb_function_msd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_function_msd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_function_msd.c  -o ${OBJECTDIR}/_ext/1864667112/usb_function_msd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_function_msd.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_function_msd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/background.o: ../../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/background.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/background.c  -o ${OBJECTDIR}/_ext/1077454706/background.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/background.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/background.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/libUDB.o: ../../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/libUDB.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/libUDB.c  -o ${OBJECTDIR}/_ext/1077454706/libUDB.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/libUDB.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/libUDB.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o: ../../libUDB/analog2digital_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/magneto_udb4.o: ../../libUDB/magneto_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/magneto_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o: ../../libUDB/radioIn_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/radioIn_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/servoOut.o: ../../libUDB/servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/servoOut.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/servoOut.c  -o ${OBJECTDIR}/_ext/1077454706/servoOut.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/servoOut.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/servoOut.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o: ../../libUDB/serialIO_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/serialIO_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/osd.o: ../../libUDB/osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/osd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/osd.c  -o ${OBJECTDIR}/_ext/1077454706/osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/osd.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/24LC256.o: ../../libUDB/24LC256.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/24LC256.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/24LC256.c  -o ${OBJECTDIR}/_ext/1077454706/24LC256.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/24LC256.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/24LC256.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/events.o: ../../libUDB/events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/events.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/events.c  -o ${OBJECTDIR}/_ext/1077454706/events.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/events.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/events.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/I2C_udb4.o: ../../libUDB/I2C_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/I2C_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/I2C_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/I2C_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/I2C_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/I2C_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/mpu6000.o: ../../libUDB/mpu6000.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mpu6000.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/mpu6000.c  -o ${OBJECTDIR}/_ext/1077454706/mpu6000.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/mpu6000.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/mpu6000.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/spiUtils.o: ../../libUDB/spiUtils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/spiUtils.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/spiUtils.c  -o ${OBJECTDIR}/_ext/1077454706/spiUtils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/spiUtils.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/spiUtils.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o: ../../libUDB/radioIn_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/radioIn_auav3.c  -o ${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o: ../../libUDB/analog2digital_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_auav3.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/mcu.o: ../../libUDB/mcu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mcu.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/mcu.c  -o ${OBJECTDIR}/_ext/1077454706/mcu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/mcu.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/mcu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/config_tests.o: ../config_tests.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/config_tests.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../config_tests.c  -o ${OBJECTDIR}/_ext/1472/config_tests.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/config_tests.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/config_tests.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/behavior.o: ../behavior.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/behavior.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../behavior.c  -o ${OBJECTDIR}/_ext/1472/behavior.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/behavior.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/behavior.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/cameraCntrl.o: ../cameraCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/cameraCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../cameraCntrl.c  -o ${OBJECTDIR}/_ext/1472/cameraCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/cameraCntrl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/cameraCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/navigate.o: ../navigate.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/navigate.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../navigate.c  -o ${OBJECTDIR}/_ext/1472/navigate.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/navigate.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/navigate.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/pitchCntrl.o: ../pitchCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/pitchCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../pitchCntrl.c  -o ${OBJECTDIR}/_ext/1472/pitchCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/pitchCntrl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pitchCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/rollCntrl.o: ../rollCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/rollCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../rollCntrl.c  -o ${OBJECTDIR}/_ext/1472/rollCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/rollCntrl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/rollCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/servoMix.o: ../servoMix.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/servoMix.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../servoMix.c  -o ${OBJECTDIR}/_ext/1472/servoMix.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/servoMix.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/servoMix.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/states.o: ../states.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/states.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../states.c  -o ${OBJECTDIR}/_ext/1472/states.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/states.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/states.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/yawCntrl.o: ../yawCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/yawCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../yawCntrl.c  -o ${OBJECTDIR}/_ext/1472/yawCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/yawCntrl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/yawCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/telemetry.o: ../telemetry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../telemetry.c  -o ${OBJECTDIR}/_ext/1472/telemetry.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/telemetry.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/telemetry.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/servoPrepare.o: ../servoPrepare.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/servoPrepare.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../servoPrepare.c  -o ${OBJECTDIR}/_ext/1472/servoPrepare.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/servoPrepare.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/servoPrepare.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/flightplan-waypoints.o: ../flightplan-waypoints.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../flightplan-waypoints.c  -o ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/flightplan-logo.o: ../flightplan-logo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-logo.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../flightplan-logo.c  -o ${OBJECTDIR}/_ext/1472/flightplan-logo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/flightplan-logo.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/flightplan-logo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mp_osd.o: ../mp_osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mp_osd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mp_osd.c  -o ${OBJECTDIR}/_ext/1472/mp_osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mp_osd.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mp_osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/altitudeCntrl.o: ../altitudeCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../altitudeCntrl.c  -o ${OBJECTDIR}/_ext/1472/altitudeCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/MAVLink.o: ../MAVLink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/MAVLink.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../MAVLink.c  -o ${OBJECTDIR}/_ext/1472/MAVLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/MAVLink.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/MAVLink.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mode_switch.o: ../mode_switch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mode_switch.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mode_switch.c  -o ${OBJECTDIR}/_ext/1472/mode_switch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mode_switch.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mode_switch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/data_services.o: ../data_services.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/data_services.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../data_services.c  -o ${OBJECTDIR}/_ext/1472/data_services.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/data_services.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/data_services.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/data_storage.o: ../data_storage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/data_storage.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../data_storage.c  -o ${OBJECTDIR}/_ext/1472/data_storage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/data_storage.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/data_storage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/nv_memory_table.o: ../nv_memory_table.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/nv_memory_table.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../nv_memory_table.c  -o ${OBJECTDIR}/_ext/1472/nv_memory_table.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/nv_memory_table.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/nv_memory_table.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/parameter_table.o: ../parameter_table.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/parameter_table.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../parameter_table.c  -o ${OBJECTDIR}/_ext/1472/parameter_table.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/parameter_table.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/parameter_table.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/airspeedCntrl.o: ../airspeedCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../airspeedCntrl.c  -o ${OBJECTDIR}/_ext/1472/airspeedCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o: ../altitudeCntrlVariable.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../altitudeCntrlVariable.c  -o ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/euler_angles.o: ../euler_angles.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/euler_angles.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../euler_angles.c  -o ${OBJECTDIR}/_ext/1472/euler_angles.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/euler_angles.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/euler_angles.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main.c  -o ${OBJECTDIR}/_ext/1472/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1658199856/cputest.o: ../../libCommon/cputest.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/cputest.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libCommon/cputest.c  -o ${OBJECTDIR}/_ext/1658199856/cputest.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1658199856/cputest.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/cputest.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1658199856/read.o: ../../libCommon/read.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/read.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libCommon/read.c  -o ${OBJECTDIR}/_ext/1658199856/read.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1658199856/read.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/read.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1658199856/uart3.o: ../../libCommon/uart3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/uart3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libCommon/uart3.c  -o ${OBJECTDIR}/_ext/1658199856/uart3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1658199856/uart3.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/uart3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1658199856/write.o: ../../libCommon/write.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/write.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libCommon/write.c  -o ${OBJECTDIR}/_ext/1658199856/write.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1658199856/write.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/write.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estWind.o: ../../libDCM/estWind.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estWind.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estWind.c  -o ${OBJECTDIR}/_ext/1077471063/estWind.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estWind.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estWind.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estYawDrift.o: ../../libDCM/estYawDrift.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estYawDrift.c  -o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o: ../../libDCM/gpsParseCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseCommon.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o: ../../libDCM/gpsParseSTD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseSTD.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o: ../../libDCM/gpsParseUBX.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseUBX.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/mathlibNAV.o: ../../libDCM/mathlibNAV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/mathlibNAV.c  -o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/rmat.o: ../../libDCM/rmat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/rmat.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/rmat.c  -o ${OBJECTDIR}/_ext/1077471063/rmat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/rmat.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/rmat.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/libDCM.o: ../../libDCM/libDCM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/libDCM.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/libDCM.c  -o ${OBJECTDIR}/_ext/1077471063/libDCM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/libDCM.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/libDCM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/deadReckoning.o: ../../libDCM/deadReckoning.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/deadReckoning.c  -o ${OBJECTDIR}/_ext/1077471063/deadReckoning.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o: ../../libDCM/gpsParseMTEK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseMTEK.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/AT45D.o: ../../libFlashFS/AT45D.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/AT45D.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/AT45D.c  -o ${OBJECTDIR}/_ext/1864667112/AT45D.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/AT45D.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/AT45D.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/FSIO.o: ../../libFlashFS/FSIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/FSIO.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/FSIO.c  -o ${OBJECTDIR}/_ext/1864667112/FSIO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/FSIO.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/FSIO.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o: ../../libFlashFS/FSIO_DBG.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/FSIO_DBG.c  -o ${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/FSIO_DBG.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o: ../../libFlashFS/MDD_AT45D.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/MDD_AT45D.c  -o ${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb.o: ../../libFlashFS/usb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb.c  -o ${OBJECTDIR}/_ext/1864667112/usb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_descriptors.o: ../../libFlashFS/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_descriptors.c  -o ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_device.o: ../../libFlashFS/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_device.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_device.c  -o ${OBJECTDIR}/_ext/1864667112/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_device.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_function_msd.o: ../../libFlashFS/usb_function_msd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_function_msd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_function_msd.c  -o ${OBJECTDIR}/_ext/1864667112/usb_function_msd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_function_msd.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_function_msd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/background.o: ../../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/background.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/background.c  -o ${OBJECTDIR}/_ext/1077454706/background.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/background.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/background.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/libUDB.o: ../../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/libUDB.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/libUDB.c  -o ${OBJECTDIR}/_ext/1077454706/libUDB.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/libUDB.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/libUDB.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o: ../../libUDB/analog2digital_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/magneto_udb4.o: ../../libUDB/magneto_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/magneto_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o: ../../libUDB/radioIn_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/radioIn_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/servoOut.o: ../../libUDB/servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/servoOut.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/servoOut.c  -o ${OBJECTDIR}/_ext/1077454706/servoOut.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/servoOut.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/servoOut.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o: ../../libUDB/serialIO_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/serialIO_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/osd.o: ../../libUDB/osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/osd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/osd.c  -o ${OBJECTDIR}/_ext/1077454706/osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/osd.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/24LC256.o: ../../libUDB/24LC256.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/24LC256.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/24LC256.c  -o ${OBJECTDIR}/_ext/1077454706/24LC256.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/24LC256.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/24LC256.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/events.o: ../../libUDB/events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/events.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/events.c  -o ${OBJECTDIR}/_ext/1077454706/events.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/events.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/events.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/I2C_udb4.o: ../../libUDB/I2C_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/I2C_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/I2C_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/I2C_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/I2C_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/I2C_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/mpu6000.o: ../../libUDB/mpu6000.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mpu6000.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/mpu6000.c  -o ${OBJECTDIR}/_ext/1077454706/mpu6000.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/mpu6000.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/mpu6000.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/spiUtils.o: ../../libUDB/spiUtils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/spiUtils.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/spiUtils.c  -o ${OBJECTDIR}/_ext/1077454706/spiUtils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/spiUtils.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/spiUtils.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o: ../../libUDB/radioIn_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/radioIn_auav3.c  -o ${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/radioIn_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o: ../../libUDB/analog2digital_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_auav3.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/mcu.o: ../../libUDB/mcu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/mcu.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/mcu.c  -o ${OBJECTDIR}/_ext/1077454706/mcu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/mcu.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/mcu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/config_tests.o: ../config_tests.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/config_tests.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../config_tests.c  -o ${OBJECTDIR}/_ext/1472/config_tests.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/config_tests.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/config_tests.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/behavior.o: ../behavior.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/behavior.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../behavior.c  -o ${OBJECTDIR}/_ext/1472/behavior.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/behavior.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/behavior.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/cameraCntrl.o: ../cameraCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/cameraCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../cameraCntrl.c  -o ${OBJECTDIR}/_ext/1472/cameraCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/cameraCntrl.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/cameraCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/navigate.o: ../navigate.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/navigate.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../navigate.c  -o ${OBJECTDIR}/_ext/1472/navigate.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/navigate.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/navigate.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/pitchCntrl.o: ../pitchCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/pitchCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../pitchCntrl.c  -o ${OBJECTDIR}/_ext/1472/pitchCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/pitchCntrl.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/pitchCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/rollCntrl.o: ../rollCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/rollCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../rollCntrl.c  -o ${OBJECTDIR}/_ext/1472/rollCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/rollCntrl.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/rollCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/servoMix.o: ../servoMix.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/servoMix.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../servoMix.c  -o ${OBJECTDIR}/_ext/1472/servoMix.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/servoMix.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/servoMix.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/states.o: ../states.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/states.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../states.c  -o ${OBJECTDIR}/_ext/1472/states.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/states.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/states.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/yawCntrl.o: ../yawCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/yawCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../yawCntrl.c  -o ${OBJECTDIR}/_ext/1472/yawCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/yawCntrl.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/yawCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/telemetry.o: ../telemetry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../telemetry.c  -o ${OBJECTDIR}/_ext/1472/telemetry.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/telemetry.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/telemetry.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/servoPrepare.o: ../servoPrepare.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/servoPrepare.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../servoPrepare.c  -o ${OBJECTDIR}/_ext/1472/servoPrepare.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/servoPrepare.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/servoPrepare.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/flightplan-waypoints.o: ../flightplan-waypoints.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../flightplan-waypoints.c  -o ${OBJECTDIR}/_ext/1472/flightplan-waypoints.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/flightplan-waypoints.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/flightplan-logo.o: ../flightplan-logo.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/flightplan-logo.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../flightplan-logo.c  -o ${OBJECTDIR}/_ext/1472/flightplan-logo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/flightplan-logo.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/flightplan-logo.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mp_osd.o: ../mp_osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mp_osd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mp_osd.c  -o ${OBJECTDIR}/_ext/1472/mp_osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mp_osd.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mp_osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/altitudeCntrl.o: ../altitudeCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../altitudeCntrl.c  -o ${OBJECTDIR}/_ext/1472/altitudeCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/altitudeCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/MAVLink.o: ../MAVLink.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/MAVLink.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../MAVLink.c  -o ${OBJECTDIR}/_ext/1472/MAVLink.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/MAVLink.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/MAVLink.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/mode_switch.o: ../mode_switch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/mode_switch.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../mode_switch.c  -o ${OBJECTDIR}/_ext/1472/mode_switch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/mode_switch.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/mode_switch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/data_services.o: ../data_services.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/data_services.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../data_services.c  -o ${OBJECTDIR}/_ext/1472/data_services.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/data_services.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/data_services.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/data_storage.o: ../data_storage.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/data_storage.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../data_storage.c  -o ${OBJECTDIR}/_ext/1472/data_storage.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/data_storage.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/data_storage.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/nv_memory_table.o: ../nv_memory_table.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/nv_memory_table.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../nv_memory_table.c  -o ${OBJECTDIR}/_ext/1472/nv_memory_table.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/nv_memory_table.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/nv_memory_table.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/parameter_table.o: ../parameter_table.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/parameter_table.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../parameter_table.c  -o ${OBJECTDIR}/_ext/1472/parameter_table.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/parameter_table.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/parameter_table.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/airspeedCntrl.o: ../airspeedCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../airspeedCntrl.c  -o ${OBJECTDIR}/_ext/1472/airspeedCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/airspeedCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o: ../altitudeCntrlVariable.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../altitudeCntrlVariable.c  -o ${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/altitudeCntrlVariable.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/euler_angles.o: ../euler_angles.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/euler_angles.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../euler_angles.c  -o ${OBJECTDIR}/_ext/1472/euler_angles.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/euler_angles.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/euler_angles.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main.c  -o ${OBJECTDIR}/_ext/1472/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../../MAVLink/include" -I"../../libUDB" -I"../../libDCM" -I"../../MAVLink/include/common" -I"../../MAVLink/include/matrixpilot" -I"../../MatrixPilot" -I"../../libFlashFS" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1658199856/cputestisr.o: ../../libCommon/cputestisr.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/cputestisr.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libCommon/cputestisr.s  -o ${OBJECTDIR}/_ext/1658199856/cputestisr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/1658199856/cputestisr.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/cputestisr.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1658199856/cputestsubsets.o: ../../libCommon/cputestsubsets.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/cputestsubsets.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libCommon/cputestsubsets.s  -o ${OBJECTDIR}/_ext/1658199856/cputestsubsets.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/1658199856/cputestsubsets.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/cputestsubsets.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1077454706/traps_asm.o: ../../libUDB/traps_asm.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/traps_asm.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libUDB/traps_asm.s  -o ${OBJECTDIR}/_ext/1077454706/traps_asm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/1077454706/traps_asm.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/traps_asm.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/madd.o: ../../libVectorMatrix/madd.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/madd.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/madd.s  -o ${OBJECTDIR}/_ext/679848855/madd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/madd.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/madd.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/mmul.o: ../../libVectorMatrix/mmul.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/mmul.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/mmul.s  -o ${OBJECTDIR}/_ext/679848855/mmul.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/mmul.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/mmul.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/mscl.o: ../../libVectorMatrix/mscl.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/mscl.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/mscl.s  -o ${OBJECTDIR}/_ext/679848855/mscl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/mscl.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/mscl.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/msub.o: ../../libVectorMatrix/msub.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/msub.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/msub.s  -o ${OBJECTDIR}/_ext/679848855/msub.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/msub.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/msub.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/mtrp.o: ../../libVectorMatrix/mtrp.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/mtrp.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/mtrp.s  -o ${OBJECTDIR}/_ext/679848855/mtrp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/mtrp.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/mtrp.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vadd.o: ../../libVectorMatrix/vadd.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vadd.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vadd.s  -o ${OBJECTDIR}/_ext/679848855/vadd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vadd.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vadd.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vcon.o: ../../libVectorMatrix/vcon.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vcon.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vcon.s  -o ${OBJECTDIR}/_ext/679848855/vcon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vcon.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vcon.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vcopy.o: ../../libVectorMatrix/vcopy.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vcopy.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vcopy.s  -o ${OBJECTDIR}/_ext/679848855/vcopy.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vcopy.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vcopy.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vcor.o: ../../libVectorMatrix/vcor.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vcor.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vcor.s  -o ${OBJECTDIR}/_ext/679848855/vcor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vcor.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vcor.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vdot.o: ../../libVectorMatrix/vdot.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vdot.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vdot.s  -o ${OBJECTDIR}/_ext/679848855/vdot.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vdot.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vdot.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vmax.o: ../../libVectorMatrix/vmax.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vmax.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vmax.s  -o ${OBJECTDIR}/_ext/679848855/vmax.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vmax.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vmax.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vmin.o: ../../libVectorMatrix/vmin.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vmin.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vmin.s  -o ${OBJECTDIR}/_ext/679848855/vmin.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vmin.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vmin.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vmul.o: ../../libVectorMatrix/vmul.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vmul.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vmul.s  -o ${OBJECTDIR}/_ext/679848855/vmul.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vmul.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vmul.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vneg.o: ../../libVectorMatrix/vneg.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vneg.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vneg.s  -o ${OBJECTDIR}/_ext/679848855/vneg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vneg.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vneg.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vpow.o: ../../libVectorMatrix/vpow.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vpow.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vpow.s  -o ${OBJECTDIR}/_ext/679848855/vpow.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vpow.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vpow.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vscl.o: ../../libVectorMatrix/vscl.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vscl.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vscl.s  -o ${OBJECTDIR}/_ext/679848855/vscl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vscl.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vscl.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vsub.o: ../../libVectorMatrix/vsub.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vsub.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vsub.s  -o ${OBJECTDIR}/_ext/679848855/vsub.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vsub.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vsub.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vzpad.o: ../../libVectorMatrix/vzpad.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vzpad.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vzpad.s  -o ${OBJECTDIR}/_ext/679848855/vzpad.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vzpad.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vzpad.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1658199856/cputestisr.o: ../../libCommon/cputestisr.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/cputestisr.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libCommon/cputestisr.s  -o ${OBJECTDIR}/_ext/1658199856/cputestisr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/1658199856/cputestisr.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/cputestisr.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1658199856/cputestsubsets.o: ../../libCommon/cputestsubsets.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1658199856 
	@${RM} ${OBJECTDIR}/_ext/1658199856/cputestsubsets.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libCommon/cputestsubsets.s  -o ${OBJECTDIR}/_ext/1658199856/cputestsubsets.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/1658199856/cputestsubsets.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1658199856/cputestsubsets.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1077454706/traps_asm.o: ../../libUDB/traps_asm.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/traps_asm.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libUDB/traps_asm.s  -o ${OBJECTDIR}/_ext/1077454706/traps_asm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/1077454706/traps_asm.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/traps_asm.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/madd.o: ../../libVectorMatrix/madd.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/madd.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/madd.s  -o ${OBJECTDIR}/_ext/679848855/madd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/madd.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/madd.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/mmul.o: ../../libVectorMatrix/mmul.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/mmul.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/mmul.s  -o ${OBJECTDIR}/_ext/679848855/mmul.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/mmul.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/mmul.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/mscl.o: ../../libVectorMatrix/mscl.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/mscl.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/mscl.s  -o ${OBJECTDIR}/_ext/679848855/mscl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/mscl.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/mscl.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/msub.o: ../../libVectorMatrix/msub.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/msub.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/msub.s  -o ${OBJECTDIR}/_ext/679848855/msub.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/msub.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/msub.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/mtrp.o: ../../libVectorMatrix/mtrp.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/mtrp.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/mtrp.s  -o ${OBJECTDIR}/_ext/679848855/mtrp.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/mtrp.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/mtrp.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vadd.o: ../../libVectorMatrix/vadd.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vadd.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vadd.s  -o ${OBJECTDIR}/_ext/679848855/vadd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vadd.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vadd.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vcon.o: ../../libVectorMatrix/vcon.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vcon.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vcon.s  -o ${OBJECTDIR}/_ext/679848855/vcon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vcon.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vcon.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vcopy.o: ../../libVectorMatrix/vcopy.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vcopy.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vcopy.s  -o ${OBJECTDIR}/_ext/679848855/vcopy.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vcopy.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vcopy.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vcor.o: ../../libVectorMatrix/vcor.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vcor.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vcor.s  -o ${OBJECTDIR}/_ext/679848855/vcor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vcor.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vcor.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vdot.o: ../../libVectorMatrix/vdot.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vdot.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vdot.s  -o ${OBJECTDIR}/_ext/679848855/vdot.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vdot.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vdot.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vmax.o: ../../libVectorMatrix/vmax.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vmax.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vmax.s  -o ${OBJECTDIR}/_ext/679848855/vmax.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vmax.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vmax.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vmin.o: ../../libVectorMatrix/vmin.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vmin.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vmin.s  -o ${OBJECTDIR}/_ext/679848855/vmin.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vmin.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vmin.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vmul.o: ../../libVectorMatrix/vmul.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vmul.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vmul.s  -o ${OBJECTDIR}/_ext/679848855/vmul.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vmul.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vmul.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vneg.o: ../../libVectorMatrix/vneg.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vneg.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vneg.s  -o ${OBJECTDIR}/_ext/679848855/vneg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vneg.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vneg.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vpow.o: ../../libVectorMatrix/vpow.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vpow.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vpow.s  -o ${OBJECTDIR}/_ext/679848855/vpow.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vpow.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vpow.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vscl.o: ../../libVectorMatrix/vscl.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vscl.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vscl.s  -o ${OBJECTDIR}/_ext/679848855/vscl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vscl.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vscl.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vsub.o: ../../libVectorMatrix/vsub.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vsub.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vsub.s  -o ${OBJECTDIR}/_ext/679848855/vsub.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vsub.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vsub.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/679848855/vzpad.o: ../../libVectorMatrix/vzpad.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/679848855 
	@${RM} ${OBJECTDIR}/_ext/679848855/vzpad.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libVectorMatrix/vzpad.s  -o ${OBJECTDIR}/_ext/679848855/vzpad.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -I"../../libVectorMatrix" -Wa,-MD,"${OBJECTDIR}/_ext/679848855/vzpad.o.d",--defsym=__MPLAB_BUILD=1,-g,--defsym,PSV_ERRATA=1,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/679848855/vzpad.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/MatrixPilot-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/MatrixPilot-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--heap=256,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/MatrixPilot-udb4.X.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/MatrixPilot-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/MatrixPilot-auav3.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--heap=256,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/MatrixPilot-udb4.X.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/MatrixPilot-auav3.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
endif


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

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif

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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MPMvirtualPose.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MPMvirtualPose.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/main.o ${OBJECTDIR}/motorCntrl.o ${OBJECTDIR}/telemetry.o ${OBJECTDIR}/servoOut.o ${OBJECTDIR}/a2d_dma_udb4.o ${OBJECTDIR}/serialIO_udb4.o ${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o ${OBJECTDIR}/_ext/1018724808/deadReckoning.o ${OBJECTDIR}/_ext/1018724808/estWind.o ${OBJECTDIR}/_ext/1018724808/estYawDrift.o ${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o ${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o ${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o ${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o ${OBJECTDIR}/_ext/1018724808/libDCM.o ${OBJECTDIR}/_ext/1018724808/mathlibNAV.o ${OBJECTDIR}/_ext/1018724808/rmat.o ${OBJECTDIR}/_ext/1018724808/rmat_obj.o ${OBJECTDIR}/_ext/1018708451/background.o ${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o ${OBJECTDIR}/_ext/1018708451/events.o ${OBJECTDIR}/_ext/1018708451/filters.o ${OBJECTDIR}/_ext/1018708451/I2C_udb4.o ${OBJECTDIR}/_ext/1018708451/libUDB.o ${OBJECTDIR}/_ext/1018708451/magneto_udb4.o ${OBJECTDIR}/_ext/1018708451/mpu6000.o ${OBJECTDIR}/_ext/1018708451/osd.o ${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o ${OBJECTDIR}/_ext/1018708451/spiUtils.o ${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o
POSSIBLE_DEPFILES=${OBJECTDIR}/main.o.d ${OBJECTDIR}/motorCntrl.o.d ${OBJECTDIR}/telemetry.o.d ${OBJECTDIR}/servoOut.o.d ${OBJECTDIR}/a2d_dma_udb4.o.d ${OBJECTDIR}/serialIO_udb4.o.d ${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o.d ${OBJECTDIR}/_ext/1018724808/deadReckoning.o.d ${OBJECTDIR}/_ext/1018724808/estWind.o.d ${OBJECTDIR}/_ext/1018724808/estYawDrift.o.d ${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o.d ${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o.d ${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o.d ${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o.d ${OBJECTDIR}/_ext/1018724808/libDCM.o.d ${OBJECTDIR}/_ext/1018724808/mathlibNAV.o.d ${OBJECTDIR}/_ext/1018724808/rmat.o.d ${OBJECTDIR}/_ext/1018724808/rmat_obj.o.d ${OBJECTDIR}/_ext/1018708451/background.o.d ${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o.d ${OBJECTDIR}/_ext/1018708451/events.o.d ${OBJECTDIR}/_ext/1018708451/filters.o.d ${OBJECTDIR}/_ext/1018708451/I2C_udb4.o.d ${OBJECTDIR}/_ext/1018708451/libUDB.o.d ${OBJECTDIR}/_ext/1018708451/magneto_udb4.o.d ${OBJECTDIR}/_ext/1018708451/mpu6000.o.d ${OBJECTDIR}/_ext/1018708451/osd.o.d ${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o.d ${OBJECTDIR}/_ext/1018708451/spiUtils.o.d ${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/main.o ${OBJECTDIR}/motorCntrl.o ${OBJECTDIR}/telemetry.o ${OBJECTDIR}/servoOut.o ${OBJECTDIR}/a2d_dma_udb4.o ${OBJECTDIR}/serialIO_udb4.o ${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o ${OBJECTDIR}/_ext/1018724808/deadReckoning.o ${OBJECTDIR}/_ext/1018724808/estWind.o ${OBJECTDIR}/_ext/1018724808/estYawDrift.o ${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o ${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o ${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o ${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o ${OBJECTDIR}/_ext/1018724808/libDCM.o ${OBJECTDIR}/_ext/1018724808/mathlibNAV.o ${OBJECTDIR}/_ext/1018724808/rmat.o ${OBJECTDIR}/_ext/1018724808/rmat_obj.o ${OBJECTDIR}/_ext/1018708451/background.o ${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o ${OBJECTDIR}/_ext/1018708451/events.o ${OBJECTDIR}/_ext/1018708451/filters.o ${OBJECTDIR}/_ext/1018708451/I2C_udb4.o ${OBJECTDIR}/_ext/1018708451/libUDB.o ${OBJECTDIR}/_ext/1018708451/magneto_udb4.o ${OBJECTDIR}/_ext/1018708451/mpu6000.o ${OBJECTDIR}/_ext/1018708451/osd.o ${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o ${OBJECTDIR}/_ext/1018708451/spiUtils.o ${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/MPMvirtualPose.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ256GP710A
MP_LINKER_FILE_OPTION=,--script=p33FJ256GP710A.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/motorCntrl.o: motorCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/motorCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  motorCntrl.c  -o ${OBJECTDIR}/motorCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/motorCntrl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/motorCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/telemetry.o: telemetry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/telemetry.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  telemetry.c  -o ${OBJECTDIR}/telemetry.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/telemetry.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/telemetry.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/servoOut.o: servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/servoOut.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  servoOut.c  -o ${OBJECTDIR}/servoOut.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/servoOut.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/servoOut.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/a2d_dma_udb4.o: a2d_dma_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/a2d_dma_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  a2d_dma_udb4.c  -o ${OBJECTDIR}/a2d_dma_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/a2d_dma_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/a2d_dma_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/serialIO_udb4.o: serialIO_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/serialIO_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  serialIO_udb4.c  -o ${OBJECTDIR}/serialIO_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/serialIO_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/serialIO_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/deadReckoning.o: ../libDCM/deadReckoning.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/deadReckoning.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/deadReckoning.c  -o ${OBJECTDIR}/_ext/1018724808/deadReckoning.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/deadReckoning.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/deadReckoning.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/estWind.o: ../libDCM/estWind.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/estWind.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/estWind.c  -o ${OBJECTDIR}/_ext/1018724808/estWind.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/estWind.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/estWind.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/estYawDrift.o: ../libDCM/estYawDrift.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/estYawDrift.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/estYawDrift.c  -o ${OBJECTDIR}/_ext/1018724808/estYawDrift.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/estYawDrift.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/estYawDrift.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o: ../libDCM/gpsParseCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/gpsParseCommon.c  -o ${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o: ../libDCM/gpsParseMTEK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/gpsParseMTEK.c  -o ${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o: ../libDCM/gpsParseSTD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/gpsParseSTD.c  -o ${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o: ../libDCM/gpsParseUBX.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/gpsParseUBX.c  -o ${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/libDCM.o: ../libDCM/libDCM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/libDCM.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/libDCM.c  -o ${OBJECTDIR}/_ext/1018724808/libDCM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/libDCM.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/libDCM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/mathlibNAV.o: ../libDCM/mathlibNAV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/mathlibNAV.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/mathlibNAV.c  -o ${OBJECTDIR}/_ext/1018724808/mathlibNAV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/mathlibNAV.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/mathlibNAV.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/rmat.o: ../libDCM/rmat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/rmat.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/rmat.c  -o ${OBJECTDIR}/_ext/1018724808/rmat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/rmat.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/rmat.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/rmat_obj.o: ../libDCM/rmat_obj.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/rmat_obj.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/rmat_obj.c  -o ${OBJECTDIR}/_ext/1018724808/rmat_obj.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/rmat_obj.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/rmat_obj.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/background.o: ../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/background.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/background.c  -o ${OBJECTDIR}/_ext/1018708451/background.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/background.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/background.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o: ../libUDB/eeprom_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/eeprom_udb4.c  -o ${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/events.o: ../libUDB/events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/events.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/events.c  -o ${OBJECTDIR}/_ext/1018708451/events.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/events.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/events.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/filters.o: ../libUDB/filters.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/filters.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/filters.c  -o ${OBJECTDIR}/_ext/1018708451/filters.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/filters.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/filters.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/I2C_udb4.o: ../libUDB/I2C_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/I2C_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/I2C_udb4.c  -o ${OBJECTDIR}/_ext/1018708451/I2C_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/I2C_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/I2C_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/libUDB.o: ../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/libUDB.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/libUDB.c  -o ${OBJECTDIR}/_ext/1018708451/libUDB.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/libUDB.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/libUDB.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/magneto_udb4.o: ../libUDB/magneto_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/magneto_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/magneto_udb4.c  -o ${OBJECTDIR}/_ext/1018708451/magneto_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/magneto_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/magneto_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/mpu6000.o: ../libUDB/mpu6000.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/mpu6000.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/mpu6000.c  -o ${OBJECTDIR}/_ext/1018708451/mpu6000.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/mpu6000.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/mpu6000.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/osd.o: ../libUDB/osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/osd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/osd.c  -o ${OBJECTDIR}/_ext/1018708451/osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/osd.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o: ../libUDB/radioIn_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/radioIn_udb4.c  -o ${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/spiUtils.o: ../libUDB/spiUtils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/spiUtils.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/spiUtils.c  -o ${OBJECTDIR}/_ext/1018708451/spiUtils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/spiUtils.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/spiUtils.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o: ../libUDB/24xxEEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/24xxEEPROM.c  -o ${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/motorCntrl.o: motorCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/motorCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  motorCntrl.c  -o ${OBJECTDIR}/motorCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/motorCntrl.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/motorCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/telemetry.o: telemetry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/telemetry.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  telemetry.c  -o ${OBJECTDIR}/telemetry.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/telemetry.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/telemetry.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/servoOut.o: servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/servoOut.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  servoOut.c  -o ${OBJECTDIR}/servoOut.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/servoOut.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/servoOut.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/a2d_dma_udb4.o: a2d_dma_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/a2d_dma_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  a2d_dma_udb4.c  -o ${OBJECTDIR}/a2d_dma_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/a2d_dma_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/a2d_dma_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/serialIO_udb4.o: serialIO_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/serialIO_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  serialIO_udb4.c  -o ${OBJECTDIR}/serialIO_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/serialIO_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/serialIO_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/deadReckoning.o: ../libDCM/deadReckoning.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/deadReckoning.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/deadReckoning.c  -o ${OBJECTDIR}/_ext/1018724808/deadReckoning.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/deadReckoning.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/deadReckoning.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/estWind.o: ../libDCM/estWind.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/estWind.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/estWind.c  -o ${OBJECTDIR}/_ext/1018724808/estWind.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/estWind.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/estWind.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/estYawDrift.o: ../libDCM/estYawDrift.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/estYawDrift.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/estYawDrift.c  -o ${OBJECTDIR}/_ext/1018724808/estYawDrift.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/estYawDrift.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/estYawDrift.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o: ../libDCM/gpsParseCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/gpsParseCommon.c  -o ${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/gpsParseCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o: ../libDCM/gpsParseMTEK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/gpsParseMTEK.c  -o ${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/gpsParseMTEK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o: ../libDCM/gpsParseSTD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/gpsParseSTD.c  -o ${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/gpsParseSTD.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o: ../libDCM/gpsParseUBX.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/gpsParseUBX.c  -o ${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/gpsParseUBX.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/libDCM.o: ../libDCM/libDCM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/libDCM.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/libDCM.c  -o ${OBJECTDIR}/_ext/1018724808/libDCM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/libDCM.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/libDCM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/mathlibNAV.o: ../libDCM/mathlibNAV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/mathlibNAV.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/mathlibNAV.c  -o ${OBJECTDIR}/_ext/1018724808/mathlibNAV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/mathlibNAV.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/mathlibNAV.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/rmat.o: ../libDCM/rmat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/rmat.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/rmat.c  -o ${OBJECTDIR}/_ext/1018724808/rmat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/rmat.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/rmat.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018724808/rmat_obj.o: ../libDCM/rmat_obj.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018724808 
	@${RM} ${OBJECTDIR}/_ext/1018724808/rmat_obj.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libDCM/rmat_obj.c  -o ${OBJECTDIR}/_ext/1018724808/rmat_obj.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018724808/rmat_obj.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018724808/rmat_obj.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/background.o: ../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/background.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/background.c  -o ${OBJECTDIR}/_ext/1018708451/background.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/background.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/background.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o: ../libUDB/eeprom_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/eeprom_udb4.c  -o ${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/eeprom_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/events.o: ../libUDB/events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/events.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/events.c  -o ${OBJECTDIR}/_ext/1018708451/events.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/events.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/events.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/filters.o: ../libUDB/filters.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/filters.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/filters.c  -o ${OBJECTDIR}/_ext/1018708451/filters.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/filters.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/filters.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/I2C_udb4.o: ../libUDB/I2C_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/I2C_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/I2C_udb4.c  -o ${OBJECTDIR}/_ext/1018708451/I2C_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/I2C_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/I2C_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/libUDB.o: ../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/libUDB.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/libUDB.c  -o ${OBJECTDIR}/_ext/1018708451/libUDB.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/libUDB.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/libUDB.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/magneto_udb4.o: ../libUDB/magneto_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/magneto_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/magneto_udb4.c  -o ${OBJECTDIR}/_ext/1018708451/magneto_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/magneto_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/magneto_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/mpu6000.o: ../libUDB/mpu6000.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/mpu6000.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/mpu6000.c  -o ${OBJECTDIR}/_ext/1018708451/mpu6000.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/mpu6000.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/mpu6000.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/osd.o: ../libUDB/osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/osd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/osd.c  -o ${OBJECTDIR}/_ext/1018708451/osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/osd.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o: ../libUDB/radioIn_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/radioIn_udb4.c  -o ${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/radioIn_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/spiUtils.o: ../libUDB/spiUtils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/spiUtils.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/spiUtils.c  -o ${OBJECTDIR}/_ext/1018708451/spiUtils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/spiUtils.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/spiUtils.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o: ../libUDB/24xxEEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../libUDB/24xxEEPROM.c  -o ${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPMvirtualPose.X" -I"../libDCM" -I"../libUDB" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/24xxEEPROM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o: ../libUDB/traps_udb4_asm.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../libUDB/traps_udb4_asm.s  -o ${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax,-al$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o: ../libUDB/traps_udb4_asm.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1018708451 
	@${RM} ${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../libUDB/traps_udb4_asm.s  -o ${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-al$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1018708451/traps_udb4_asm.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/MPMvirtualPose.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/MPMvirtualPose.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--heap=0,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library=dsp,--library=q,--library-path="../C:/Program Files/Microchip/MPLAB C30/lib",--no-force-link,--smart-io,-Map="${DISTDIR}/MatrixPilotQuad-udb4.X.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/MPMvirtualPose.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/MPMvirtualPose.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--heap=0,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library=dsp,--library=q,--library-path="../C:/Program Files/Microchip/MPLAB C30/lib",--no-force-link,--smart-io,-Map="${DISTDIR}/MatrixPilotQuad-udb4.X.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/MPMvirtualPose.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
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

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif

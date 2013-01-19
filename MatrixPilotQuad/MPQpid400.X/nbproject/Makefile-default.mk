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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MPQpid400.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/MPQpid400.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1077471063/deadReckoning.o ${OBJECTDIR}/_ext/1077471063/estWind.o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o ${OBJECTDIR}/_ext/1077471063/libDCM.o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o ${OBJECTDIR}/_ext/1077471063/rmat.o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o ${OBJECTDIR}/_ext/1077454706/background.o ${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o ${OBJECTDIR}/_ext/1077454706/libUDB.o ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o ${OBJECTDIR}/_ext/1077454706/osd.o ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o ${OBJECTDIR}/_ext/1077454706/servoOut.o ${OBJECTDIR}/_ext/1077454706/filters.o ${OBJECTDIR}/_ext/1472/motorCntrl.o ${OBJECTDIR}/_ext/1472/telemetry.o ${OBJECTDIR}/main.o ${OBJECTDIR}/_ext/1077454706/asmUtils.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d ${OBJECTDIR}/_ext/1077471063/estWind.o.d ${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d ${OBJECTDIR}/_ext/1077471063/libDCM.o.d ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d ${OBJECTDIR}/_ext/1077471063/rmat.o.d ${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o.d ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d ${OBJECTDIR}/_ext/1077454706/background.o.d ${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o.d ${OBJECTDIR}/_ext/1077454706/libUDB.o.d ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d ${OBJECTDIR}/_ext/1077454706/osd.o.d ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d ${OBJECTDIR}/_ext/1077454706/servoOut.o.d ${OBJECTDIR}/_ext/1077454706/filters.o.d ${OBJECTDIR}/_ext/1472/motorCntrl.o.d ${OBJECTDIR}/_ext/1472/telemetry.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/_ext/1077454706/asmUtils.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1077471063/deadReckoning.o ${OBJECTDIR}/_ext/1077471063/estWind.o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o ${OBJECTDIR}/_ext/1077471063/libDCM.o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o ${OBJECTDIR}/_ext/1077471063/rmat.o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o ${OBJECTDIR}/_ext/1077454706/background.o ${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o ${OBJECTDIR}/_ext/1077454706/libUDB.o ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o ${OBJECTDIR}/_ext/1077454706/osd.o ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o ${OBJECTDIR}/_ext/1077454706/servoOut.o ${OBJECTDIR}/_ext/1077454706/filters.o ${OBJECTDIR}/_ext/1472/motorCntrl.o ${OBJECTDIR}/_ext/1472/telemetry.o ${OBJECTDIR}/main.o ${OBJECTDIR}/_ext/1077454706/asmUtils.o


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/MPQpid400.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ256GP710A
MP_LINKER_FILE_OPTION=,--script=p33FJ256GP710A.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1077471063/deadReckoning.o: ../../libDCM/deadReckoning.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/deadReckoning.c  -o ${OBJECTDIR}/_ext/1077471063/deadReckoning.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estWind.o: ../../libDCM/estWind.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estWind.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estWind.c  -o ${OBJECTDIR}/_ext/1077471063/estWind.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estWind.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estWind.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estYawDrift.o: ../../libDCM/estYawDrift.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estYawDrift.c  -o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o: ../../libDCM/gpsParseCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseCommon.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o: ../../libDCM/gpsParseMTEK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseMTEK.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o: ../../libDCM/gpsParseSTD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseSTD.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o: ../../libDCM/gpsParseUBX.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseUBX.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/libDCM.o: ../../libDCM/libDCM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/libDCM.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/libDCM.c  -o ${OBJECTDIR}/_ext/1077471063/libDCM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/libDCM.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/libDCM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/mathlibNAV.o: ../../libDCM/mathlibNAV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/mathlibNAV.c  -o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/rmat.o: ../../libDCM/rmat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/rmat.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/rmat.c  -o ${OBJECTDIR}/_ext/1077471063/rmat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/rmat.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/rmat.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o: ../../libUDB/analog2digital_udb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_udb.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o: ../../libUDB/analog2digital_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/background.o: ../../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/background.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/background.c  -o ${OBJECTDIR}/_ext/1077454706/background.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/background.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/background.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o: ../../libUDB/eeprom_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/eeprom_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/libUDB.o: ../../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/libUDB.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/libUDB.c  -o ${OBJECTDIR}/_ext/1077454706/libUDB.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/libUDB.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/libUDB.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/magneto_udb4.o: ../../libUDB/magneto_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/magneto_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/osd.o: ../../libUDB/osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/osd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/osd.c  -o ${OBJECTDIR}/_ext/1077454706/osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/osd.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o: ../../libUDB/radioIn_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/radioIn_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o: ../../libUDB/serialIO_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/serialIO_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/servoOut.o: ../../libUDB/servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/servoOut.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/servoOut.c  -o ${OBJECTDIR}/_ext/1077454706/servoOut.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/servoOut.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/servoOut.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/filters.o: ../../libUDB/filters.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/filters.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/filters.c  -o ${OBJECTDIR}/_ext/1077454706/filters.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/filters.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/filters.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/motorCntrl.o: ../motorCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/motorCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../motorCntrl.c  -o ${OBJECTDIR}/_ext/1472/motorCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/motorCntrl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/motorCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/telemetry.o: ../telemetry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../telemetry.c  -o ${OBJECTDIR}/_ext/1472/telemetry.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/telemetry.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/telemetry.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1077471063/deadReckoning.o: ../../libDCM/deadReckoning.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/deadReckoning.c  -o ${OBJECTDIR}/_ext/1077471063/deadReckoning.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/deadReckoning.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estWind.o: ../../libDCM/estWind.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estWind.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estWind.c  -o ${OBJECTDIR}/_ext/1077471063/estWind.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estWind.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estWind.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/estYawDrift.o: ../../libDCM/estYawDrift.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/estYawDrift.c  -o ${OBJECTDIR}/_ext/1077471063/estYawDrift.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/estYawDrift.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o: ../../libDCM/gpsParseCommon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseCommon.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseCommon.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o: ../../libDCM/gpsParseMTEK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseMTEK.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseMTEK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o: ../../libDCM/gpsParseSTD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseSTD.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseSTD.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o: ../../libDCM/gpsParseUBX.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/gpsParseUBX.c  -o ${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/gpsParseUBX.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/libDCM.o: ../../libDCM/libDCM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/libDCM.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/libDCM.c  -o ${OBJECTDIR}/_ext/1077471063/libDCM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/libDCM.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/libDCM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/mathlibNAV.o: ../../libDCM/mathlibNAV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/mathlibNAV.c  -o ${OBJECTDIR}/_ext/1077471063/mathlibNAV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/mathlibNAV.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077471063/rmat.o: ../../libDCM/rmat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077471063 
	@${RM} ${OBJECTDIR}/_ext/1077471063/rmat.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libDCM/rmat.c  -o ${OBJECTDIR}/_ext/1077471063/rmat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077471063/rmat.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077471063/rmat.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o: ../../libUDB/analog2digital_udb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_udb.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_udb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o: ../../libUDB/analog2digital_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/analog2digital_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/analog2digital_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/background.o: ../../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/background.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/background.c  -o ${OBJECTDIR}/_ext/1077454706/background.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/background.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/background.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o: ../../libUDB/eeprom_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/eeprom_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/eeprom_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/libUDB.o: ../../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/libUDB.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/libUDB.c  -o ${OBJECTDIR}/_ext/1077454706/libUDB.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/libUDB.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/libUDB.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/magneto_udb4.o: ../../libUDB/magneto_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/magneto_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/magneto_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/magneto_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/osd.o: ../../libUDB/osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/osd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/osd.c  -o ${OBJECTDIR}/_ext/1077454706/osd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/osd.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/osd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o: ../../libUDB/radioIn_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/radioIn_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/radioIn_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o: ../../libUDB/serialIO_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/serialIO_udb4.c  -o ${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/serialIO_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/servoOut.o: ../../libUDB/servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/servoOut.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/servoOut.c  -o ${OBJECTDIR}/_ext/1077454706/servoOut.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/servoOut.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/servoOut.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1077454706/filters.o: ../../libUDB/filters.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/filters.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libUDB/filters.c  -o ${OBJECTDIR}/_ext/1077454706/filters.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1077454706/filters.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/filters.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/motorCntrl.o: ../motorCntrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/motorCntrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../motorCntrl.c  -o ${OBJECTDIR}/_ext/1472/motorCntrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/motorCntrl.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/motorCntrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/telemetry.o: ../telemetry.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/telemetry.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../telemetry.c  -o ${OBJECTDIR}/_ext/1472/telemetry.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/telemetry.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/telemetry.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"        -g -omf=elf -legacy-libc -mlarge-code -O0 -I"../MPQpid400.X" -msmart-io=1 -Wall -msfr-warn=off --save-temps  -D_ADDED_C_LIB
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1077454706/asmUtils.o: ../../libUDB/asmUtils.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/asmUtils.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libUDB/asmUtils.s  -o ${OBJECTDIR}/_ext/1077454706/asmUtils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1077454706/asmUtils.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax,-al$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/asmUtils.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1077454706/asmUtils.o: ../../libUDB/asmUtils.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1077454706 
	@${RM} ${OBJECTDIR}/_ext/1077454706/asmUtils.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../libUDB/asmUtils.s  -o ${OBJECTDIR}/_ext/1077454706/asmUtils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -I".." -Wa,-MD,"${OBJECTDIR}/_ext/1077454706/asmUtils.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-al$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1077454706/asmUtils.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/MPQpid400.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/MPQpid400.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library=dsp,--library=q,--library-path="../C:/Program Files/Microchip/MPLAB C30/lib",--no-force-link,--smart-io,-Map="${DISTDIR}/MatrixPilotQuad-udb4.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/MPQpid400.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/MPQpid400.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library=dsp,--library=q,--library-path="../C:/Program Files/Microchip/MPLAB C30/lib",--no-force-link,--smart-io,-Map="${DISTDIR}/MatrixPilotQuad-udb4.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/MPQpid400.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
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

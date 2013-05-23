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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/LedTest-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/LedTest-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/io_test_auav3.o ${OBJECTDIR}/_ext/1159519357/background.o ${OBJECTDIR}/_ext/1159519357/libUDB.o ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o ${OBJECTDIR}/_ext/1159519357/servoOut.o ${OBJECTDIR}/_ext/1159519357/mpu6000.o ${OBJECTDIR}/_ext/1159519357/spiUtils.o ${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o ${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o ${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1472/io_test_auav3.o.d ${OBJECTDIR}/_ext/1159519357/background.o.d ${OBJECTDIR}/_ext/1159519357/libUDB.o.d ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d ${OBJECTDIR}/_ext/1159519357/servoOut.o.d ${OBJECTDIR}/_ext/1159519357/mpu6000.o.d ${OBJECTDIR}/_ext/1159519357/spiUtils.o.d ${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o.d ${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o.d ${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o.d ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/io_test_auav3.o ${OBJECTDIR}/_ext/1159519357/background.o ${OBJECTDIR}/_ext/1159519357/libUDB.o ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o ${OBJECTDIR}/_ext/1159519357/servoOut.o ${OBJECTDIR}/_ext/1159519357/mpu6000.o ${OBJECTDIR}/_ext/1159519357/spiUtils.o ${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o ${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o ${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/LedTest-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP512MU810
MP_LINKER_FILE_OPTION=,--script=p33EP512MU810.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main.c  -o ${OBJECTDIR}/_ext/1472/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/io_test_auav3.o: ../io_test_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/io_test_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../io_test_auav3.c  -o ${OBJECTDIR}/_ext/1472/io_test_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/io_test_auav3.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/io_test_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/background.o: ../../../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/background.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/background.c  -o ${OBJECTDIR}/_ext/1159519357/background.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/background.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/background.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/libUDB.o: ../../../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/libUDB.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/libUDB.c  -o ${OBJECTDIR}/_ext/1159519357/libUDB.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/libUDB.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/libUDB.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/magneto_udb4.o: ../../../libUDB/magneto_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/magneto_udb4.c  -o ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/servoOut.o: ../../../libUDB/servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/servoOut.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/servoOut.c  -o ${OBJECTDIR}/_ext/1159519357/servoOut.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/servoOut.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/servoOut.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/mpu6000.o: ../../../libUDB/mpu6000.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/mpu6000.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/mpu6000.c  -o ${OBJECTDIR}/_ext/1159519357/mpu6000.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/mpu6000.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/mpu6000.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/spiUtils.o: ../../../libUDB/spiUtils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/spiUtils.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/spiUtils.c  -o ${OBJECTDIR}/_ext/1159519357/spiUtils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/spiUtils.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/spiUtils.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o: ../../../libUDB/radioIn_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/radioIn_auav3.c  -o ${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o: ../../../libUDB/eeprom_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/eeprom_auav3.c  -o ${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o: ../../../libUDB/analog2digital_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/analog2digital_auav3.c  -o ${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o: ../../../libUDB/serialIO_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/serialIO_udb4.c  -o ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../main.c  -o ${OBJECTDIR}/_ext/1472/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/io_test_auav3.o: ../io_test_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/io_test_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../io_test_auav3.c  -o ${OBJECTDIR}/_ext/1472/io_test_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/io_test_auav3.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/io_test_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/background.o: ../../../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/background.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/background.c  -o ${OBJECTDIR}/_ext/1159519357/background.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/background.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/background.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/libUDB.o: ../../../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/libUDB.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/libUDB.c  -o ${OBJECTDIR}/_ext/1159519357/libUDB.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/libUDB.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/libUDB.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/magneto_udb4.o: ../../../libUDB/magneto_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/magneto_udb4.c  -o ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/servoOut.o: ../../../libUDB/servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/servoOut.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/servoOut.c  -o ${OBJECTDIR}/_ext/1159519357/servoOut.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/servoOut.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/servoOut.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/mpu6000.o: ../../../libUDB/mpu6000.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/mpu6000.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/mpu6000.c  -o ${OBJECTDIR}/_ext/1159519357/mpu6000.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/mpu6000.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/mpu6000.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/spiUtils.o: ../../../libUDB/spiUtils.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/spiUtils.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/spiUtils.c  -o ${OBJECTDIR}/_ext/1159519357/spiUtils.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/spiUtils.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/spiUtils.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o: ../../../libUDB/radioIn_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/radioIn_auav3.c  -o ${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/radioIn_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o: ../../../libUDB/eeprom_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/eeprom_auav3.c  -o ${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/eeprom_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o: ../../../libUDB/analog2digital_auav3.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/analog2digital_auav3.c  -o ${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/analog2digital_auav3.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o: ../../../libUDB/serialIO_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../libUDB/serialIO_udb4.c  -o ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d"      -g -omf=elf -legacy-libc -mlarge-data -msmall-scalar -O0 -I"../../../libUDB" -I"../LedTest-auav3" -mcci -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/LedTest-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/LedTest-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--heap=256,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="../C:/Program Files/Microchip/MPLAB C30/lib",--no-force-link,--smart-io,-Map="${DISTDIR}/LedTest-udb5.X.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/LedTest-auav3.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/LedTest-auav3.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--heap=256,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library-path="../C:/Program Files/Microchip/MPLAB C30/lib",--no-force-link,--smart-io,-Map="${DISTDIR}/LedTest-udb5.X.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/LedTest-auav3.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
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

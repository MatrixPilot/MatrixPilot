#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile

# Environment
# Adding MPLAB X bin directory to path
PATH:=/opt/microchip/mplabxv1.00a/mplab_ide/mplab_ide/modules/../../bin/:$(PATH)
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/FlashOSD-udb4.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/FlashOSD-udb4.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1159519357/background.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1159519357/libUDB.o ${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o ${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o ${OBJECTDIR}/_ext/1159519357/servoOut.o ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o ${OBJECTDIR}/_ext/1159519357/osd.o ${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1159519357/background.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1159519357/libUDB.o.d ${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o.d ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d ${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o.d ${OBJECTDIR}/_ext/1159519357/servoOut.o.d ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d ${OBJECTDIR}/_ext/1159519357/osd.o.d ${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1159519357/background.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1159519357/libUDB.o ${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o ${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o ${OBJECTDIR}/_ext/1159519357/servoOut.o ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o ${OBJECTDIR}/_ext/1159519357/osd.o ${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH="/usr/lib/jvm/java-6-sun-1.6.0.26/jre/bin/"
OS_CURRENT="$(shell uname -s)"
############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
MP_CC="/opt/microchip/mplabc30/v3.30c/bin/pic30-gcc"
# MP_BC is not defined
MP_AS="/opt/microchip/mplabc30/v3.30c/bin/pic30-as"
MP_LD="/opt/microchip/mplabc30/v3.30c/bin/pic30-ld"
MP_AR="/opt/microchip/mplabc30/v3.30c/bin/pic30-ar"
DEP_GEN=${MP_JAVA_PATH}java -jar "/opt/microchip/mplabxv1.00a/mplab_ide/mplab_ide/modules/../../bin/extractobjectdependencies.jar" 
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps
MP_CC_DIR="/opt/microchip/mplabc30/v3.30c/bin"
# MP_BC_DIR is not defined
MP_AS_DIR="/opt/microchip/mplabc30/v3.30c/bin"
MP_LD_DIR="/opt/microchip/mplabc30/v3.30c/bin"
MP_AR_DIR="/opt/microchip/mplabc30/v3.30c/bin"
# MP_BC_DIR is not defined

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/FlashOSD-udb4.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ256GP710A
MP_LINKER_FILE_OPTION=,-Tp33FJ256GP710A.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1159519357/background.o: ../../../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/background.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/background.o.ok ${OBJECTDIR}/_ext/1159519357/background.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/background.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/background.o.d" -o ${OBJECTDIR}/_ext/1159519357/background.o ../../../libUDB/background.c  -legacy-libc
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/libUDB.o: ../../../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/libUDB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/libUDB.o.ok ${OBJECTDIR}/_ext/1159519357/libUDB.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/libUDB.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/libUDB.o.d" -o ${OBJECTDIR}/_ext/1159519357/libUDB.o ../../../libUDB/libUDB.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o: ../../../libUDB/analog2digital_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o.ok ${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o.d" -o ${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o ../../../libUDB/analog2digital_udb4.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/magneto_udb4.o: ../../../libUDB/magneto_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.ok ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d" -o ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o ../../../libUDB/magneto_udb4.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o: ../../../libUDB/radioIn_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o.ok ${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o.d" -o ${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o ../../../libUDB/radioIn_udb4.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/servoOut.o: ../../../libUDB/servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/servoOut.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/servoOut.o.ok ${OBJECTDIR}/_ext/1159519357/servoOut.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/servoOut.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/servoOut.o.d" -o ${OBJECTDIR}/_ext/1159519357/servoOut.o ../../../libUDB/servoOut.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o: ../../../libUDB/serialIO_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.ok ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d" -o ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o ../../../libUDB/serialIO_udb4.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/osd.o: ../../../libUDB/osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/osd.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/osd.o.ok ${OBJECTDIR}/_ext/1159519357/osd.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/osd.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/osd.o.d" -o ${OBJECTDIR}/_ext/1159519357/osd.o ../../../libUDB/osd.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o: ../../../libUDB/eeprom_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o.ok ${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o.d" -o ${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o ../../../libUDB/eeprom_udb4.c  -legacy-libc
	
else
${OBJECTDIR}/_ext/1159519357/background.o: ../../../libUDB/background.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/background.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/background.o.ok ${OBJECTDIR}/_ext/1159519357/background.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/background.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/background.o.d" -o ${OBJECTDIR}/_ext/1159519357/background.o ../../../libUDB/background.c  -legacy-libc
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/libUDB.o: ../../../libUDB/libUDB.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/libUDB.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/libUDB.o.ok ${OBJECTDIR}/_ext/1159519357/libUDB.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/libUDB.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/libUDB.o.d" -o ${OBJECTDIR}/_ext/1159519357/libUDB.o ../../../libUDB/libUDB.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o: ../../../libUDB/analog2digital_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o.ok ${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o.d" -o ${OBJECTDIR}/_ext/1159519357/analog2digital_udb4.o ../../../libUDB/analog2digital_udb4.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/magneto_udb4.o: ../../../libUDB/magneto_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.ok ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/magneto_udb4.o.d" -o ${OBJECTDIR}/_ext/1159519357/magneto_udb4.o ../../../libUDB/magneto_udb4.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o: ../../../libUDB/radioIn_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o.ok ${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o.d" -o ${OBJECTDIR}/_ext/1159519357/radioIn_udb4.o ../../../libUDB/radioIn_udb4.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/servoOut.o: ../../../libUDB/servoOut.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/servoOut.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/servoOut.o.ok ${OBJECTDIR}/_ext/1159519357/servoOut.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/servoOut.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/servoOut.o.d" -o ${OBJECTDIR}/_ext/1159519357/servoOut.o ../../../libUDB/servoOut.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o: ../../../libUDB/serialIO_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.ok ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o.d" -o ${OBJECTDIR}/_ext/1159519357/serialIO_udb4.o ../../../libUDB/serialIO_udb4.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/osd.o: ../../../libUDB/osd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/osd.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/osd.o.ok ${OBJECTDIR}/_ext/1159519357/osd.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/osd.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/osd.o.d" -o ${OBJECTDIR}/_ext/1159519357/osd.o ../../../libUDB/osd.c  -legacy-libc
	
${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o: ../../../libUDB/eeprom_udb4.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1159519357 
	@${RM} ${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o.d 
	@${RM} ${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o.ok ${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=coff -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I".." -I"../../../libUDB" -MMD -MF "${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o.d" -o ${OBJECTDIR}/_ext/1159519357/eeprom_udb4.o ../../../libUDB/eeprom_udb4.c  -legacy-libc
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/FlashOSD-udb4.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/FlashOSD-udb4.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}       -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,-L"../C:/Program Files/Microchip/MPLAB C30/lib",-Map="${DISTDIR}/FlashOSD-udb4.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/FlashOSD-udb4.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=coff -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/FlashOSD-udb4.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}       -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,-L"../C:/Program Files/Microchip/MPLAB C30/lib",-Map="${DISTDIR}/FlashOSD-udb4.X.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}/pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/FlashOSD-udb4.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=coff
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "/opt/microchip/mplabxv1.00a/mplab_ide/mplab_ide/modules/../../bin/"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif

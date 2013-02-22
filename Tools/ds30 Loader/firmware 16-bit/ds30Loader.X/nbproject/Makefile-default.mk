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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ds30Loader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ds30Loader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360937237/ds30loader.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360937237/ds30loader.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360937237/ds30loader.o


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/ds30Loader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ256GP710A
MP_LINKER_FILE_OPTION=--script="C:\Program Files (x86)\Microchip\MPLAB ASM30 Suite\Support\dsPIC33F\gld\p33FJ256GP710A.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360937237/ds30loader.o: ../src/ds30loader.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ds30loader.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ds30loader.o.ok ${OBJECTDIR}/_ext/1360937237/ds30loader.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/ds30loader.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_AS} $(MP_EXTRA_AS_PRE)  ../src/ds30loader.s -o ${OBJECTDIR}/_ext/1360937237/ds30loader.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__DEBUG=1 --defsym=__MPLAB_DEBUGGER_PK3=1 -g -I"../src" -MD "${OBJECTDIR}/_ext/1360937237/ds30loader.o.d" 
	
else
${OBJECTDIR}/_ext/1360937237/ds30loader.o: ../src/ds30loader.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360937237 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ds30loader.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/ds30loader.o.ok ${OBJECTDIR}/_ext/1360937237/ds30loader.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/ds30loader.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_AS} $(MP_EXTRA_AS_PRE)  ../src/ds30loader.s -o ${OBJECTDIR}/_ext/1360937237/ds30loader.o -omf=elf -p=$(MP_PROCESSOR_OPTION) -I"../src" -MD "${OBJECTDIR}/_ext/1360937237/ds30loader.o.d" 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/ds30Loader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    C:/Program\ Files\ (x86)/Microchip/MPLAB\ ASM30\ Suite/Support/dsPIC33F/gld/p33FJ256GP710A.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)  ${OBJECTFILES_QUOTED_IF_SPACED}    -omf=elf $(MP_LINKER_FILE_OPTION) --defsym=__MPLAB_DEBUG=1 --defsym=__MPLAB_DEBUGGER_PK3=1 --defsym=__ICD2RAM=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/ds30Loader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} 
else
dist/${CND_CONF}/${IMAGE_TYPE}/ds30Loader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   C:/Program\ Files\ (x86)/Microchip/MPLAB\ ASM30\ Suite/Support/dsPIC33F/gld/p33FJ256GP710A.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)  ${OBJECTFILES_QUOTED_IF_SPACED}    -omf=elf $(MP_LINKER_FILE_OPTION) --defsym=__MPLAB_DEBUG=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/ds30Loader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
	${MP_AS_DIR}\\pic30-bin2hex  dist/${CND_CONF}/${IMAGE_TYPE}/ds30Loader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
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

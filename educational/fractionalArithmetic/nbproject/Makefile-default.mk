#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/fractionalArithmetic.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/fractionalArithmetic.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/main.o ${OBJECTDIR}/traps.o ${OBJECTDIR}/addSat.o ${OBJECTDIR}/_ext/1258608875/mathlibNAV.o
POSSIBLE_DEPFILES=${OBJECTDIR}/main.o.d ${OBJECTDIR}/traps.o.d ${OBJECTDIR}/addSat.o.d ${OBJECTDIR}/_ext/1258608875/mathlibNAV.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/main.o ${OBJECTDIR}/traps.o ${OBJECTDIR}/addSat.o ${OBJECTDIR}/_ext/1258608875/mathlibNAV.o


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/fractionalArithmetic.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ256GP710A
MP_LINKER_FILE_OPTION=,--script=p33FJ256GP710A.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"        -g -D__DEBUG   -omf=elf -legacy-libc -O0 -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libDCM" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad/MPQ6000.X" -msmart-io=1 -msfr-warn=off --save-temps
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) 
	
${OBJECTDIR}/traps.o: traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/traps.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  traps.c  -o ${OBJECTDIR}/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/traps.o.d"        -g -D__DEBUG   -omf=elf -legacy-libc -O0 -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libDCM" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad/MPQ6000.X" -msmart-io=1 -msfr-warn=off --save-temps
	@${FIXDEPS} "${OBJECTDIR}/traps.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1258608875/mathlibNAV.o: ../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libDCM/mathlibNAV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1258608875 
	@${RM} ${OBJECTDIR}/_ext/1258608875/mathlibNAV.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libDCM/mathlibNAV.c  -o ${OBJECTDIR}/_ext/1258608875/mathlibNAV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1258608875/mathlibNAV.o.d"        -g -D__DEBUG   -omf=elf -legacy-libc -O0 -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libDCM" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad/MPQ6000.X" -msmart-io=1 -msfr-warn=off --save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1258608875/mathlibNAV.o.d" $(SILENT) 
	
else
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"        -g -omf=elf -legacy-libc -O0 -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libDCM" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad/MPQ6000.X" -msmart-io=1 -msfr-warn=off --save-temps
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) 
	
${OBJECTDIR}/traps.o: traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/traps.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  traps.c  -o ${OBJECTDIR}/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/traps.o.d"        -g -omf=elf -legacy-libc -O0 -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libDCM" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad/MPQ6000.X" -msmart-io=1 -msfr-warn=off --save-temps
	@${FIXDEPS} "${OBJECTDIR}/traps.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1258608875/mathlibNAV.o: ../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libDCM/mathlibNAV.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1258608875 
	@${RM} ${OBJECTDIR}/_ext/1258608875/mathlibNAV.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libDCM/mathlibNAV.c  -o ${OBJECTDIR}/_ext/1258608875/mathlibNAV.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1258608875/mathlibNAV.o.d"        -g -omf=elf -legacy-libc -O0 -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libDCM" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad" -I"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/MatrixPilotQuad/MPQ6000.X" -msmart-io=1 -msfr-warn=off --save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1258608875/mathlibNAV.o.d" $(SILENT) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/addSat.o: addSat.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/addSat.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  addSat.s  -o ${OBJECTDIR}/addSat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG   -omf=elf -legacy-libc -Wa,-MD,"${OBJECTDIR}/addSat.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/addSat.o.d"  $(SILENT) 
	
else
${OBJECTDIR}/addSat.o: addSat.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/addSat.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  addSat.s  -o ${OBJECTDIR}/addSat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -legacy-libc -Wa,-MD,"${OBJECTDIR}/addSat.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/addSat.o.d"  $(SILENT) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/fractionalArithmetic.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/fractionalArithmetic.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG   -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library=q-dsp,--no-force-link,--smart-io,--report-mem,--cref$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/fractionalArithmetic.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/fractionalArithmetic.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -legacy-libc -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library=q-dsp,--no-force-link,--smart-io,--report-mem,--cref$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/fractionalArithmetic.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
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

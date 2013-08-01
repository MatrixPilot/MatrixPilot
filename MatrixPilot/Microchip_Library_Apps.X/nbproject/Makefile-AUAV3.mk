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
ifeq "$(wildcard nbproject/Makefile-local-AUAV3.mk)" "nbproject/Makefile-local-AUAV3.mk"
include nbproject/Makefile-local-AUAV3.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=AUAV3
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=a
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Microchip_Library_Apps.X.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=a
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Microchip_Library_Apps.X.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../libFlashFS/MDD_AT45D.c ../../libFlashFS/usb_msd.c ../../libFlashFS/AT45D.c ../../libFlashFS/usb.c ../../libFlashFS/minIni.c ../../libFlashFS/usb_descriptors.c ../../libFlashFS/AT45D_FS.c ../../libFlashFS/usb_cdc.c ../../libFlashFS/AT45D_DMA.c "../../Microchip/MDD File System/FSIO.c" "../../Microchip/USB/CDC Device Driver/usb_function_cdc.c" "../../Microchip/USB/MSD Device Driver/usb_function_msd.c" ../../Microchip/USB/usb_device.c ../../Microchip/USB/usb_hal_dspic33e.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o ${OBJECTDIR}/_ext/1864667112/usb_msd.o ${OBJECTDIR}/_ext/1864667112/AT45D.o ${OBJECTDIR}/_ext/1864667112/usb.o ${OBJECTDIR}/_ext/1864667112/minIni.o ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o ${OBJECTDIR}/_ext/1864667112/AT45D_FS.o ${OBJECTDIR}/_ext/1864667112/usb_cdc.o ${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o ${OBJECTDIR}/_ext/65885567/FSIO.o ${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o ${OBJECTDIR}/_ext/41277818/usb_function_msd.o ${OBJECTDIR}/_ext/713068069/usb_device.o ${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d ${OBJECTDIR}/_ext/1864667112/usb_msd.o.d ${OBJECTDIR}/_ext/1864667112/AT45D.o.d ${OBJECTDIR}/_ext/1864667112/usb.o.d ${OBJECTDIR}/_ext/1864667112/minIni.o.d ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d ${OBJECTDIR}/_ext/1864667112/AT45D_FS.o.d ${OBJECTDIR}/_ext/1864667112/usb_cdc.o.d ${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o.d ${OBJECTDIR}/_ext/65885567/FSIO.o.d ${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o.d ${OBJECTDIR}/_ext/41277818/usb_function_msd.o.d ${OBJECTDIR}/_ext/713068069/usb_device.o.d ${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o ${OBJECTDIR}/_ext/1864667112/usb_msd.o ${OBJECTDIR}/_ext/1864667112/AT45D.o ${OBJECTDIR}/_ext/1864667112/usb.o ${OBJECTDIR}/_ext/1864667112/minIni.o ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o ${OBJECTDIR}/_ext/1864667112/AT45D_FS.o ${OBJECTDIR}/_ext/1864667112/usb_cdc.o ${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o ${OBJECTDIR}/_ext/65885567/FSIO.o ${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o ${OBJECTDIR}/_ext/41277818/usb_function_msd.o ${OBJECTDIR}/_ext/713068069/usb_device.o ${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o

# Source Files
SOURCEFILES=../../libFlashFS/MDD_AT45D.c ../../libFlashFS/usb_msd.c ../../libFlashFS/AT45D.c ../../libFlashFS/usb.c ../../libFlashFS/minIni.c ../../libFlashFS/usb_descriptors.c ../../libFlashFS/AT45D_FS.c ../../libFlashFS/usb_cdc.c ../../libFlashFS/AT45D_DMA.c ../../Microchip/MDD File System/FSIO.c ../../Microchip/USB/CDC Device Driver/usb_function_cdc.c ../../Microchip/USB/MSD Device Driver/usb_function_msd.c ../../Microchip/USB/usb_device.c ../../Microchip/USB/usb_hal_dspic33e.c


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
	${MAKE}  -f nbproject/Makefile-AUAV3.mk dist/${CND_CONF}/${IMAGE_TYPE}/Microchip_Library_Apps.X.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP512MU810
MP_LINKER_FILE_OPTION=,--script=p33EP512MU810.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o: ../../libFlashFS/MDD_AT45D.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/MDD_AT45D.c  -o ${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_msd.o: ../../libFlashFS/usb_msd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_msd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_msd.c  -o ${OBJECTDIR}/_ext/1864667112/usb_msd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_msd.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_msd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/AT45D.o: ../../libFlashFS/AT45D.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/AT45D.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/AT45D.c  -o ${OBJECTDIR}/_ext/1864667112/AT45D.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/AT45D.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/AT45D.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb.o: ../../libFlashFS/usb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb.c  -o ${OBJECTDIR}/_ext/1864667112/usb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/minIni.o: ../../libFlashFS/minIni.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/minIni.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/minIni.c  -o ${OBJECTDIR}/_ext/1864667112/minIni.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/minIni.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/minIni.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_descriptors.o: ../../libFlashFS/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_descriptors.c  -o ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/AT45D_FS.o: ../../libFlashFS/AT45D_FS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/AT45D_FS.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/AT45D_FS.c  -o ${OBJECTDIR}/_ext/1864667112/AT45D_FS.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/AT45D_FS.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/AT45D_FS.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_cdc.o: ../../libFlashFS/usb_cdc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_cdc.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_cdc.c  -o ${OBJECTDIR}/_ext/1864667112/usb_cdc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_cdc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_cdc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o: ../../libFlashFS/AT45D_DMA.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/AT45D_DMA.c  -o ${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/65885567/FSIO.o: ../../Microchip/MDD\ File\ System/FSIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/65885567 
	@${RM} ${OBJECTDIR}/_ext/65885567/FSIO.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "../../Microchip/MDD File System/FSIO.c"  -o ${OBJECTDIR}/_ext/65885567/FSIO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/65885567/FSIO.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/65885567/FSIO.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o: ../../Microchip/USB/CDC\ Device\ Driver/usb_function_cdc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1166020650 
	@${RM} ${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "../../Microchip/USB/CDC Device Driver/usb_function_cdc.c"  -o ${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/41277818/usb_function_msd.o: ../../Microchip/USB/MSD\ Device\ Driver/usb_function_msd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/41277818 
	@${RM} ${OBJECTDIR}/_ext/41277818/usb_function_msd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "../../Microchip/USB/MSD Device Driver/usb_function_msd.c"  -o ${OBJECTDIR}/_ext/41277818/usb_function_msd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/41277818/usb_function_msd.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/41277818/usb_function_msd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/713068069/usb_device.o: ../../Microchip/USB/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/713068069 
	@${RM} ${OBJECTDIR}/_ext/713068069/usb_device.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../Microchip/USB/usb_device.c  -o ${OBJECTDIR}/_ext/713068069/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/713068069/usb_device.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/713068069/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o: ../../Microchip/USB/usb_hal_dspic33e.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/713068069 
	@${RM} ${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../Microchip/USB/usb_hal_dspic33e.c  -o ${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o: ../../libFlashFS/MDD_AT45D.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/MDD_AT45D.c  -o ${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/MDD_AT45D.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_msd.o: ../../libFlashFS/usb_msd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_msd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_msd.c  -o ${OBJECTDIR}/_ext/1864667112/usb_msd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_msd.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_msd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/AT45D.o: ../../libFlashFS/AT45D.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/AT45D.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/AT45D.c  -o ${OBJECTDIR}/_ext/1864667112/AT45D.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/AT45D.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/AT45D.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb.o: ../../libFlashFS/usb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb.c  -o ${OBJECTDIR}/_ext/1864667112/usb.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/minIni.o: ../../libFlashFS/minIni.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/minIni.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/minIni.c  -o ${OBJECTDIR}/_ext/1864667112/minIni.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/minIni.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/minIni.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_descriptors.o: ../../libFlashFS/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_descriptors.c  -o ${OBJECTDIR}/_ext/1864667112/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_descriptors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/AT45D_FS.o: ../../libFlashFS/AT45D_FS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/AT45D_FS.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/AT45D_FS.c  -o ${OBJECTDIR}/_ext/1864667112/AT45D_FS.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/AT45D_FS.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/AT45D_FS.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/usb_cdc.o: ../../libFlashFS/usb_cdc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/usb_cdc.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/usb_cdc.c  -o ${OBJECTDIR}/_ext/1864667112/usb_cdc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/usb_cdc.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/usb_cdc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o: ../../libFlashFS/AT45D_DMA.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1864667112 
	@${RM} ${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../libFlashFS/AT45D_DMA.c  -o ${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1864667112/AT45D_DMA.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/65885567/FSIO.o: ../../Microchip/MDD\ File\ System/FSIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/65885567 
	@${RM} ${OBJECTDIR}/_ext/65885567/FSIO.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "../../Microchip/MDD File System/FSIO.c"  -o ${OBJECTDIR}/_ext/65885567/FSIO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/65885567/FSIO.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/65885567/FSIO.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o: ../../Microchip/USB/CDC\ Device\ Driver/usb_function_cdc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1166020650 
	@${RM} ${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "../../Microchip/USB/CDC Device Driver/usb_function_cdc.c"  -o ${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1166020650/usb_function_cdc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/41277818/usb_function_msd.o: ../../Microchip/USB/MSD\ Device\ Driver/usb_function_msd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/41277818 
	@${RM} ${OBJECTDIR}/_ext/41277818/usb_function_msd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  "../../Microchip/USB/MSD Device Driver/usb_function_msd.c"  -o ${OBJECTDIR}/_ext/41277818/usb_function_msd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/41277818/usb_function_msd.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/41277818/usb_function_msd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/713068069/usb_device.o: ../../Microchip/USB/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/713068069 
	@${RM} ${OBJECTDIR}/_ext/713068069/usb_device.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../Microchip/USB/usb_device.c  -o ${OBJECTDIR}/_ext/713068069/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/713068069/usb_device.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/713068069/usb_device.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o: ../../Microchip/USB/usb_hal_dspic33e.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/713068069 
	@${RM} ${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../Microchip/USB/usb_hal_dspic33e.c  -o ${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o.d"      -g -omf=elf -legacy-libc -mlarge-code -mlarge-data -mconst-in-code -O0 -I"../../libUDB" -I"../../MatrixPilot" -I"../../libFlashFS" -I"../../Microchip/Include" -I"../../Microchip/Include/MDD File System" -I"../../Microchip/Include/USB" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/713068069/usb_hal_dspic33e.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
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
# Rules for buildStep: archive
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Microchip_Library_Apps.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/Microchip_Library_Apps.X.${OUTPUT_SUFFIX} 
	${MP_AR} $(MP_EXTRA_AR_PRE)  -omf=elf -r dist/${CND_CONF}/${IMAGE_TYPE}/Microchip_Library_Apps.X.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Microchip_Library_Apps.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/Microchip_Library_Apps.X.${OUTPUT_SUFFIX} 
	${MP_AR} $(MP_EXTRA_AR_PRE)  -omf=elf -r dist/${CND_CONF}/${IMAGE_TYPE}/Microchip_Library_Apps.X.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/AUAV3
	${RM} -r dist/AUAV3

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif

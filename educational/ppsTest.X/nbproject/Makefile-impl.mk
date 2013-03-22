#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a pre- and a post- target defined where you can add customization code.
#
# This makefile implements macros and targets common to all configurations.
#
# NOCDDL


# Building and Cleaning subprojects are done by default, but can be controlled with the SUB
# macro. If SUB=no, subprojects will not be built or cleaned. The following macro
# statements set BUILD_SUB-CONF and CLEAN_SUB-CONF to .build-reqprojects-conf
# and .clean-reqprojects-conf unless SUB has the value 'no'
SUB_no=NO
SUBPROJECTS=${SUB_${SUB}}
BUILD_SUBPROJECTS_=.build-subprojects
BUILD_SUBPROJECTS_NO=
BUILD_SUBPROJECTS=${BUILD_SUBPROJECTS_${SUBPROJECTS}}
CLEAN_SUBPROJECTS_=.clean-subprojects
CLEAN_SUBPROJECTS_NO=
CLEAN_SUBPROJECTS=${CLEAN_SUBPROJECTS_${SUBPROJECTS}}


# Project Name
PROJECTNAME=ppsTest.X

# Active Configuration
DEFAULTCONF=C30_dsPIC33FJ256GP710A
CONF=${DEFAULTCONF}

# All Configurations
ALLCONFS=C30_dsPIC33FJ256GP710A XC16_dsPIC33FJ256GP710A C30_dsPIC33FJ128MC804 XC16_dsPIC33FJ128MC804 C30_dsPIC33FJ16GS504 XC16_dsPIC33FJ16GS504 C30_dsPIC33FJ06GS101 XC16_dsPIC33FJ06GS101 C30_dsPIC33EP512MU810 XC16_dsPIC33EP512MU810 


# build
.build-impl: .build-pre
	${MAKE} -f nbproject/Makefile-${CONF}.mk SUBPROJECTS=${SUBPROJECTS} .build-conf


# clean
.clean-impl: .clean-pre
	${MAKE} -f nbproject/Makefile-${CONF}.mk SUBPROJECTS=${SUBPROJECTS} .clean-conf

# clobber
.clobber-impl: .clobber-pre .depcheck-impl
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=C30_dsPIC33FJ256GP710A clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=XC16_dsPIC33FJ256GP710A clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=C30_dsPIC33FJ128MC804 clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=XC16_dsPIC33FJ128MC804 clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=C30_dsPIC33FJ16GS504 clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=XC16_dsPIC33FJ16GS504 clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=C30_dsPIC33FJ06GS101 clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=XC16_dsPIC33FJ06GS101 clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=C30_dsPIC33EP512MU810 clean
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=XC16_dsPIC33EP512MU810 clean



# all
.all-impl: .all-pre .depcheck-impl
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=C30_dsPIC33FJ256GP710A build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=XC16_dsPIC33FJ256GP710A build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=C30_dsPIC33FJ128MC804 build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=XC16_dsPIC33FJ128MC804 build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=C30_dsPIC33FJ16GS504 build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=XC16_dsPIC33FJ16GS504 build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=C30_dsPIC33FJ06GS101 build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=XC16_dsPIC33FJ06GS101 build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=C30_dsPIC33EP512MU810 build
	    ${MAKE} SUBPROJECTS=${SUBPROJECTS} CONF=XC16_dsPIC33EP512MU810 build



# dependency checking support
.depcheck-impl:
#	@echo "# This code depends on make tool being used" >.dep.inc
#	@if [ -n "${MAKE_VERSION}" ]; then \
#	    echo "DEPFILES=\$$(wildcard \$$(addsuffix .d, \$${OBJECTFILES}))" >>.dep.inc; \
#	    echo "ifneq (\$${DEPFILES},)" >>.dep.inc; \
#	    echo "include \$${DEPFILES}" >>.dep.inc; \
#	    echo "endif" >>.dep.inc; \
#	else \
#	    echo ".KEEP_STATE:" >>.dep.inc; \
#	    echo ".KEEP_STATE_FILE:.make.state.\$${CONF}" >>.dep.inc; \
#	fi

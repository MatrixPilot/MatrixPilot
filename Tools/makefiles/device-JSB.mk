# this file is included from makefile

TOOLCHAIN := GCC
ifeq ($(OS),Windows_NT)
TARGET_TYPE := exe
else
TARGET_TYPE := out
endif
CPU :=

modules += Tools/MatrixPilot-JSB Tools/JSB-SIL

incpath += Tools/JSBSim/src
#incpath += ../jsbsim/src

#cfgpath := Config
addlibs := ../Tools/JSBSimLib/Debug/JSBSimLib.lib
defines += JSB_SIL

lflags := Tools/JSBSim
libs := JSBSim

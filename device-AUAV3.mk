# this file is included from makefile

#TOOLCHAIN ?= C30
TOOLCHAIN ?= XC16
TARGET_TYPE := hex
CPU := 33EP512MU810

modules += libUDB libFlashFS libVectorMatrix Microchip
incpath += Microchip Microchip/Include libVectorMatrix
#cfgpath := Config

#defines += USE_USB USE_CDC USE_MSD USE_FILESYS

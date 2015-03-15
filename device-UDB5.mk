# this file is included from makefile

TOOLCHAIN ?= XC16
TARGET_TYPE := hex
CPU := 33FJ256GP710A

#modules += libUDB libFlashFS libVectorMatrix Microchip
modules += libUDB libVectorMatrix
incpath += Microchip Microchip/Include libVectorMatrix
#cfgpath := Config

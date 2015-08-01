# this file is included from makefile and the project generator python script
# note: the project generator is not make, and hence does not obey make syntax
#   lines are matched as tokens once only, so do not use repeated +=
#

TOOLCHAIN ?= XC16
TARGET_TYPE := hex
CPU := 33FJ256GP710A

modules += libUDB libVectorMatrix libFlashFS Microchip
incpath += Microchip Microchip/Include libVectorMatrix
#cfgpath := Config

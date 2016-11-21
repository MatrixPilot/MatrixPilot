# this file is included from makefile

ifeq ($(DEVICE),PX4)
modules := MatrixPilot MAVLink libDCM
incpath := MAVLink/include
else
#modules := MatrixPilot MAVLink libDCM libVectorMatrix
modules := MatrixPilot MAVLink libDCM libVectorMatrix libFlashFS Microchip
incpath := MAVLink/include Microchip Microchip/Include libVectorMatrix
#defines := NOFS
defines :=
endif
cfgpath := Config

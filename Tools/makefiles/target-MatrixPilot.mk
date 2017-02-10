# this file is included from makefile

ifneq ($(DEVICE),PX4)

modules := MatrixPilot MAVLink libDCM libVectorMatrix libFlashFS Microchip
incpath := MAVLink/include Microchip Microchip/Include libVectorMatrix
defines :=
#defines := NOFS

else

modules := MatrixPilot MAVLink libDCM
incpath := MAVLink/include

endif

cfgpath := Config


@echo off

@rem pyProjectCreator.py --help
@rem	-n, "--name", dest="name", help="specify the project name", type="string", default="MatrixPilot", metavar="MatrixPilot")
@rem	-t, "--target", dest="target", help="specify the target board", type="string", default="UDB5", metavar="UDB5")
@rem	-m, "--mod", dest="modules", help="search directory for module.mk file", action='append')
@rem	-i, "--inc", dest="includes", help="additional include files directory", default="")
@rem	-c, "--cfg", dest="config", help="specify configuration files directory", default="../Config")
@rem	-o, "--out", dest="out", help="project files output path", default="output")
@rem #	--defines", dest="defines", help="specify optional defines", action='append')

@rem -r specify project root path
@rem -i specify include only path (relative to root path)
@rem -m specify source only path (relative to root path)
@rem -p specify path (include + source) path (relative to root path)

@rem -f specify file from makefile system to define further options, modules, includes, defines

@rem        value="%%CONFIG%%;../../MAVLink/include;../../Microchip;../../Microchip/Include" />


@rem pyProjectCreator.py -r ../.. -t SIL -o build  -----  and then compute  (-f ../../device-SILSIM.mk)

pyProjectCreator.py -t SIL   -r ../..
pyProjectCreator.py -t PX4   -r ../..
pyProjectCreator.py -t UDB4  -r ../..
pyProjectCreator.py -t UDB5  -r ../..
pyProjectCreator.py -t AUAV3 -r ../..

pyProjectCreator.py -n RollPitchYaw -t UDB4  -r ../..
pyProjectCreator.py -n RollPitchYaw -t UDB5  -r ../..
pyProjectCreator.py -n RollPitchYaw -t AUAV3 -r ../..

pyProjectCreator.py -n LedTest -t UDB5  -r ../..

goto END:


pyProjectCreator.py -t SIL   -o ../../build -r ../.. -f ../../device-SILSIM.mk
pyProjectCreator.py -t PX4   -o ../../build -r ../.. -f ../../device-PX4.mk
pyProjectCreator.py -t UDB4  -o ../../build -r ../.. -f ../../device-UDB4.mk
pyProjectCreator.py -t UDB5  -o ../../build -r ../.. -f ../../device-UDB5.mk
pyProjectCreator.py -t AUAV3 -o ../../build -r ../.. -f ../../device-AUAV3.mk

pyProjectCreator.py -n RollPitchYaw -t UDB5  -o ../../build -r ../.. -f ../../device-UDB5.mk

pyProjectCreator.py -t SIL   -o ../../build -r ../.. -c ../Config -m Config -m MatrixPilot -m libDCM -m MAVLink -m Tools/MatrixPilot-SIL -i FreeRTOS/include
pyProjectCreator.py -t PX4   -o ../../build -r ../.. -c ../Config -m Config -m MatrixPilot -m libDCM -m libSTM -m MAVLink -i FreeRTOS/include
pyProjectCreator.py -t UDB4  -o ../../build -r ../.. -c ../Config -m Config -m MatrixPilot -m libDCM -m libUDB -m libFlashFS -m libVectorMatrix -m MAVLink -m Microchip -m Tools/MatrixPilot-SIL -i MAVLink/include -i Microchip -i Microchip/Include
pyProjectCreator.py -t UDB5  -o ../../build -r ../.. -c ../Config -m Config -m MatrixPilot -m libDCM -m libUDB -m libFlashFS -m libVectorMatrix -m MAVLink -m Microchip -m Tools/MatrixPilot-SIL -i MAVLink/include -i Microchip -i Microchip/Include
pyProjectCreator.py -t AUAV3 -o ../../build -r ../.. -c ../Config -m Config -m MatrixPilot -m libDCM -m libUDB -m libFlashFS -m libVectorMatrix -m MAVLink -m Microchip -m Tools/MatrixPilot-SIL -i MAVLink/include -i Microchip -i Microchip/Include -i FreeRTOS/include

pyProjectCreator.py -n RollPitchYaw -t UDB4  -o ../../build -r ../.. -c ../RollPitchYaw -m RollPitchYaw -m libDCM -m libUDB -m libVectorMatrix -i MAVLink/include -i Microchip -i Microchip/Include
pyProjectCreator.py -n RollPitchYaw -t UDB5  -o ../../build -r ../.. -c ../RollPitchYaw -m RollPitchYaw -m libDCM -m libUDB -m libVectorMatrix -i MAVLink/include -i Microchip -i Microchip/Include
pyProjectCreator.py -n RollPitchYaw -t AUAV3 -o ../../build -r ../.. -c ../RollPitchYaw -m RollPitchYaw -m libDCM -m libUDB -m libVectorMatrix -i MAVLink/include -i Microchip -i Microchip/Include

pyProjectCreator.py -n LedTest -t UDB4  -o ../../build -r ../.. -c ../../Tools/LedTest -m Tools/LedTest -m libUDB -i MAVLink/include -i Microchip -i Microchip/Include
pyProjectCreator.py -n LedTest -t UDB5  -o ../../build -r ../.. -c ../../Tools/LedTest -m Tools/LedTest -m libUDB -i MAVLink/include -i Microchip -i Microchip/Include
pyProjectCreator.py -n LedTest -t AUAV3 -o ../../build -r ../.. -c ../../Tools/LedTest -m Tools/LedTest -m libUDB -i MAVLink/include -i Microchip -i Microchip/Include

:END
pause

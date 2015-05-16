@echo off

@rem pyProjectCreator.py --help
@rem	-r, "--root", specify project root path, default="."
@rem	-n, "--name", specify the project name, default="MatrixPilot"
@rem	-t, "--target", specify the target board, default="UDB5"

@rem	-c, "--cfg", specify configuration files directory, default="../Config"
@rem	-d, "--defines", specify optional defines"

@rem	-m, "--mod", dest="modules", help="search directory for module.mk file", action='append')
@rem	-i, "--inc", dest="includes", help="additional include files directory", default="")
@rem	-o, "--out", dest="out", help="project files output path", default="output")

@rem -i specify include only path (relative to root path)
@rem -m specify source only path (relative to root path)
@rem -p specify path (include + source) path (relative to root path)
@rem -f specify file from makefile system to define further options, modules, includes, defines

@rem        value="%%CONFIG%%;../../MAVLink/include;../../Microchip;../../Microchip/Include" />


@rem pyProjectCreator.py -r ../.. -t SIL -o build  -----  and then compute  (-f ../../device-SILSIM.mk)

pyProjectCreator.py -r ../.. -n MatrixPilot -t SIL
pyProjectCreator.py -r ../.. -n MatrixPilot -t PX4
pyProjectCreator.py -r ../.. -n MatrixPilot -t UDB4
pyProjectCreator.py -r ../.. -n MatrixPilot -t UDB5
pyProjectCreator.py -r ../.. -n MatrixPilot -t AUAV3

pyProjectCreator.py -r ../.. -n RollPitchYaw -t PX4
pyProjectCreator.py -r ../.. -n RollPitchYaw -t UDB4
pyProjectCreator.py -r ../.. -n RollPitchYaw -t UDB5
pyProjectCreator.py -r ../.. -n RollPitchYaw -t AUAV3

pyProjectCreator.py -r ../.. -n LedTest -t PX4
pyProjectCreator.py -r ../.. -n LedTest -t UDB4
pyProjectCreator.py -r ../.. -n LedTest -t UDB5
pyProjectCreator.py -r ../.. -n LedTest -t AUAV3

goto END:
:END

pause

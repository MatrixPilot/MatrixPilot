@echo off

pyProjectCreator.py -r ../.. -t SIL -c Config/Cessna

pyProjectCreator.py -r ../.. -t SIL
pyProjectCreator.py -r ../.. -t PX4
pyProjectCreator.py -r ../.. -t UDB4
pyProjectCreator.py -r ../.. -t UDB5
pyProjectCreator.py -r ../.. -t AUAV3

pyProjectCreator.py -r ../.. -n RollPitchYaw -t UDB4
pyProjectCreator.py -r ../.. -n RollPitchYaw -t UDB5
pyProjectCreator.py -r ../.. -n RollPitchYaw -t AUAV3

pyProjectCreator.py -r ../.. -n LedTest -t UDB4
pyProjectCreator.py -r ../.. -n LedTest -t UDB5
pyProjectCreator.py -r ../.. -n LedTest -t AUAV3

goto END:
:END
pause

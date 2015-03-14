@echo off

pyProjectCreator.py -t SIL   -r ../..
pyProjectCreator.py -t UDB4  -r ../..
pyProjectCreator.py -t UDB5  -r ../..
pyProjectCreator.py -t AUAV3 -r ../..

pyProjectCreator.py -n RollPitchYaw -t UDB4  -r ../..
pyProjectCreator.py -n RollPitchYaw -t UDB5  -r ../..
pyProjectCreator.py -n RollPitchYaw -t AUAV3 -r ../..

pyProjectCreator.py -n LedTest -t UDB4   -r ../..
pyProjectCreator.py -n LedTest -t UDB5   -r ../..
pyProjectCreator.py -n LedTest -t AUAV3  -r ../..

pause

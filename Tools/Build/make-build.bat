@echo off

pyProjectCreator.py -r ../.. -t SIL -c Config/Cessna -d USE_LOGO
pyProjectCreator.py -r ../.. -t PX4 -c Config/Cessna
pyProjectCreator.py -r ../.. -t UDB4 -c Config/Cessna -d NOFS=1

@rem goto END

pyProjectCreator.py -r ../.. -t UDB5 -c Config/Cessna -d NOFS=1
pyProjectCreator.py -r ../.. -t UDB5 -c Config/CloudsFly -d NOFS=1

pyProjectCreator.py -r ../.. -t AUAV3 -c Config/Cessna -d USE_NORADIO
pyProjectCreator.py -r ../.. -t AUAV3

pyProjectCreator.py -r ../.. -n RollPitchYaw -t UDB4
pyProjectCreator.py -r ../.. -n RollPitchYaw -t UDB5
pyProjectCreator.py -r ../.. -n RollPitchYaw -t AUAV3

pyProjectCreator.py -r ../.. -n LedTest -t UDB4
pyProjectCreator.py -r ../.. -n LedTest -t UDB5
pyProjectCreator.py -r ../.. -n LedTest -t AUAV3

:END
pause

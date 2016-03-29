@echo off
@for /f "usebackq" %%i in (`git symbolic-ref -q HEAD`) do @echo %%~ni

goto START:

.\Tools\Build\pyProjectCreator.py -n MatrixPilot -t SIL -c Config/Cessna
.\Tools\Build\pyProjectCreator.py -n MatrixPilot -t SIL -c Config/CloudsFly

:START

.\Tools\Build\pyProjectCreator.py -n MatrixPilot -t PX4 -c Config/Cessna -d HILSIM

goto END:

.\Tools\Build\pyProjectCreator.py -n MatrixPilot -t AUAV3 -c Config/Cessna -d HILSIM


.\Tools\Build\pyProjectCreator.py -n RollPitchYaw -t PX4
.\Tools\Build\pyProjectCreator.py -n RollPitchYaw -t UDB4
.\Tools\Build\pyProjectCreator.py -n RollPitchYaw -t UDB5
.\Tools\Build\pyProjectCreator.py -n RollPitchYaw -t AUAV3

.\Tools\Build\pyProjectCreator.py -n LedTest -t PX4
.\Tools\Build\pyProjectCreator.py -n LedTest -t UDB4
.\Tools\Build\pyProjectCreator.py -n LedTest -t UDB5
.\Tools\Build\pyProjectCreator.py -n LedTest -t AUAV3

goto END:

@rem .\Tools\Build\pyProjectCreator.py -n MatrixPilot -t SIL -d USE_FREERTOS -c Config/Cessna
.\Tools\Build\pyProjectCreator.py -n MatrixPilot -t SIL -c Config/Cessna
.\Tools\Build\pyProjectCreator.py -n MatrixPilot -t PX4 -d HILSIM -c Config/Cessna
.\Tools\Build\pyProjectCreator.py -n MatrixPilot -t AUAV3 -d HILSIM -c Config/Cessna

goto END:

.\Tools\Build\pyProjectCreator.py -n MatrixPilot -t UDB4 -d USE_FREERTOS
.\Tools\Build\pyProjectCreator.py -n MatrixPilot -t UDB5 -d USE_FREERTOS
.\Tools\Build\pyProjectCreator.py -n MatrixPilot -t AUAV3 -d USE_FREERTOS

:END

pause

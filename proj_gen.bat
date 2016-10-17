@echo off
@for /f "usebackq" %%i in (`git symbolic-ref -q HEAD`) do @echo %%~ni

goto START:
.\Tools\ProjGen\pyProjectCreator.py -n MatrixPilot -t SIL -c Config/Cessna
.\Tools\ProjGen\pyProjectCreator.py -n MatrixPilot -t SIL -c Config/CloudsFly
.\Tools\ProjGen\pyProjectCreator.py -n MatrixPilot -t PX4 -c Config/Cessna -d HILSIM
.\Tools\ProjGen\pyProjectCreator.py -n MatrixPilot -t AUAV3 -c Config/Cessna -d HILSIM
:START

.\Tools\ProjGen\pyProjectCreator.py -o . -s1 -n MatrixPilot -t UDB4
.\Tools\ProjGen\pyProjectCreator.py -o . -s1 -n MatrixPilot -t UDB5
.\Tools\ProjGen\pyProjectCreator.py -o . -s1 -n MatrixPilot -t AUAV3

.\Tools\ProjGen\pyProjectCreator.py -o . -s1 -n RollPitchYaw -t UDB4
.\Tools\ProjGen\pyProjectCreator.py -o . -s1 -n RollPitchYaw -t UDB5
.\Tools\ProjGen\pyProjectCreator.py -o . -s1 -n RollPitchYaw -t AUAV3

.\Tools\ProjGen\pyProjectCreator.py -o Tools -s1 -n LedTest -t UDB4
.\Tools\ProjGen\pyProjectCreator.py -o Tools -s1 -n LedTest -t UDB5
.\Tools\ProjGen\pyProjectCreator.py -o Tools -s1 -n LedTest -t AUAV3

.\Tools\ProjGen\pyProjectCreator.py -o Tools -s1 -n FlashOSD -t UDB4
.\Tools\ProjGen\pyProjectCreator.py -o Tools -s1 -n FlashOSD -t UDB5
.\Tools\ProjGen\pyProjectCreator.py -o Tools -s1 -n FlashOSD -t AUAV3

goto END:
@rem .\Tools\ProjGen\pyProjectCreator.py -n MatrixPilot -t SIL -d USE_FREERTOS -c Config/Cessna
.\Tools\ProjGen\pyProjectCreator.py -n MatrixPilot -t SIL -c Config/Cessna
.\Tools\ProjGen\pyProjectCreator.py -n MatrixPilot -t PX4 -d HILSIM -c Config/Cessna
.\Tools\ProjGen\pyProjectCreator.py -n MatrixPilot -t AUAV3 -d HILSIM -c Config/Cessna
.\Tools\ProjGen\pyProjectCreator.py -n MatrixPilot -t UDB4 -d USE_FREERTOS
.\Tools\ProjGen\pyProjectCreator.py -n MatrixPilot -t UDB5 -d USE_FREERTOS
.\Tools\ProjGen\pyProjectCreator.py -n MatrixPilot -t AUAV3 -d USE_FREERTOS
:END

pause

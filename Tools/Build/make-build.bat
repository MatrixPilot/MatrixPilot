@echo off

pyProjectCreator.py -t UDB4  -o ../../build -c ../Config -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip -d Tools/MatrixPilot-SIL -d FreeRTOS
pyProjectCreator.py -t UDB5  -o ../../build -c ../Config -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip -d Tools/MatrixPilot-SIL -d FreeRTOS
pyProjectCreator.py -t AUAV3 -o ../../build -c ../Config -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip -d Tools/MatrixPilot-SIL -d FreeRTOS
pyProjectCreator.py -t SIL   -o ../../build -c ../Config -d Config -d MatrixPilot -d libDCM -d MAVLink -d Tools/MatrixPilot-SIL -d FreeRTOS

pyProjectCreator.py -n RollPitchYaw -t UDB4  -o ../../build -c ../RollPitchYaw -d RollPitchYaw -d libUDB -d libDCM -d libVectorMatrix -d FreeRTOS
pyProjectCreator.py -n RollPitchYaw -t UDB5  -o ../../build -c ../RollPitchYaw -d RollPitchYaw -d libUDB -d libDCM -d libVectorMatrix -d FreeRTOS
pyProjectCreator.py -n RollPitchYaw -t AUAV3 -o ../../build -c ../RollPitchYaw -d RollPitchYaw -d libUDB -d libDCM -d libVectorMatrix -d FreeRTOS

pause

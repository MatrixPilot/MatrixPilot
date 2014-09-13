@echo off

pyProjectCreator.py -t UDB4  -o ..\..\build -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip
pyProjectCreator.py -t UDB5  -o ..\..\build -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip
pyProjectCreator.py -t AUAV3 -o ..\..\build -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip

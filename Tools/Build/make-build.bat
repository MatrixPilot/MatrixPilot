@echo off

pyProjectCreator.py -t UDB4  -o ../../build -c ../../Config -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip
pyProjectCreator.py -t UDB5  -o ../../build -c ../../Config -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip
pyProjectCreator.py -t AUAV3 -o ../../build -c ../../Config -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip

pyProjectCreator.py -n RollPitchYaw -t UDB4  -o ../../build -c ../../RollPitchYaw -d RollPitchYaw -d libUDB -d libDCM -d libVectorMatrix
pyProjectCreator.py -n RollPitchYaw -t UDB5  -o ../../build -c ../../RollPitchYaw -d RollPitchYaw -d libUDB -d libDCM -d libVectorMatrix
pyProjectCreator.py -n RollPitchYaw -t AUAV3 -o ../../build -c ../../RollPitchYaw -d RollPitchYaw -d libUDB -d libDCM -d libVectorMatrix

pyProjectCreator.py -n LedTest -t UDB4  -o ../../build -c ../../Tools/LedTest -d Tools/LedTest -d libUDB
pyProjectCreator.py -n LedTest -t UDB5  -o ../../build -c ../../Tools/LedTest -d Tools/LedTest -d libUDB
pyProjectCreator.py -n LedTest -t AUAV3 -o ../../build -c ../../Tools/LedTest -d Tools/LedTest -d libUDB

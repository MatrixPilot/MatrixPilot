#!/bin/bash

python pyProjectCreator.py -t SIL  -r ../..  -o ../../build -d Config -d MatrixPilot -d libDCM -d MAVLink -d Tools/MatrixPilot-SIL

python pyProjectCreator.py -t UDB4 -r ../.. -o ../../build -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip -d Tools/MatrixPilot-SIL
python pyProjectCreator.py -t UDB5 -r ../..  -o ../../build -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip -d Tools/MatrixPilot-SIL
python pyProjectCreator.py -t AUAV3 -r ../.. -o ../../build -d Config -d MatrixPilot -d libUDB -d libDCM -d libFlashFS -d libVectorMatrix -d MAVLink -d Microchip -d Tools/MatrixPilot-SIL

#python pyProjectCreator.py -n RollPitchYaw -t UDB4  -o ../../build -c ../../RollPitchYaw -d RollPitchYaw -d libUDB -d libDCM -d libVectorMatrix
#python pyProjectCreator.py -n RollPitchYaw -t UDB5  -o ../../build -c ../../RollPitchYaw -d RollPitchYaw -d libUDB -d libDCM -d libVectorMatrix
#python pyProjectCreator.py -n RollPitchYaw -t AUAV3 -o ../../build -c ../../RollPitchYaw -d RollPitchYaw -d libUDB -d libDCM -d libVectorMatrix

#python pyProjectCreator.py -n LedTest -t UDB4  -o ../../build -c ../../Tools/LedTest -d Tools/LedTest -d libUDB
#python pyProjectCreator.py -n LedTest -t UDB5  -o ../../build -c ../../Tools/LedTest -d Tools/LedTest -d libUDB
#python pyProjectCreator.py -n LedTest -t AUAV3 -o ../../build -c ../../Tools/LedTest -d Tools/LedTest -d libUDB

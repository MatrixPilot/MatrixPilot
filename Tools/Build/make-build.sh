#!/bin/bash

python pyProjectCreator.py -t SIL  -r ../..  -o ../../build -m Config -m MatrixPilot -m libDCM -m MAVLink -m Tools/MatrixPilot-SIL

python pyProjectCreator.py -t UDB4 -r ../.. -o ../../build -m Config -m MatrixPilot -m libUDB -m libDCM -m libFlashFS -m libVectorMatrix -m MAVLink -m Microchip -m Tools/MatrixPilot-SIL
python pyProjectCreator.py -t UDB5 -r ../..  -o ../../build -m Config -m MatrixPilot -m libUDB -m libDCM -m libFlashFS -m libVectorMatrix -m MAVLink -m Microchip -m Tools/MatrixPilot-SIL
python pyProjectCreator.py -t AUAV3 -r ../.. -o ../../build -m Config -m MatrixPilot -m libUDB -m libDCM -m libFlashFS -m libVectorMatrix -m MAVLink -m Microchip -m Tools/MatrixPilot-SIL

#python pyProjectCreator.py -n RollPitchYaw -t UDB4  -o ../../build -c ../../RollPitchYaw -m RollPitchYaw -m libUDB -m libDCM -m libVectorMatrix
#python pyProjectCreator.py -n RollPitchYaw -t UDB5  -o ../../build -c ../../RollPitchYaw -m RollPitchYaw -m libUDB -m libDCM -m libVectorMatrix
#python pyProjectCreator.py -n RollPitchYaw -t AUAV3 -o ../../build -c ../../RollPitchYaw -m RollPitchYaw -m libUDB -m libDCM -m libVectorMatrix

#python pyProjectCreator.py -n LedTest -t UDB4  -o ../../build -c ../../Tools/LedTest -m Tools/LedTest -m libUDB
#python pyProjectCreator.py -n LedTest -t UDB5  -o ../../build -c ../../Tools/LedTest -m Tools/LedTest -m libUDB
#python pyProjectCreator.py -n LedTest -t AUAV3 -o ../../build -c ../../Tools/LedTest -m Tools/LedTest -m libUDB

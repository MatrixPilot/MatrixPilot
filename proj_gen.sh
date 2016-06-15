#!/bin/bash

#python ./Tools/Build/pyProjectCreator.py -n MatrixPilot -t SIL -c Config/Cessna
#python ./Tools/Build/pyProjectCreator.py -n MatrixPilot -t SIL -c Config/CloudsFly
python ./Tools/Build/pyProjectCreator.py -n MatrixPilot -t PX4 -c Config/Cessna -d HILSIM

#exit 0

python ./Tools/Build/pyProjectCreator.py -n MatrixPilot -t UDB4
python ./Tools/Build/pyProjectCreator.py -n MatrixPilot -t UDB5
python ./Tools/Build/pyProjectCreator.py -n MatrixPilot -t AUAV3

python ./Tools/Build/pyProjectCreator.py -n RollPitchYaw -t UDB4
python ./Tools/Build/pyProjectCreator.py -n RollPitchYaw -t UDB5
python ./Tools/Build/pyProjectCreator.py -n RollPitchYaw -t AUAV3

python ./Tools/Build/pyProjectCreator.py -n LedTest -t UDB4
python ./Tools/Build/pyProjectCreator.py -n LedTest -t UDB5
python ./Tools/Build/pyProjectCreator.py -n LedTest -t AUAV3

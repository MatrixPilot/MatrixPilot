#!/bin/bash

#python ./Tools/ProjGen/pyProjectCreator.py -o . -n MatrixPilot -t SIL -c Config/Cessna
#python ./Tools/ProjGen/pyProjectCreator.py -o . -n MatrixPilot -t SIL -c Config/CloudsFly
#python ./Tools/ProjGen/pyProjectCreator.py -o . -n MatrixPilot -t PX4 -c Config/Cessna -d HILSIM
#exit 0

python ./Tools/ProjGen/pyProjectCreator.py -o . -s1 -n MatrixPilot -t UDB4
python ./Tools/ProjGen/pyProjectCreator.py -o . -s1 -n MatrixPilot -t UDB5
python ./Tools/ProjGen/pyProjectCreator.py -o . -s1 -n MatrixPilot -t AUAV3

python ./Tools/ProjGen/pyProjectCreator.py -o . -s1 -n RollPitchYaw -t UDB4
python ./Tools/ProjGen/pyProjectCreator.py -o . -s1 -n RollPitchYaw -t UDB5
python ./Tools/ProjGen/pyProjectCreator.py -o . -s1 -n RollPitchYaw -t AUAV3

python ./Tools/ProjGen/pyProjectCreator.py -o Tools -s1 -n LedTest -t UDB4
python ./Tools/ProjGen/pyProjectCreator.py -o Tools -s1 -n LedTest -t UDB5
python ./Tools/ProjGen/pyProjectCreator.py -o Tools -s1 -n LedTest -t AUAV3

python ./Tools/ProjGen/pyProjectCreator.py -o Tools -s1 -n FlashOSD -t UDB4
python ./Tools/ProjGen/pyProjectCreator.py -o Tools -s1 -n FlashOSD -t UDB5
python ./Tools/ProjGen/pyProjectCreator.py -o Tools -s1 -n FlashOSD -t AUAV3


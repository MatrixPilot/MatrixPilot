#!/bin/bash

python pyProjectCreator.py -t SIL   -r ../..

python pyProjectCreator.py -t UDB4  -r ../..
python pyProjectCreator.py -t UDB5  -r ../..
python pyProjectCreator.py -t AUAV3 -r ../..

python pyProjectCreator.py -n RollPitchYaw -t UDB4  -r ../..
python pyProjectCreator.py -n RollPitchYaw -t UDB5  -r ../..
python pyProjectCreator.py -n RollPitchYaw -t AUAV3 -r ../..

python pyProjectCreator.py -n LedTest -t UDB4   -r ../..
python pyProjectCreator.py -n LedTest -t UDB5   -r ../..
python pyProjectCreator.py -n LedTest -t AUAV3  -r ../..

@echo off

@rem python generateds_gui.py --session=genParamDB.session
generateDS.py -o ParameterDatabase.py -s SubParameterDatabase.py ParameterDatabase.xsd

pause


@echo off

pushd MatrixPilot
@rem java -jar ../Tools/Smc.jar -c -verbose FlightState.sm
@rem java -jar ../Tools/Smc.jar -graph -glevel 1 FlightState.sm
@rem "C:\Program Files (x86)\Graphviz2.38\bin\dot.exe" -T png -o FlightState_sm.png FlightState_sm.dot
make -f makefile.sm DEVICE=SIL
make -f makefile.sm graph DEVICE=SIL
@rem make -f makefile.sm png
"C:\Program Files (x86)\Graphviz2.38\bin\dot.exe" -T png -o FlightState_sm.png FlightState_sm.dot
popd

mkdir build >nul 2>&1
pushd build

goto START:
:START

mkdir SIL >nul 2>&1
pushd SIL
make -j 8 -f ../../makefile DEVICE=SIL
@rem make -f ../../makefile graph
popd

mkdir HILSIM >nul 2>&1
pushd HILSIM
mkdir Cessna >nul 2>&1
pushd Cessna
make -j 8 -f ../../../makefile DEVICE=SIL CONFIG=Cessna
popd
mkdir CloudsFly >nul 2>&1
pushd CloudsFly
make -j 8 -f ../../../makefile DEVICE=SIL CONFIG=CloudsFly
popd
popd

@rem goto END:

mkdir PX4 >nul 2>&1
pushd PX4
make -j 8 -f ../../makefile TARGET_NAME=MatrixPilot DEVICE=PX4
popd

mkdir PX4-RPY >nul 2>&1
pushd PX4-RPY
make -j 8 -f ../../makefile TARGET_NAME=RollPitchYaw DEVICE=PX4
popd

mkdir UDB5-RPY >nul 2>&1
pushd UDB5-RPY
make -j 8 -f ../../makefile TARGET_NAME=RollPitchYaw DEVICE=UDB5
popd

goto END:

mkdir AUAV3-C30 >nul 2>&1
pushd AUAV3-C30
make -j 8 -f ../../makefile DEVICE=AUAV3 TOOLCHAIN=C30
popd

goto LAST:

mkdir UDB4-C30 >nul 2>&1
pushd UDB4-C30
make -j 8 -f ../../makefile DEVICE=UDB4 TOOLCHAIN=C30
popd
mkdir UDB5-C30 >nul 2>&1
pushd UDB5-C30
make -j 8 -f ../../makefile DEVICE=UDB5 TOOLCHAIN=C30
popd

mkdir AUAV3-XC16 >nul 2>&1
pushd AUAV3-XC16
make -j 8 -f ../../makefile DEVICE=AUAV3 TOOLCHAIN=XC16
popd
mkdir UDB4-XC16 >nul 2>&1
pushd UDB4-XC16
make -j 8 -f ../../makefile DEVICE=UDB4 TOOLCHAIN=XC16
popd

:LAST

mkdir UDB5-XC16 >nul 2>&1
pushd UDB5-XC16
make -j 8 -f ../../makefile DEVICE=UDB5 TOOLCHAIN=XC16
popd

goto END:
:END

popd

dir build\*.hex build\*.exe build\*.map /B /S

pause

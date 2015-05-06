@echo off

mkdir build >nul 2>&1
pushd build

goto START:
:START

mkdir SIL >nul 2>&1
pushd SIL
make -j 8 -f ../../makefile DEVICE=SIL CONFIG=Cessna
popd

mkdir PX4 >nul 2>&1
pushd PX4
make -j 8 -f ../../makefile DEVICE=PX4 CONFIG=Cessna
popd

mkdir AUAV3-C30 >nul 2>&1
pushd AUAV3-C30
make -j 8 -f ../../makefile DEVICE=AUAV3 TOOLCHAIN=C30 CONFIG=Cessna
popd
mkdir UDB4-C30 >nul 2>&1
pushd UDB4-C30
make -j 8 -f ../../makefile DEVICE=UDB4 TOOLCHAIN=C30 CONFIG=Cessna
popd
mkdir UDB5-C30 >nul 2>&1
pushd UDB5-C30
make -j 8 -f ../../makefile DEVICE=UDB5 TOOLCHAIN=C30 CONFIG=Cessna
popd

mkdir AUAV3-XC16 >nul 2>&1
pushd AUAV3-XC16
make -j 8 -f ../../makefile DEVICE=AUAV3 TOOLCHAIN=XC16 CONFIG=Cessna
popd
mkdir UDB4-XC16 >nul 2>&1
pushd UDB4-XC16
make -j 8 -f ../../makefile DEVICE=UDB4 TOOLCHAIN=XC16 CONFIG=Cessna
popd
mkdir UDB5-XC16 >nul 2>&1
pushd UDB5-XC16
make -j 8 -f ../../makefile DEVICE=UDB5 TOOLCHAIN=XC16 CONFIG=Cessna
popd

popd

@rem dir build\*.hex build\*.exe build\*.map /B /S
dir build\*.hex build\*.exe /B /S

pause

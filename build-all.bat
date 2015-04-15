@echo off
mkdir build >nul 2>&1
pushd build

mkdir SILSIM >nul 2>&1
pushd SILSIM
make -f ../../makefile DEVICE=SILSIM
popd

mkdir AUAV3 >nul 2>&1
pushd AUAV3
make -f ../../makefile DEVICE=AUAV3 TOOLCHAIN=C30
popd
mkdir UDB4 >nul 2>&1
pushd UDB4
make -f ../../makefile DEVICE=UDB4 TOOLCHAIN=C30
popd
mkdir UDB5 >nul 2>&1
pushd UDB5
make -f ../../makefile DEVICE=UDB5 TOOLCHAIN=C30
popd

mkdir AUAV3-XC16 >nul 2>&1
pushd AUAV3-XC16
make -f ../../makefile DEVICE=AUAV3 TOOLCHAIN=XC16
popd
mkdir UDB4-XC16 >nul 2>&1
pushd UDB4-XC16
make -f ../../makefile DEVICE=UDB4 TOOLCHAIN=XC16
popd
mkdir UDB5-XC16 >nul 2>&1
pushd UDB5-XC16
make -f ../../makefile DEVICE=UDB5 TOOLCHAIN=XC16
popd

popd

dir build\*.hex build\*.exe build\*.map /B /S

pause

@echo off

mkdir _build >nul 2>&1
pushd _build

mkdir SIL >nul 2>&1
pushd SIL
make -j 8 -f ../../makefile DEVICE=SIL CONFIG=Cessna
popd

@rem mkdir PX4 >nul 2>&1
@rem pushd PX4
@rem make -j 8 -f ../../makefile DEVICE=PX4
@rem popd

mkdir AUAV3-XC16 >nul 2>&1
pushd AUAV3-XC16
make -j 8 -f ../../makefile DEVICE=AUAV3 TOOLCHAIN=XC16
popd
mkdir UDB4-XC16 >nul 2>&1
pushd UDB4-XC16
make -j 8 -f ../../makefile DEVICE=UDB4 TOOLCHAIN=XC16
popd
mkdir UDB5-XC16 >nul 2>&1
pushd UDB5-XC16
make -j 8 -f ../../makefile DEVICE=UDB5 TOOLCHAIN=XC16
popd

popd

@rem dir _build\*.hex _build\*.exe _build\*.map /B /S
dir _build\*.hex _build\*.exe /B /S

pause

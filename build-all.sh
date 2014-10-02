#!/bin/bash

mkdir build
cd build

mkdir SILSIM
cd SILSIM
pwd
make -f ../../makefile SOURCE_DIR=../.. DEVICE=SILSIM
cd ..
pwd
read -p "Press [Enter] key to continue"

mkdir UDB5
cd UDB5
pwd
make -f ../../makefile SOURCE_DIR=../.. DEVICE=UDB5 TOOLCHAIN=XC16
cd ..
read -p "Press [Enter] key to continue"

mkdir AUAV3
cd AUAV3
pwd
make -f ../../makefile SOURCE_DIR=../.. DEVICE=AUAV3 TOOLCHAIN=XC16
cd ..
read -p "Press [Enter] key to continue"

mkdir UDB4
cd UDB4
pwd
make -f ../../makefile SOURCE_DIR=../.. DEVICE=UDB4 TOOLCHAIN=XC16
cd ..
read -p "Press [Enter] key to continue"

cd ..

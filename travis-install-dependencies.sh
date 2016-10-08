#/usr/bin/env sh
#
# Install the software needed for a build.
#
#*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
# 
# Copyright (c) 2015 Jeroen de Bruijn
# 
# This file is part of MPLABX_Travis-CI_Example which is released under the
# MIT License (MIT).
# For full license details see file "LICENSE" or go to
# https://opensource.org/licenses/MIT
#
#*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*
#
# Downloads the software from Microchip and saves them in a folder outside
# of the project directory. Than extracts and installs them.
# 
# Versions:
#  MPLAB® X IDE:         v3.15 (not required for travis builds)
#  MPLAB® XC16 Compiler: v1.24
#  MPLAB® XC16 Compiler: v1.25
#  MPLAB® XC16 Compiler: v1.26 (default)
#

# Save the current working directory
pushd .

# Create a folder for the installation files.
mkdir ../install-dependencies
cd ../install-dependencies

export XC16VER=v1.26
echo "- Microchip XC16 Compiler "$XC16VER" -"

### IDE
# Install MPLAB X IDE v3.15
#echo "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+"
#echo "- Downloading MPLAB X IDE v3.15..."
#wget http://ww1.microchip.com/downloads/en/DeviceDoc/MPLABX-v3.15-linux-installer.tar
#echo "- Download finished..."
#echo "- Unpacking MPLABX-v3.15-linux-installer.tar..."
#tar -xvf MPLABX-v3.15-linux-installer.tar
#echo "- Unpack finished..."
#echo "- Installing MPLAB X IDE v3.15..."
#sudo ./MPLABX-v3.15-linux-installer.sh -- --mode unattended
#echo "- Installation of MPLAB X IDE v3.15 finished."
#echo "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"
#echo 

### Compiler
# Install Microchip XC16 Compiler v1.25
#echo "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--+"
#echo "- Downloading Microchip XC16 Compiler v1.25"
#wget http://ww1.microchip.com/downloads/en/DeviceDoc/xc16-v1.25-full-install-linux-installer.run
#echo "- Download finished..."
#echo "- Adding excecution rights..."
#chmod +x xc16-v1.25-full-install-linux-installer.run
#echo "- Installing Microchip XC16 Compiler v1.25"
#sudo ./xc16-v1.25-full-install-linux-installer.run --mode unattended --netservername dontknow
#echo "- Installation of Microchip XC16 Compiler v1.25 finished."
#echo "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"
#echo 

### Compiler
# Install Microchip XC16 Compiler v1.26
echo "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--+"
echo "- Downloading Microchip XC16 Compiler v1.26"
wget http://ww1.microchip.com/downloads/en/DeviceDoc/xc16-v1.26-full-install-linux-installer.run
echo "- Download finished..."
echo "- Adding excecution rights..."
chmod +x xc16-v1.26-full-install-linux-installer.run
echo "- Installing Microchip XC16 Compiler v1.26"
sudo ./xc16-v1.26-full-install-linux-installer.run --mode unattended --netservername dontknow
echo "- Installation of Microchip XC16 Compiler v1.26 finished."
echo "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"
echo 

### Microchip Libraries
# Install Microchip Peripheral Libraries (PLIBS) v2.00
echo "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"
echo "- Downloading Microchip PLIBS v2.00"
wget http://ww1.microchip.com/downloads/en/DeviceDoc/peripheral-libraries-for-pic24-and-dspic-v2.00-linux-installer.run
echo "- Download finished..."
echo "- Adding excecution rights..."
chmod +x peripheral-libraries-for-pic24-and-dspic-v2.00-linux-installer.run
echo "- Installing Microchip PLIBS v2.00"
#sudo ./peripheral-libraries-for-pic24-and-dspic-v2.00-linux-installer.run --mode unattended --prefix /opt/microchip/xc16/v1.25
sudo ./peripheral-libraries-for-pic24-and-dspic-v2.00-linux-installer.run --mode unattended --prefix /opt/microchip/xc16/v1.26
echo "- Installation of Microchip PLIBS v2.00 finished."
echo "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"
echo 

### Compiler
# Install MPLAB XC16 Compiler v1.24
#echo "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"
#echo "- Downloading MPLAB XC16 Compiler v1.24"
#wget http://ww1.microchip.com/downloads/en/DeviceDoc/xc16-v1.24-full-install-linux-installer.run
#echo "- Download finished..."
#echo "- Adding excecution rights..."
#chmod +x xc16-v1.24-full-install-linux-installer.run
#echo "- Installing MPLAB XC16 Compiler v1.24"
#sudo ./xc16-v1.24-full-install-linux-installer.run --mode unattended --netservername dontknow
#echo "- Installation of MPLAB XC16 Compiler v1.24 finished."
#echo "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-"
#echo 

# Return to the saved working directory
popd


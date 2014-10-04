// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.

/*
LPS331 Key Features:
    260 to 1260 mbar absolute pressure range
    High-resolution mode: 0.020 mbar RMS
    Low power consumption:
    Low resolution mode: 5.5 ?A
    High resolution mode: 30 ?A
    High overpressure capability: 20x full scale
    Embedded temperature compensation
    Embedded 24-bit ADC
    Selectable ODR from 1 Hz to 25 Hz
    SPI and I2C interfaces
    Supply voltage: 1.71 to 3.6 V
    High shock survivability: 10,000 g
 */
 
//typedef void (*barometer_callback_funcptr)(long pressure, int temperature, char status);
//int rxBarometer(barometer_callback_funcptr);  // service the barometer

#ifndef LPS331_H
#define LPS331_H

//#include <Arduino.h> // for byte data type

// SA0 states

#define LPS331_SA0_LOW 0
#define LPS331_SA0_HIGH 1
#define LPS331_SA0_AUTO 2

// register addresses
// Note: Some of the register names in the datasheet are inconsistent
// between Table 14 in section 6 and the register descriptions in
// section 7. Where they differ, the names from section 7 have been
// used here.

#define LPS331_REF_P_XL 0x08
#define LPS331_REF_P_L 0x09
#define LPS331_REF_P_H 0x0A

#define LPS331_WHO_AM_I 0x0F

#define LPS331_RES_CONF 0x10

#define LPS331_CTRL_REG1 0x20
#define LPS331_CTRL_REG2 0x21
#define LPS331_CTRL_REG3 0x22
#define LPS331_INTERRUPT_CFG 0x23
#define LPS331_INT_SOURCE 0x24
#define LPS331_THS_P_L 0x25
#define LPS331_THS_P_H 0x26
#define LPS331_STATUS_REG 0x27

#define LPS331_PRESS_OUT_XL 0x28
#define LPS331_PRESS_OUT_L 0x29
#define LPS331_PRESS_OUT_H 0x2A

#define LPS331_TEMP_OUT_L 0x2B
#define LPS331_TEMP_OUT_H 0x2C

#define LPS331_AMP_CTRL 0x30

#define LPS331_DELTA_PRESS_XL 0x3C
#define LPS331_DELTA_PRESS_L 0x3D
#define LPS331_DELTA_PRESS_H 0x3E


//bool init(uint8_t sa0);

void enableDefault(void);

//void writeReg(uint8_t reg, uint8_t value);
//uint8_t readReg(uint8_t reg);

//float readPressureMillibars(void);
//float readPressureInchesHg(void);
//long readPressureRaw(void);
//float readTemperatureC(void);
//float readTemperatureF(void);
//int readTemperatureRaw(void);

//float pressureToAltitudeMeters(float pressure_mbar, float altimeter_setting_mbar = 1013.25);
//float pressureToAltitudeFeet(float pressure_inHg, float altimeter_setting_inHg = 29.9213);

#endif // LPS331_H


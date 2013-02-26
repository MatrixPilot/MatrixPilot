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


// also peeked at pitot tube code at:
// http://code.google.com/p/ardupilot-mega/source/browse/libraries/AP_Airspeed/AP_Airspeed.cpp

#include "defines.h"

#if (ANALOG_AIRSPEED_INPUT_CHANNEL != CHANNEL_UNUSED)
#include "airspeedPitot.h"

const float AIRSPEED_SCALAR = 1.9936;
const int16_t AIRSPEED_CAL_LENGTH = 20; // time is this value * ADC_sample_period
const float AIRSPEED_LPF_1_COEF = 0.1; // must be <= 1. Value == 1 means LPF is bypassed, lower means lower corner freq
const float AIRSPEED_LPF_2_COEF = 0.3; // must be <= 1. Value == 1 means LPF is bypassed, lower means lower corner freq

// global variables
AirspeedPitot airspeedPitot; // units are in cm/s


// Local functions
int16_t LPF_IIR(int16_t input);


// Local variables
boolean resetInputIIR, isCalibrating;
int16_t calIndex;
int32_t calAccum;



int16_t LPF_IIR(int16_t input)
{
	static int16_t lpf_1_lastOutput = 0;
    if (resetInputIIR)
    {
        resetInputIIR = false;
        lpf_1_lastOutput = input; // Init to steady state
    }

    lpf_1_lastOutput += (airspeedPitot.lpf_1_coef * (input - lpf_1_lastOutput));
    return lpf_1_lastOutput;
}

void udb_init_pitot(void)
{
    airspeedPitot.scalar = AIRSPEED_SCALAR;
    airspeedPitot.lpf_1_coef = AIRSPEED_LPF_1_COEF;
    airspeedPitot.lpf_2_coef = AIRSPEED_LPF_2_COEF;
    airspeedPitot.oneMinusLpf_2_coef = 1 - airspeedPitot.lpf_2_coef;

    start_Calibration();
}

void start_Calibration(void)
{
    airspeedPitot.zeroOffset = 0;
    calIndex = 0;
    calAccum = 0;
    resetInputIIR = true;
    isCalibrating = true;
    airspeedPitot.value = 0;
}

void AirspeedCalibration(int16_t value)
{
    if (calIndex < AIRSPEED_CAL_LENGTH)
    {
        calIndex++;
        calAccum += value;
    }
    else
    {
        airspeedPitot.zeroOffset = calAccum / calIndex;
        isCalibrating = false;
    }
}

void setAirspeedUsingAdcValue(int16_t adcValue)
{
    int16_t pressure;

    airspeedPitot.filteredAdcValue = LPF_IIR(adcValue);

    if (isCalibrating)
    {
        AirspeedCalibration(airspeedPitot.filteredAdcValue);
    }
    else
    {
        pressure = airspeedPitot.filteredAdcValue - airspeedPitot.zeroOffset;
        if (pressure < 0) // clip to positive
            pressure = 0;

        airspeedPitot.value =
            (airspeedPitot.lpf_2_coef * sqrt(pressure * airspeedPitot.scalar)) +
            (airspeedPitot.oneMinusLpf_2_coef * airspeedPitot.value);
    }
}
#endif //(ANALOG_AIRSPEED_INPUT_CHANNEL != CHANNEL_UNUSED)

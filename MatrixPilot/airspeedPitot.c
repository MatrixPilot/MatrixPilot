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

#define AIRSPEED_RATIO          1.9936
#define AIRSPEED_CAL_LENGTH     100 // time is this value * ADC_sample_period
#define AIRSPEED_FIR_LPFcoef    0.7 // must be <= 1. Value == 1 means LPF is bypassed, lower means lower corner freq

INT32 convertADCtoPressure(INT16 adcValue);

AirspeedPitot pitotAirspeed;


void udb_init_pitot()
{
    pitotAirspeed.ratio = AIRSPEED_RATIO;
    pitotAirspeed.lpfCoef = AIRSPEED_FIR_LPFcoef;
    start_Calibration();
}

void start_Calibration(void)
{
    pitotAirspeed.calIndex = 0;
    pitotAirspeed.calAccum = 0;
    pitotAirspeed.IsCalibrating = true;
    pitotAirspeed.airspeed = 0;
}

void AirspeedCalibration(INT32 pressure)
{
    if (pitotAirspeed.calIndex < AIRSPEED_CAL_LENGTH)
    {
        pitotAirspeed.calIndex++;
        pitotAirspeed.calAccum += pressure;
    }
    else
    {
        pitotAirspeed.zeroOffset = pitotAirspeed.calAccum / pitotAirspeed.calIndex;
        pitotAirspeed.IsCalibrating = false;
    }
}

INT32 convertADCtoPressure(INT16 adcValue)
{
    // Vout = 5V × (0.2 × P(kPa)+0.5) ± 6.25% VFSS
    //P(kPa) = ((Vout / 5) - 0.5) / 0.2
    //P(kPa) = Vout - 2.5 (easy!)

    INT32 pressure_kPa; // units in kPa
    float v;

    v = (adcValue * 3.3) / 0x03FF;  // convert 10bit value to 0 - 3.3V
    pressure_kPa = (v - 2.5);

    return pressure_kPa;
}

void SetAirspeedUsingAdcValue(INT16 adcValue)
{
    INT32 relativePressure;
    INT32 pressure_raw = convertADCtoPressure(adcValue);

    if (pitotAirspeed.IsCalibrating)
    {
        AirspeedCalibration(pressure_raw);
    }
    else
    {
        relativePressure = pressure_raw - pitotAirspeed.zeroOffset;
        if (relativePressure < 0) // clip positive
            relativePressure = 0;

        // First order FIR LPF with a twist
        pitotAirspeed.airspeed = (pitotAirspeed.lpfCoef * pitotAirspeed.airspeed) +
                                 ((1-pitotAirspeed.lpfCoef) * sqrt(relativePressure * pitotAirspeed.ratio));
    }
}
#endif //(ANALOG_AIRSPEED_INPUT_CHANNEL != CHANNEL_UNUSED)

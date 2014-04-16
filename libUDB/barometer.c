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


#include "libUDB_internal.h"
#include "I2C.h"
#include "barometer.h"

#if (USE_BAROMETER == 1 && HILSIM != 1)

typedef union {

    struct {
        int16_t ac1;
        int16_t ac2;
        int16_t ac3;
        uint16_t ac4;
        uint16_t ac5;
        uint16_t ac6;
        int16_t b1;
        int16_t b2;
        int16_t mb;
        int16_t mc;
        int16_t md;
    };
    uint8_t buf[22];
} barCalib_union_t;

#define BMP085_COMMAND 0xEE  // I2C address of BMP085
static barCalib_union_t bc;
static int32_t b5;

static uint8_t bmp085read_barCalib[] = {0xAA}; // Address of the first register to read
static uint8_t bmp085read_barTemp[] = {0x2E};
static uint8_t bmp085read_barPres[] = {0x34 + (BOS << 6)};
static uint8_t bmp085read_barData[] = {0xF6};
static uint8_t bmp085write_index[] = {0xF4}; // Address of the command register to write

static uint8_t barData[3];
static int16_t brindex; // index into the read write buffer
static int16_t barMessage = 0; // message type, state machine counter
static int16_t barCalibPause = 0;
#if (SBS != 0)
static int16_t p_i = 0; // index for pressure
static int16_t t_i = 0; // index for temperature
#endif
static int32_t p_s = 0; // pressure sum
static int16_t t_s = 0; // index into the read write buffer
static int32_t temperature;

void ReadBarTemp_callback(boolean I2CtrxOK);
void ReadBarPres_callback(boolean I2CtrxOK);
void ReadBarCalib_callback(boolean I2CtrxOK);


barometer_callback_funcptr barometer_callback = NULL;

void rxBarometer(barometer_callback_funcptr callback) // service the barometer
{
    barometer_callback = callback;

    if (I2C_Normal() == false) // if I2C is not ok
    {
        barMessage = 0; // start over again
        I2C_reset(); // reset the I2C
        return;
    }

    brindex = 0;

    if (barCalibPause == 0) {
        barMessage++;
        if (barMessage > 7) {
            barMessage = 4;
        }
        switch (barMessage) {
            case 1: // read the barometer in case it is still sending data, so as to NACK it
                break;
            case 2: // put barometer into the power up defaults on a reset
                break;
            case 3: // clear out any data that is still there
                I2C_Read(BMP085_COMMAND, bmp085read_barCalib, 1, bc.buf, 22, &ReadBarCalib_callback);
                break;
            case 4: // enable the calibration process
#if (BPXD != 0)
                barCalibPause = BPXD; // unecessary?
#endif
                I2C_Write(BMP085_COMMAND, bmp085write_index, 1, bmp085read_barTemp, 1, NULL);
                break;
            case 5:
                I2C_Read(BMP085_COMMAND, bmp085read_barData, 1, barData, 2, &ReadBarTemp_callback);
                break;
            case 6:
#if (BPXD != 0)
                barCalibPause = BPXD; // unecessary?
#endif
                I2C_Write(BMP085_COMMAND, bmp085write_index, 1, bmp085read_barPres, 1, NULL);
                break;
            case 7:
                I2C_Read(BMP085_COMMAND, bmp085read_barData, 1, barData, 3, &ReadBarPres_callback);
                break;
            default:
                barMessage = 0;
                break;
        }
    } else {
        barCalibPause--;
    }
    return;
}

static void byteswaparray(uint8_t* ary, int16_t len) {
    uint16_t i;
    uint8_t tmp;

    for (i = 0; i < len; i += 2) {
        tmp = ary[i];
        ary[i] = ary[i + 1];
        ary[i + 1] = tmp;
    }
}

void ReadBarCalib_callback(boolean I2CtrxOK) {
    if (I2CtrxOK == true) {
        byteswaparray(bc.buf, 22);
    }
}

// Calculate temperature given ut.
// Value returned originally in units of 0.1 deg C

static int32_t bmp085CalcTemperature(uint16_t ut) {
    int32_t x1, x2;
    x1 = ((int32_t) ut - bc.ac6) * bc.ac5 >> 15;
    x2 = ((int32_t) bc.mc << 11) / (x1 + bc.md);
    b5 = x1 + x2;

#if (SBS == 0)
    t_s = (b5 + 8) >> 4;
#else
    t_s += (b5 + 8) >> 4;

    // final sampling
    t_i++;
    if (t_i == SBS) {
        t_s *= 0.5;
        t_i /= 2;
    }
#endif	// (SBS == 0)

    return t_s;

}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085CalcTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.

static int32_t bmp085CalcPressure(uint32_t up) {
    int32_t x1, x2, x3, b3, b6, p;
    uint32_t b4, b7;
    int32_t tmp;

    b6 = b5 - 4000;
    x1 = (bc.b2 * (b6 * b6 >> 12)) >> 11;
    x2 = bc.ac2 * b6 >> 11;
    x3 = x1 + x2;

    tmp = bc.ac1;
    tmp = (tmp * 4 + x3) << BOS;
    b3 = (tmp + 2) / 4;

    x1 = bc.ac3 * b6 >> 13;
    x2 = (bc.b1 * (b6 * b6 >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (bc.ac4 * (uint32_t) (x3 + 32768)) >> 15;
    b7 = ((uint32_t) up - b3) * (50000 >> BOS);
    p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16; 
    x2 = (-7357 * p) >> 16;
#if (SBS == 0)
    p_s = p + (x1 + x2 + 3791) >> 4;
#else
    p_s += p + ((x1 + x2 + 3791) >> 4);
    p_i++;
    if (p_i == SBS) {
        p_s *= 0.5;
        p_i /= 2;
    }
#endif	// (SBS == 0)
    return p_s;
}

void ReadBarTemp_callback(boolean I2CtrxOK) {
    int16_t ut;

    if (I2CtrxOK == true) {
        ut = (int16_t) (barData[0] << 8 | barData[1]);
        t_s = bmp085CalcTemperature(ut);
#if (SBS == 0)
        temperature = 0.1f * t_s;
#else
        t_s = 0.1f * (t_s / t_i);
        temperature = t_s;
        t_i = 0;
#endif // (SBS == 0)
        t_s = 0;
    } else {
        temperature = 0;
    }
}

void ReadBarPres_callback(boolean I2CtrxOK) {
    uint32_t up, pressure;

    if (I2CtrxOK == true) {
        up = ((uint32_t) barData[0] << 16
                | (uint32_t) barData[1] << 8
                | (uint32_t) barData[2]) >> (8 - BOS);
        p_s = bmp085CalcPressure(up);
        if (barometer_callback != NULL) {
#if (SBS == 0)
            pressure = 0.1f * p_s;
            barometer_callback(pressure, temperature, 1);
#else
            p_s = 0.1f * (p_s / p_i);
            pressure = p_s;
            barometer_callback(pressure, temperature, 1);
            p_i = 0;
#endif	// (SBS == 0)
            p_s = 0;
            temperature = 0;
        }
    } else {
        // the operation failed
        barometer_callback(0, 0, 0);
    }
}

#endif // USE_BAROMETER

// GOOFY'S PARKING LOT:

/*

 */




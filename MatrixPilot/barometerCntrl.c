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

#include "../libDCM/libDCM.h"
#include "../libUDB/barometer.h"
#include "barometerCntrl.h"
#include "defines.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//	The origin is recorded as the altitude of the plane during power up of the control board.
//  ..Stored prior sampled and averaged runtime barometric data

int32_t il_pressure = 0; // inline pressure feed
int16_t il_temperature = 0; // inline temperature feed
int32_t il_aslaltitude = 0; // inline  ASL (above sea level) altitude, in Cm

int32_t il_pressure_ogn = 0; //  inline  pressure at origin
int16_t il_temperature_ogn = 0; //   inline temperature at origin
int32_t il_aslaltitude_ogn = 0; //   inline ASL altitude estimate at origin


static int32_t barometer_pressure = 0; // RT barometer.c pressure feed
static int16_t barometer_temperature = 0; // RT barometer.c temperature feed
static int32_t baralt_offset = 0;
static int32_t calpressure_ogn = 0; // calibrated origin pressure  persistent data
static int16_t caltemperature_ogn = 0; // calibrated origin temperature persistent data
static const int16_t room_temp = 15;
static uint8_t barometer_dataready = 0; // RT barometer.c feed status

// INLINES:

inline int16_t get_barometer_temp_rtf(void) {
    return il_temperature;
} //  Runtime barometer.c temperature feed
inline int32_t get_barometer_pres_rtf(void) {
    return il_pressure;
} //  Runtime barometer.c pressure feed
inline int32_t get_barometer_aslalt_est(void) {
    return il_aslaltitude;
} //  Runtime ASL altitude estimate

inline int16_t get_barometer_temp_ogn(void) {
    return il_temperature_ogn;
} //  temperature at origin
inline int32_t get_barometer_pres_ogn(void) {
    return il_pressure_ogn;
} // pressure at origin
inline int32_t get_barometer_aslalt_ogn(void) {
    return il_aslaltitude_ogn;
} //  ASL altitude estimate  at origin


// Point of origin and runtime PA-pressure and temperature sampling and definition

void barometerCalibrate(void) {

    static int32_t calpressure_sum = 0;
    static int16_t caltemperature_sum = 0;
    static int16_t dd = 0;
    static int16_t d = 0;
    static int16_t dc = 0;
    static int16_t ii = 0;
    static int16_t ww = 0;

    if (flags._.barometer_calalt_updtrun == 1 && flags._.bar_ucinitdone != 1) {
        dc = 4; // update run delay count eq. to 1 sec @ CHZ 10/4 Hz
        flags._.bar_ucinitdone = 1;
    } else {

        // First run requires a longer delay as defined in options.h
        // Let the barometer settle for a minimum of >4 seconds after power on. Depending on ambient
        // temperature, it reads quite a long way off w/in the first 2-3 seconds, leading to about
        // >2 M and 100 C of error if there's no wait.

        dc = CAL_WU_TD;
    }

    if (!flags._.d_init) {

        // Depending on cycle speed setting defined in options.h,
        //  auto adjust delay: 10 for 4Hz (def), 8 for 5Hz, 5 for 8Hz, 4 for 10Hz and 2 for
        //  Finaly, a SWAG to auto adjust delay timer (SWAG) to ensure 2 second delay for warm
        //  and about 4 seconds, for cold or lower than typical room temperature of 15 C.

        if (d < dc ) {
            d++ ;
        }
        if (d >= dc) {
            if (flags._.barometer_calalt_updtrun) {
                if (barometer_temperature >= room_temp) {
                    ww = 2;  // 3
                } else {
                    ww = 4;  // 5
                }
            } else {
                if (barometer_temperature >= room_temp) {
                    ww = 4;  // 5
                } else {
                    ww = 8;  // 7
                }
            }
            if (CHZ == 20) {
                dd = 3 * ww;
            } else if (CHZ == 10) {
                dd = 5 * ww;
            } else if (CHZ == 8) {
                dd = 8 * ww;
            } else if (CHZ == 5) {
                dd = 15 * ww;
            } else if (CHZ == 4) {
                dd = 22 * ww;
            } else if (CHZ == 2) {
                dd = 26 * ww;
            }
            flags._.d_init = 1;

            if (flags._.barometer_calalt_updtrun) {
                calpressure_sum = 0;
                caltemperature_sum = 0;
            }
        }
    }

    //  Calibrate ORIGIN barometer temperature and pressure data
    if (flags._.d_init && !flags._.i_init) {
        if (ii < dd) {
            ii++ ;
            caltemperature_sum += barometer_temperature;
            calpressure_sum += barometer_pressure;
        }
        if (ii >= dd) {
            ii /= 2;
            caltemperature_sum *= 0.5;
            calpressure_sum *= 0.5;
            caltemperature_sum = (int16_t) (caltemperature_sum / ii) ;
            calpressure_sum = (int32_t) (calpressure_sum / ii);
            caltemperature_ogn = caltemperature_sum;
            calpressure_ogn = calpressure_sum;

            il_temperature_ogn = caltemperature_ogn;
            il_pressure_ogn = calpressure_ogn;

            // Update runtime/inline variables with calibrated origin data
            il_temperature = caltemperature_ogn;
            il_pressure = calpressure_ogn;

            //  turn on flag to estimate origin altitude
            flags._.barometer_calalt_initrun = 1;
            flags._.barometer_calibrated = 1;
            flags._.i_init = 1;
            ii = 0;
        }
    }
}

// Retrieve barometer data, run calibration and estimation functions, populate inlines
#if (USE_BAROMETER == 1 && HILSIM != 1)

void udb_barometer_callback(int32_t pressure, int16_t temperature, uint8_t dataready) {
    static int32_t barometer_pressure_pvr = 0;
    static int16_t barometer_temperature_pvr = 0;
    barometer_dataready = dataready;

    if (!flags._.bar_cbinitdone) {
        initFlags();
        flags._.bar_cbinitdone = 1;
    }
    if (dataready == 1 && temperature > 0 && pressure >= PA_PRESSURE * BAR_PAPCT_THRESH) {
        il_pressure = pressure;
        il_temperature = temperature; 
        barometer_pressure_pvr = pressure;
        barometer_temperature_pvr = temperature;
        barometer_pressure = pressure;
        barometer_temperature = temperature;
    } else {
        //  For now pass on the previously validated readings
        //   TODO:  see if velocity scaling can be used to estimate best readings
        il_pressure = barometer_pressure_pvr;
        il_temperature = barometer_temperature_pvr;
        barometer_pressure = barometer_pressure_pvr;
        barometer_temperature = barometer_temperature_pvr;
    }

    //  0- barometerCntrl.c,  1- libDCM.c (40Hz, rec. def.);  2- altitudeCntrl.c (40Hz); 3- states.c (40Hz, high priority);
#if (BARCAL_TRIG == 0)
    if (!flags._.barometer_calibrated && flags._.fltrs_init) {
        barometerCalibrate();
    }
#endif
}
#endif  //(USE_BAROMETER == 1 && HILSIM != 1)

// Estimate the ASL altitude once calibration is finished

void estBarometerAltitude(void) {

    static int32_t aslaltitude_est = 0; // ASL (above sea level) altitude, in Cm

    // update-estimate the ASL altitude, initially with no offset after calibration at origin,
    if (flags._.barometer_calalt_initrun) {

        baralt_offset = 0;
        aslaltitude_est = calcSHABarASLAlt( calpressure_ogn, calpressure_ogn, caltemperature_ogn, caltemperature_ogn);

        // compute the offset as part of calibration
        baralt_offset = estBarAltOffset(aslaltitude_est);
        aslaltitude_est += baralt_offset;

        // Synchronize runtime/inline variables with calibrated origin data
        il_pressure = calpressure_ogn;
        il_temperature = caltemperature_ogn;

        il_aslaltitude_ogn = aslaltitude_est;
        il_aslaltitude = aslaltitude_est;

        //  re-initialize flags to let this run again when triggered from an update RT function
        flags._.barometer_calalt_initrun = 0;
        flags._.barometer_calalt_ready = 1;

        //  tidy up
        if (flags._.barometer_calalt_updtrun) {
            flags._.barometer_calalt_updated = 1;
            flags._.barometer_calalt_updtrun = 0;
        }
    } else {
        aslaltitude_est = calcSHABarASLAlt(barometer_pressure, calpressure_ogn, barometer_temperature, caltemperature_ogn);
        il_aslaltitude = aslaltitude_est;
        flags._.barometer_alt_ready = 1;
    }

}

// IMPORTANT: THESE ARE COMPUTATIONS STILL TO BE FLIGHT TESTED TO SEE HOW IT PERFORMS UNDER VARYING
//  ATMOSPHERIC CONDITIONS

// This is using a super high accuracy barometric altitude computation, technicaly, +-1.5 M of the standard
// atmosphere tables in the troposphere (up to 11,000 m amsl), however requires faster cpus

int32_t calcSHABarASLAlt(int32_t pres_rt, int32_t pres_ogn, int16_t temp_rt, int16_t temp_ogn) {
    float scaling = 0, temp = 0;
    int32_t alt_asl = 0;
    if (pres_rt != pres_ogn) {
        scaling = (float) pres_rt / (float) pres_ogn;
    } else {
        scaling = 1.0f;
    }
    if (temp_ogn != temp_rt) {
        temp = (float) ((temp_ogn + temp_rt) >> 1) + 273.15f;
    } else {
        temp = (float) temp_ogn + 273.15f;
    }
    alt_asl = (int32_t) (153.8462f * temp * (1.0f - expf(0.190259f * logf(scaling))));
    return alt_asl + baralt_offset;
}

void initFlags(void) {
    flags._.barometer_calibrated = 0;
    flags._.barometer_calalt_ready = 0;
    flags._.barometer_calalt_updated = 0;
    flags._.barometer_calalt_initrun = 0;
    flags._.barometer_calalt_updtrun = 0;
    flags._.barometer_alt_ready = 0;
    flags._.bar_ucinitdone = 0;
    flags._.gps_locked = 0;
    flags._.i_init = 0;
    flags._.d_init = 0;
}

// Altitude offset in centimeters subtracted/added to barometric ASL altitude. This is used to
//  allow for the automatic adjustment/correction of the base barometric altitude by a ground .
//  station equipped with a barometer. The value is subtracted/added above to the barometric
//  altitude read by the aircraft.

int32_t estBarAltOffset(int32_t estaslalt) {
    int32_t offset = 0;
    if (ASL_GROUND_ALT != estaslalt) {
        offset = ASL_GROUND_ALT - estaslalt;
    } else {
        offset = ASL_GROUND_ALT;
    }
    return offset;
}


// GOOFY'S PARKING LOT

/*  **Deprecated**

// Determine which calculation scaling level to use as defined in options.h

// Determine which calculation scaling level to use as defined in options.h

int32_t calcBarASLAlt(int32_t pres_rt, int32_t pres_ogn, int16_t temp_ogn) {
    int32_t asl_alt = 0;
#if  (BARAE_SCALE==1)
    asl_alt = (int32_t)calcSMABarASLAlt(pres_rt, pres_ogn, temp_ogn);
#elif  (BARAE_SCALE==2)
    asl_alt = (int32_t)calcSHABarASLAlt(pres_rt, pres_ogn, temp_ogn);
#endif
    return asl_alt + baralt_offset;
}

// This is using a super-high accurate barometric altitude computation, technicaly, +-2.5 M of the standard
// atmosphere tables in the troposphere (up to 11,000 m amsl), however requires faster cpus

int32_t calcVHABarASLAlt(int32_t pres_rt, int32_t pres_ogn, int16_t temp_ogn) {
    float scaling = 0, temp = 0;
    int32_t alt_asl = 0;
    if (pres_rt != pres_ogn) {
        scaling = (float) pres_rt / (float) pres_ogn;
    } else {
        scaling = 1.0f;
    }
    temp = (float) temp_ogn + 273.15f;
    alt_asl = (int32_t) (153.8462f * temp * (1.0f - expf(0.190259f * logf(scaling))));
    return alt_asl + baralt_offset;
}
 * 
// on slower board/CPUs, use a less semi-medium high, +- 4m, and faster, less demanding calculation

int32_t calcSMABarASLAlt(int32_t pres_rt, int32_t pres_ogn, int16_t temp_ogn) {
    float scaling = 0, temp = 0;
    float alt_asl = 0;
    scaling = (float) pres_ogn / (float) pres_rt;
    temp = (float) temp_ogn + 273.15f;
    alt_asl = logf(scaling) * temp * 29.271267f;
    return (int32_t) alt_asl;
}

 *
// simple altitude computation, less exact, faster, less demanding calculation

int32_t calcNScBarASLAlt(int32_t pres_rt) {
    float alt_asl = 0;
    float pa_pressure = 0;
    pa_pressure = (float) (pres_rt * 10); // convert to PA
#if (USE_PA_PRESSURE==1)  // eg. 101325 defined in options.h option, average or current SL Pa (newton / sq. M)
    static const float p0 = (float) PA_PRESSURE; // Current pressure at sea level (SL Pa) defined in options.h
#elif (USE_PA_PRESSURE==2)  // use mercury value
    static const float p0 = (float) (MC_PRESSURE / 0.0295333727112); // Current pressure at sea level (SL METAR  mercury in.) defined in options.h
#elif (USE_PA_PRESSURE==3)  // fuse Pa and mercury value
    static const float p0 = (float) ((PA_PRESSURE * 0.5f)+((MC_PRESSURE / 0.0295333727112)*0.5f));
#endif
    alt_asl = (float) 44330 * (1 - pow(((float) pa_pressure / p0), 0.190295));
    return (int32_t) alt_asl;
}
 
 *
 *  */



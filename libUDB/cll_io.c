// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2017 MatrixPilot Team
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
#include "interrupt.h"
#include "cll_io.h"

void cll_set_pullup(uint8_t active);


#if (USE_CASTLE_LINK_THROTTLE == 1)


// Handle Pin Setup

// CASTLE_LINK_LIVE hardware only available on udb5mini
// (You could hack up your own support on a udb5 using external resistors...)
#define CASTLE_LINK_THROTTLE_INPUT 8 // Input 8
#define CASTLE_LINK_PULLUP_PIN    G1

#define IC_TRIS8            _TRISD15
#define IC_LAT8             _LATD15

#define IC1VAL 0x0081
#define IC2VAL 0 // SYNCSEL = 0x00: no sync, no trigger, rollover at 0xFFFF

#define _IC_INIT(x) \
{ \
	IC##x##CON = IC2VAL; \
	IC##x##CON = IC1VAL; \
	_IC##x##IP = INT_PRI_IC; \
	_IC##x##IF = 0; \
	_IC##x##IE = 1; \
}
#define IC_INIT(x) _IC_INIT(x)


void cll_setup(void)
{
    IC_INIT(CASTLE_LINK_THROTTLE_INPUT);
    cll_set_pullup(1);
}


// Handle Output of throttle signal

#define CAT(a, ...) _CAT(a, __VA_ARGS__)
#define _CAT(a, ...) a ## __VA_ARGS__

#define SET_CLL_PIN(active) \
{ \
    if (active) { \
        CAT(CAT(_IC, CASTLE_LINK_THROTTLE_INPUT),IE) = 0; \
        CAT(CAT(_IC, CASTLE_LINK_THROTTLE_INPUT),IF) = 0; \
        CAT(IC_LAT, CASTLE_LINK_THROTTLE_INPUT) = 0; \
        CAT(IC_TRIS, CASTLE_LINK_THROTTLE_INPUT) = 0; \
     } \
    else { \
        CAT(IC_TRIS, CASTLE_LINK_THROTTLE_INPUT) = 1; \
    } \
}
#define SET_PULLUP(active) \
{ \
    CAT(_TRIS, CASTLE_LINK_PULLUP_PIN) = !active; \
    CAT(_LAT, CASTLE_LINK_PULLUP_PIN) = 1;  \
}
void cll_set_pullup(uint8_t active)
{
    SET_PULLUP(active);
}

void cll_start_servo_out(unsigned int scaled_duration, unsigned int scaled_skip_duration)
{
    if (scaled_duration > 0) {
        PR4 = scaled_duration;
        SET_PULLUP(0);
        SET_CLL_PIN(1);
    }
    else {
        PR4 = scaled_skip_duration;
        SET_CLL_PIN(0);
        SET_PULLUP(1);
    }
    TMR4 = 0;
}



// Stop outputting servo signal, and prepare to read values back from the ESC

static unsigned int CLL_StartTime = 0;
static uint8_t CLL_itemIndex = 0;
void CLL_addItem(unsigned int t);
void CLL_scaleValues();


#define _CLL_START_LISTENING(cllPin) \
void cll_start_listening(void) \
{ \
    CLL_StartTime = TMR2; \
    \
    static uint16_t lastIndex = 0; \
    if (lastIndex == CLL_itemIndex) { \
        CLL_itemIndex = 0; \
    } \
    lastIndex = CLL_itemIndex; \
    IC_TRIS##cllPin = 1; \
    SET_PULLUP(1); \
	_IC##cllPin##IF = 0; \
	_IC##cllPin##IE = 1; \
}
#define CLL_START_LISTENING(cllPin) _CLL_START_LISTENING(cllPin)
CLL_START_LISTENING(CASTLE_LINK_THROTTLE_INPUT);



// Handle reading values back from the ESC

#define _CLL_HANDLER(x) \
void __attribute__((__interrupt__,__no_auto_psv__)) _IC##x##Interrupt(void) \
{ \
	indicate_loading_inter; \
	interrupt_save_set_corcon; \
	uint16_t time = 0; \
	while (IC##x##CONbits.ICBNE) \
		time = IC##x##BUF; \
    if (time - CLL_StartTime > 950) { \
    	CLL_addItem(time - CLL_StartTime); \
        _IC##x##IE = 0; \
    } \
    _IC##x##IF = 0; \
	interrupt_restore_corcon; \
}
#define CLL_HANDLER(x) _CLL_HANDLER(x)
CLL_HANDLER(CASTLE_LINK_THROTTLE_INPUT);



// 11 values are sent by the ESC as per the spec -- see below
#define NUM_CCL_VALS 11
unsigned int CLL_rawValues[NUM_CCL_VALS];

void CLL_addItem(unsigned int t)
{
    if (CLL_itemIndex < NUM_CCL_VALS) {
        CLL_rawValues[CLL_itemIndex++] = t;
        if (CLL_itemIndex == NUM_CCL_VALS) {
            CLL_scaleValues();
        }
    }
}

void CLL_scaleValues(void)
{
    // should be 2000 and 1000
    uint16_t oneMs = CLL_rawValues[0];
    uint16_t halfMs = (CLL_rawValues[9] < CLL_rawValues[10]) ? CLL_rawValues[9] : CLL_rawValues[10];
    uint16_t refRange = oneMs - halfMs;
    
    // calibrate values
    uint16_t scaledV, scaledA;

    if (CLL_rawValues[1] < halfMs) {
        scaledV = 0;
    }
    else {
        scaledV = ((CLL_rawValues[1] - (uint32_t)halfMs) * 1000) / refRange;
    }
    
    if (CLL_rawValues[3] < halfMs) {
        scaledA = 0;
    }
    else {
        scaledA = ((CLL_rawValues[3] - (uint32_t)halfMs) * 1000) / refRange;
    }
    
    // scale to appropriate units
    scaledV = scaledV * (uint32_t)200 / 2000;
    scaledA = scaledA * (uint32_t)500 / 2000;
    
    battery_voltage.WW = ((uint32_t)scaledV) << 16;
    battery_current.WW = ((uint32_t)scaledA) << 16;
}

/*
 From the Castle Link Live spec here:
 http://www.castlecreations.com/castle-link-live
 
index (Data Item) (Scale Units/ms) (Max Pulse Length) (Max Value)
 0 Calibration 1ms
 1 Voltage 20 Volts 5 100
 2 Ripple Voltage 4 Volts 5 20
 3 Current 50 Amps 5 250
 4 Throttle 1 Milliseconds 2.5 2.5
 5 Output Power 0.2502 Percent 4 1
 6 RPM 20,416.7 Electrical RPM 5 100,000
 7 BEC Voltage 4 Volts 5 20
 8 BEC Current 4 Amps 5 20
 9 Temperature (Linear) 30 Degrees C 5 150
10 Temperature (NTC) 63.8125 Units ** 4 255
*/

#endif

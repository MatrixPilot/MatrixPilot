/* 
 * File:   config_AeroQuad.h
 * Author: markw
 *
 * Created on February 24, 2013, 10:27 AM
 */

#ifndef CONFIG_AEROQUAD_H
#define CONFIG_AEROQUAD_H

#include "p33FJ256GP710A.h"
#include "../../libUDB/ConfigUDB4.h"
#include "../../libUDB/libUDB_defines.h"

// tail light output (inverted)
#define TAIL_LIGHT  _LATD4

// aircraft specific functions

static inline void tail_light_on(void) {
    LED_BLUE = LED_ON;
    TAIL_LIGHT = LED_ON;
}

static inline void tail_light_off(void) {
    LED_BLUE = LED_OFF;
    TAIL_LIGHT = LED_OFF;
}

// return true if tail light is off
static inline boolean tail_light_toggle(void) {
    LED_BLUE = 1 - LED_BLUE;
    TAIL_LIGHT = 1 - TAIL_LIGHT;
    return (TAIL_LIGHT == LED_OFF);
}

#endif

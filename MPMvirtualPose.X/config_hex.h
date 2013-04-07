/* 
 * File:   config_hex.h
 * Author: markw
 *
 * Created on February 24, 2013, 11:04 AM
 */

#ifndef CONFIG_HEX_H
#define CONFIG_HEX_H

#include "p33FJ256GP710A.h"
#include "ConfigUDB4.h"
#include "libUDB_defines.h"

// X config left/right red/green LEDs (noninverted) OC7
#define TAIL_LIGHT  _LATD6
// X config rear left/right yellow LEDs (noninverted) OC8
#define REAR_LIGHT _LATD7
// X config front left/right blue LEDs (noninverted) IC8
#define FRONT_LIGHT _LATD15

// aircraft specific functions

static inline void tail_light_on(void) {
    TAIL_LIGHT = 1;
}

static inline void tail_light_off(void) {
    TAIL_LIGHT = 0;
}

static inline boolean tail_light_toggle(void) {
    TAIL_LIGHT = 1 - TAIL_LIGHT;
    return (TAIL_LIGHT == LED_OFF);
}

static inline void front_light_on(void) {
    FRONT_LIGHT = 1;
}

static inline void front_light_off(void) {
    FRONT_LIGHT = 0;
}

static inline boolean front_light_toggle(void) {
    FRONT_LIGHT = 1 - FRONT_LIGHT;
    return (FRONT_LIGHT == 1);
}

static inline void rear_light_on(void) {
    REAR_LIGHT = 1;
}

static inline void rear_light_off(void) {
    REAR_LIGHT = 0;
}

static inline boolean rear_light_toggle(void) {
    REAR_LIGHT = 1 - REAR_LIGHT;
    return (REAR_LIGHT == 1);
}
#endif

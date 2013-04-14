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
// X config front left / rear right blue/yellow LEDs (noninverted) OC8
#define FLRR_LIGHT _LATD7
// X config front right / rear left blue/yellow LEDs (noninverted) IC8
#define FRRL_LIGHT _LATD15

// aircraft specific functions
static inline void init_aircraft() {
    // using IC8 pin D15 as output for FRRL lights
    _TRISD15 = 0;
}

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

static inline void flrr_light_on(void) {
    FRRL_LIGHT = 1;
}

static inline void flrr_light_off(void) {
    FRRL_LIGHT = 0;
}

static inline boolean flrr_light_toggle(void) {
    FRRL_LIGHT = 1 - FRRL_LIGHT;
    return (FRRL_LIGHT == 1);
}

static inline void frrl_light_on(void) {
    FLRR_LIGHT = 1;
}

static inline void frrl_light_off(void) {
    FLRR_LIGHT = 0;
}

static inline boolean frrl_light_toggle(void) {
    FLRR_LIGHT = 1 - FLRR_LIGHT;
    return (FLRR_LIGHT == 1);
}

static inline void all_lights_off(void) {
    tail_light_off();
    flrr_light_off();
    frrl_light_off();
}

static int lightState = 0;

static inline void lightFSM(void) {
    switch (lightState) {
        case 0:
            frrl_light_on();
            tail_light_off();
            flrr_light_off();
            lightState = 1;
            break;
        case 1:
            frrl_light_off();
            tail_light_on();
            flrr_light_off();
            lightState = 2;
            break;
        case 2:
            frrl_light_off();
            tail_light_off();
            flrr_light_on();
            lightState = 0;
            break;
    }
}

#endif

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


#include "libUDB.h"

extern volatile uint16_t trap_flags;
extern volatile uint32_t trap_source;
extern volatile uint16_t osc_fail_count;

// Get flags telling the reason for the last reset (RCON)
uint16_t get_reset_flags(void);

inline void udb_init_leds(void);
inline void udb_init_ADC(void);
inline void udb_init_clock(void);
inline void udb_init_capture(void);
inline void udb_init_GPS(void);
inline void udb_init_USART(void);
inline void udb_init_pwm(void);
inline void udb_eeprom_init(void);

inline void MPU6000_init16(void);

inline void start_pwm_outputs(void);

inline void calculate_analog_sensor_values(void);

inline void radioIn_failsafe_check(void);
inline void radioIn_failsafe_reset(void);

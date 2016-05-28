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

#ifndef LED_WING_H
#define	LED_WING_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define LED_MODE_OFF            0    
#define LED_MODE_ON             1    
#define LED_MODE_BLINK          2
#define LED_MODE_PATTERN        3    
#define LED_MODE_PATTERN_INV    4    

#define LED_ALL_WING            255    
#define LED_RIGHT_WING          0
#define LED_LEFT_WING           1
    
    
struct Led {
    uint8_t mode;
    uint8_t offset;
    uint8_t index;
};    
void led_wing_heartbeat_callback();
void led_wing_set_mode(uint8_t led_num, uint8_t mode, uint8_t offset);


#ifdef	__cplusplus
}
#endif

#endif	/* LED_WING_H */


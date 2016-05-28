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

#include "../MatrixPilot/defines.h"

#define LED_COUNTER_MAX 20
#define NUM_LED_WING 2      // Max 255 leds

uint16_t led_counter = 0;
uint8_t  led_pattern[LED_COUNTER_MAX] = {1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0};
struct Led led_wing[NUM_LED_WING];

void led_wing_heartbeat_callback()
{
#if (USE_WING_LED == 1)    
    led_wing[LED_RIGHT_WING].index = (led_counter + led_wing[LED_RIGHT_WING].offset) % LED_COUNTER_MAX;
    switch(led_wing[LED_RIGHT_WING].mode)
    {
        case LED_MODE_OFF:
            led_off_wing(LED_GREEN_EXT);                
            break;

        case LED_MODE_ON:
            led_on_wing(LED_GREEN_EXT);                
            break;

        case LED_MODE_BLINK:    // Offset variable used as blink frequency in Hz
            if (led_counter % (10/led_wing[LED_RIGHT_WING].offset) == 0)  // toggle twice the frequency (20/(led_wing[].offset *2))
                udb_led_toggle(LED_GREEN_EXT);               
            break;  

        case LED_MODE_PATTERN:
            LED_GREEN_EXT = led_pattern[led_wing[LED_RIGHT_WING].index];             
            break;

        case LED_MODE_PATTERN_INV:
            LED_GREEN_EXT = !led_pattern[led_wing[LED_RIGHT_WING].index];                
            break;       
    }        

    led_wing[LED_LEFT_WING].index = (led_counter + led_wing[LED_LEFT_WING].offset) % LED_COUNTER_MAX;
    switch(led_wing[LED_LEFT_WING].mode)
    {
        case LED_MODE_OFF:
            led_off_wing(LED_RED_EXT);                
            break;

        case LED_MODE_ON:
            led_on_wing(LED_RED_EXT);                
            break;

        case LED_MODE_BLINK:    // Offset variable used as blink frequency in Hz
            if (led_counter % (10/led_wing[LED_LEFT_WING].offset) == 0)  // toggle twice the frequency (20/(led_wing[].offset *2))
                udb_led_toggle(LED_RED_EXT);               
            break;  

        case LED_MODE_PATTERN:
            LED_RED_EXT = led_pattern[led_wing[LED_LEFT_WING].index];             
            break;

        case LED_MODE_PATTERN_INV:
            LED_RED_EXT = !led_pattern[led_wing[LED_LEFT_WING].index];                
            break;       
    }
    
	led_counter = (led_counter+1) % LED_COUNTER_MAX;
#endif    
}

void led_wing_set_mode(uint8_t led_num, uint8_t mode, uint8_t offset)
{   
    uint8_t startFor = 0;
    uint8_t endFor = 0;
    uint8_t i = 0;
    
    if(led_num == LED_ALL_WING){
        startFor = 0;
        endFor = NUM_LED_WING-1;
    }
    else{
        startFor = led_num;
        endFor = led_num;
    }  
        
    for(i=startFor; i<=endFor; i++)
    {
        switch(mode)
        {
            case LED_MODE_OFF:
                led_wing[i].mode     = LED_MODE_OFF;                
                break;

            case LED_MODE_ON:
                led_wing[i].mode     = LED_MODE_ON;               
                break;

            case LED_MODE_BLINK:    // Offset variable used as blink frequency
                led_wing[i].mode     = LED_MODE_BLINK;
                if(offset == 0)
                    led_wing[i].offset   = 1;      
                else if (offset > 10)   // led_wing_heatbeat_callback() is called at 20Hz, thus 10Hz max blinking frequency
                    led_wing[i].offset   = 10;      
                else
                    led_wing[i].offset   = offset;                          
                break;  

            case LED_MODE_PATTERN:
                led_wing[i].mode     = LED_MODE_PATTERN;
                led_wing[i].offset   = offset;                
                break;

            case LED_MODE_PATTERN_INV:
                led_wing[i].mode     = LED_MODE_PATTERN_INV;
                led_wing[i].offset   = offset;                
                break;       
        }
    }
}

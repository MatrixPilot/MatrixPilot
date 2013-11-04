/* 
 * File:   cbox.h
 * Author: markw
 *
 * Created on November 24, 2012, 8:30 PM
 */

#ifndef CBOX_H
#define	CBOX_H

#ifdef	__cplusplus
extern "C" {
#endif

    // GAIN_MENU control box definitions

    void inc_gain(int menu_item);
    void dec_gain(int menu_item);
    void cbox_write(const char* text);
    void cbox_display(int menu_item);
    void check_cbox();

    // globals for analog input from 5-way switch and cbox display state
    extern struct ADchannel udb_analogInputs[NUM_ANALOG_INPUTS]; // 0-indexed, unlike servo pwIn/Out/Trim arrays
    extern int cbox_on;

#ifdef	__cplusplus
}
#endif

#endif	/* CBOX_H */


/*
 * File:   cbox.c
 * Author: markw
 *
 * Created on November 24, 2012, 8:30 PM
 */

#include "options.h"
#ifdef ENABLE_GAIN_CBOX

#include "../../libUDB/libUDB.h"
#include "defines.h"
#include "cbox.h"
#include "ring_buffer.h"
#include "data_services.h"

static void storage_complete_callback(boolean success) {};

static void storeGains(void)
{
    data_services_save_all(STORAGE_FLAG_STORE_CALIB, &storage_complete_callback);
};

// control box menu item labels
//char* menu_label[N_MENU_ITEMS] = {"item 0", "item 1", "item 2", "item 3", "item 4",};
#define MENU_ITEMS_N (13)
#define GAIN_INC .005
char* menu_label[MENU_ITEMS_N] = {"0:yawkpail", "1:yawkdail",
    "2:rollkp", "3:rollkd",
    "4:pitchgain", "5:pitchkd",
    "6:rudderElevMixGain", "7:rollElevMixGain",
    "8:yawkprud", "9:yawkdrud",
    "10:aileronbgain", "11:elevatorbgain",
    "12:rudderbgain"};

extern uint16_t
yawkpail, yawkdail,
        rollkp, rollkd,
        pitchgain, pitchkd,
        rudderElevMixGain, rollElevMixGain,
        yawkprud, yawkdrud,
        aileronbgain, elevatorbgain,
        rudderbgain;

uint16_t* pGain[MENU_ITEMS_N] = {&yawkpail, &yawkdail,
    &rollkp, &rollkd,
    &pitchgain, &pitchkd,
    &rudderElevMixGain, &rollElevMixGain,
    &yawkprud, &yawkdrud,
    &aileronbgain, &elevatorbgain,
    &rudderbgain};

// measured actual voltages are 0, 0.86, 1.47, 2.17, 2.60, 3.12V
// convert midpoints to 16 bit signed fractional A/D format
#define THR1 ((0.43/3.3) * 65536 - 32768)
#define THR2 ((1.17/3.3) * 65536 - 32768)
#define THR3 ((1.82/3.3) * 65536 - 32768)
#define THR4 ((2.39/3.3) * 65536 - 32768)
#define THR5 ((2.86/3.3) * 65536 - 32768)

// GAIN_MENU control box definitions
#define BUTTON_UP       0
#define BUTTON_DOWN     1
#define BUTTON_LEFT     2
#define BUTTON_RIGHT    3
#define BUTTON_CLICK    4
#define BUTTON_NONE     5
#define N_MENU_ITEMS    13

int cbox_read(void)
{
    // control box button is AN15 = udb_analogInputs[0]
    // 0 = inactive
    // Up,down,left,right,center, none are 6 increasing voltages from 0 to 3.3V
    // target voltages are (0, .2, .4, .6, .8, 1.0) * VCC
    // resistor values are 2.5K, 6.7K, 15K, 40K
    // standard values: 2.49K, 6.65K, 15K, 40.2K
    // which are -1, -.6, -.2, +.2, +.6, +1 in 16 bit signed fractional form
    if (udb_analogInputs[0].input < THR1)
        return BUTTON_UP;
    else if (udb_analogInputs[0].input < THR2)
        return BUTTON_DOWN;
    else if (udb_analogInputs[0].input < THR3)
        return BUTTON_LEFT;
    else if (udb_analogInputs[0].input < THR4)
        return BUTTON_RIGHT;
    else if (udb_analogInputs[0].input < THR5)
        return BUTTON_CLICK;
    else
        return BUTTON_NONE;

}

static int previous_level = 0;

int debounce_cbox_button()
{
    int button_level = cbox_read();
    if (previous_level != button_level)
    {
        // value hasn't settled down yet
        previous_level = button_level;
        return -1;
    }
    else
        return button_level;
}

extern volatile char serial_interrupt_stopped;

void cbox_write(const char* text)
{
    // blocking serial output to telemetry port
    int16_t status;
    status = queue_string(text);
    if (status && (serial_interrupt_stopped == 1))
    {
        serial_interrupt_stopped = 0;
        udb_serial_start_sending_data();
    }
    // wait for completion
    while (!serial_interrupt_stopped)
    {
    }
}

// clear display: send 0xFE 0x01, set cursor to start of line 1: send 0xFE, 0x80
static const char cbox_clear[] = {0xFE, 0x01, 0xFE, 0x80, 0x0};
// set cursor to start of line 2: send 0xFE, 0x80 + 0x40
static const char cbox_line2[] = {0xFE, 0xC0, 0x0};
//const char cbox_delay[100] = {0xFF};
static char serial_buffer[256];
//extern struct gains_variables gains;

void cbox_init(void)
{
    udb_serial_set_rate(9600);
}

void cbox_display(int menu_item)
{
    cbox_write(cbox_clear);

    // display 1st line of text (menu item label)
    cbox_write(menu_label[menu_item]);

    // display 2nd line of text (field value)
    cbox_write(cbox_line2);
    snprintf(serial_buffer, sizeof (serial_buffer), "%7.3f", (double) *(pGain[menu_item]) / RMAX);
    cbox_write(serial_buffer);

    return;
}

void inc_gain(int menu_item)
{
    unsigned int delta = RMAX * GAIN_INC;
    if (*(pGain[menu_item]) <= (unsigned int) ((RMAX << 2) - delta))
    {
        *(pGain[menu_item]) += delta;
    }
}

void dec_gain(int menu_item)
{
    unsigned int delta = RMAX * GAIN_INC;
    if (*(pGain[menu_item]) >= delta)
    {
        *(pGain[menu_item]) -= delta;
    }
}

static boolean auto_repeat = false;
static int auto_count = 0;
static int button_state = BUTTON_CLICK;
static int menu_item = 0, last_button_state = 0;
static int toggle_cbox_on = 1;

void check_cbox()
{

    // use telemetry channel for connection to parameter control box
    // 2x16 LCD panel for menu and value display
    // button left/right: change selected gain and display its name on line 1
    // and value on line 2
    // button up/down: inc/dec selected gain
    // click: write selected value

    // perform action when button state changes from none to something else
    // (called at 20Hz)
    button_state = debounce_cbox_button();
    if (button_state >= 0)
    {
        // auto repeat if BUTTON_UP,DOWN,LEFT,RIGHT is held down
        if (button_state != BUTTON_CLICK)
        {
            auto_count++;
            if (auto_count > 10)
            {
                auto_repeat = true;
                auto_count = 0;
            }
        }
        if (button_state != last_button_state || auto_repeat)
        {
            auto_repeat = false;
            last_button_state = button_state;
            switch (button_state)
            {
            case BUTTON_LEFT:
                if (menu_item > 0) menu_item--;
                break;
            case BUTTON_RIGHT:
                if (menu_item < MENU_ITEMS_N - 1) menu_item++;
                break;
            case BUTTON_UP:
                inc_gain(menu_item);
                break;
            case BUTTON_DOWN:
                dec_gain(menu_item);
                break;
            case BUTTON_CLICK:
                // toggle between cbox display and telemetry output
                toggle_cbox_on = 1;
                break;
            }
        }
    }
    // toggle cbox on and off to allow enabling telemetry without reflashing
    if (toggle_cbox_on)
    {
        toggle_cbox_on = 0;
        cbox_on = 1 - cbox_on;
        if (cbox_on)
        {
            // set baud rate for serial LCD and display menu
            cbox_init();
            cbox_display(menu_item);
        }
        else
        {
            // save gains and init telemetry output
            storeGains();
            init_serial();
        }
    }
    if (cbox_on && button_state != BUTTON_NONE) cbox_display(menu_item);
}

#endif // ENABLE_GAIN_MENU
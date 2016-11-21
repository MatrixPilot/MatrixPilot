//
//  SIL-ui-term.c
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/10/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#if (WIN == 1 || NIX == 1)

#include "SIL-udb.h"
#include "UDBSocket.h"
#include "../../MatrixPilot/defines.h"
#include "../../MatrixPilot/states.h"
#include "../../MatrixPilot/config.h"
#include "../../MatrixPilot/flightplan.h"
#include "../../libDCM/hilsim.h"
#include <stdio.h>

#define BUFLEN 512

static UDBSocket stdioSocket = NULL;
static uint8_t lastLedBits = 0;
static boolean showLEDs = 0;
static uint8_t inputState = 0;
static int hasShownInitStates = 0;
static uint8_t leds[5] = {0, 0, 0, 0, 0};

static int sil_handle_key_input(char c);
static void sil_checkForLedUpdates(void);

void udb_led_toggle(uint8_t x)
{
	leds[x] = !leds[x];
}

void led_on(uint8_t x)
{
	leds[x] = LED_ON;
}

void led_off(uint8_t x)
{
	leds[x] = LED_OFF;
}

static void print_help(void)
{
	printf("1/2/3/4 = mode manual/stabilized/waypoint/signal-lost\n");
	printf("w/s     = throttle up/down\n");
	printf("a/d     = rudder left/right\n");
	printf("i/k     = elevator forward/back\n");
	printf("j/l     = aileron left/right\n");
	printf("\n");
	printf("z       = zero the sticks\n");
	printf(";       = toggle LEDs\n");
	printf("0       = toggle RC Radio connection on/off\n");
#if (FLIGHT_PLAN_TYPE == FP_LOGO)
	printf("xN      = execute LOGO subroutine N(0-9)\n");
#endif
	printf("r       = reset\n");
	printf("?       = show this help message\n");
}

void sil_ui_init(uint16_t mp_rcon)
{
	printf("MatrixPilot SIL%s\n\n", (mp_rcon == 128) ? " (HW Reset)" : "");
	print_help();
	printf("\nINIT: Calibrating...\n");

	stdioSocket = UDBSocket_init(UDBSocketStandardInOut, 0, NULL, NULL, 0);
}

void sil_ui_will_reset(void)
{
	if (stdioSocket) {
		UDBSocket_close(stdioSocket);
	}
}

void sil_ui_update(void)
{
	uint8_t buffer[BUFLEN];
	int32_t bytesRead;
	int16_t i;

	// Handle stdin
	if (stdioSocket) {
		bytesRead = UDBSocket_read(stdioSocket, buffer, BUFLEN);
		for (i = 0; i < bytesRead; i++) {
			sil_handle_key_input(buffer[i]);
		}
	}

	sil_checkForLedUpdates();

	if (hasShownInitStates == 0 && waggle != 0) {
		printf("INIT: sensors calibrated and trims recorded.\n");
		printf("INIT: waiting for GPS...\n");
		hasShownInitStates = 1;
	} else if (hasShownInitStates == 1 && dcm_flags._.dead_reckon_enable == 1) {
		printf("INIT: GPS link acquired.\n");
		printf("INIT: Ready.\n");
		hasShownInitStates = 2;
	}
}

static void print_LED_status(void)
{
	printf("LEDs: %c %c %c %c\r",
	    (leds[0] == LED_ON) ? 'R' : '-',
	    (leds[1] == LED_ON) ? 'G' : '-',
	    (leds[2] == LED_ON) ? 'O' : '-',
	    (leds[3] == LED_ON) ? 'B' : '-');
}

static void sil_checkForLedUpdates(void)
{
	uint8_t newLedBits = 0;

	if (leds[0] == LED_ON) newLedBits |= 1;
	if (leds[1] == LED_ON) newLedBits |= 2;
	if (leds[2] == LED_ON) newLedBits |= 4;
	if (leds[3] == LED_ON) newLedBits |= 8;

	if (lastLedBits	!= newLedBits) {
		if (showLEDs) {
			print_LED_status();
		}
		lastLedBits	= newLedBits;
	}
}

#define KEYPRESS_INPUT_DELTA 50

static int sil_handle_key_input(char c)
{
	switch (inputState) {
		case 0:
		{
			switch (c) {
				case '?':
					printf("\n");
					print_help();
					break;
				case 'w':
					hilsim_input_adjust("throttle", KEYPRESS_INPUT_DELTA*2);
					break;
				case 's':
					hilsim_input_adjust("throttle", -KEYPRESS_INPUT_DELTA*2);
					break;
				case 'a':
					hilsim_input_adjust("rudder", KEYPRESS_INPUT_DELTA);
					break;
				case 'd':
					hilsim_input_adjust("rudder", -KEYPRESS_INPUT_DELTA);
					break;
				case 'i':
					hilsim_input_adjust("elevator", KEYPRESS_INPUT_DELTA);
					break;
				case 'k':
					hilsim_input_adjust("elevator", -KEYPRESS_INPUT_DELTA);
					break;
				case 'j':
					hilsim_input_adjust("aileron", KEYPRESS_INPUT_DELTA);
					break;
				case 'l':
					hilsim_input_adjust("aileron", -KEYPRESS_INPUT_DELTA);
					break;
				case 'z':
					hilsim_input_adjust("stick", 0);
					break;
				case '1': // switch mode to manual
					hilsim_input_adjust("mode", 1);
					break;
				case '2': // switch mode to stabilised
					hilsim_input_adjust("mode", 2);
					break;
				case '3': // switch mode to guided
					hilsim_input_adjust("mode", 3);
					break;
				case '4': // switch mode to failsafe
					hilsim_input_adjust("mode", 4);
					break;
				case '0':
					sil_radio_on = !sil_radio_on;
					printf("\nRadio %s\n", (sil_radio_on) ? "On" : "Off");
					break;
				case ';':
					showLEDs = !showLEDs;
					if (showLEDs) {
						printf("\n");
						print_LED_status();
					}
					break;
#if (FLIGHT_PLAN_TYPE == FP_LOGO)
				case 'x':
					inputState = 1;
					break;
#endif
				case 'r':
					printf("\nReally reset? (y/N)");
					fflush(stdout);
					inputState = 2;
					break;
				case '9':
					printf("Saving Params to ini file\r\n");
					config_save();
					break;
				default:
					return 0;
			}
			break;
		}
		case 1:
		{
			if (c >= '0' && c <= '9') {
				printf("\nExecuting LOGO subroutine #%c\n", c);
				flightplan_live_begin() ;
				flightplan_live_received_byte(10) ; // Exec command
				flightplan_live_received_byte(c-'0') ; // Subroutine #
				flightplan_live_received_byte(0) ; // Don't use param or set fly bit
				flightplan_live_received_byte(0) ; // Exec command
				flightplan_live_received_byte(0) ; // Exec command
				flightplan_live_commit() ;
			}
			inputState = 0;
			break;
		}
		case 2:
		{
			if (c == 'y') {
				printf("\nReset MatrixPilot...\n\n\n");
				sil_reset();
			}
			inputState = 0;
			break;
		}
	}
	return 1;
}

#endif // (WIN == 1 || NIX == 1)

//
//  libSTM.c
//  MatrixPilot
//
//  Created by Robert Dickenson on 17/6/2014.
//  Copyright (c) 2014 MatrixPilot. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../libUDB/libUDB.h"
#include "../libUDB/ADchannel.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/magnetometer.h"
#include "magnetometerOptions.h"
#include "../libUDB/events.h"
#include "../libUDB/osd.h"
#include "../libUDB/mpu6000.h"

#include "../libUDB/uart.h"

#if (BOARD_TYPE == PX4_BOARD)

//#include "stm32f4xx_hal.h"
//#include "usart.h"

//#include "cmsis_os.h"
//#include "ff.h"
//#include "ff_gen_drv.h"
//#include "sd_diskio.h" /* defines SD_Driver as external */

//#include "SIL-config.h"
//#include "SIL-udb.h"
//#include "SIL-ui.h"
//#include "SIL-events.h"
//#include "SIL-eeprom.h"

uint16_t udb_heartbeat_counter;

int16_t udb_pwIn[MAX_INPUTS];   // pulse widths of radio inputs
int16_t udb_pwTrim[MAX_INPUTS]; // initial pulse widths for trimming
int16_t udb_pwOut[MAX_OUTPUTS]; // pulse widths for servo outputs

union udb_fbts_byte udb_flags;

struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel;    // x, y, and z accelerometer channels
struct ADchannel udb_xrate, udb_yrate, udb_zrate;       // x, y, and z gyro channels
struct ADchannel udb_vref;                              // reference voltage
struct ADchannel udb_analogInputs[4];

int16_t udb_magFieldBody[3];                    // magnetic field in the body frame of reference
int16_t udb_magOffset[3] = { 0 , 0 , 0 };       // magnetic offset in the body frame of reference
int16_t magGain[3] = { RMAX , RMAX , RMAX };    // magnetometer calibration gains
int16_t rawMagCalib[3] = { 0 , 0 , 0 };
uint8_t magreg[6];                              // magnetometer read-write buffer
int16_t magFieldRaw[3];

union longww battery_current;                   // battery_current._.W1 is in tenths of Amps
union longww battery_mAh_used;                  // battery_mAh_used._.W1 is in mAh
union longww battery_voltage;                   // battery_voltage._.W1 is in tenths of Volts
uint8_t rc_signal_strength;                     // rc_signal_strength is 0-100 as percent of full signal

int16_t magMessage;
int16_t vref_adj;

volatile uint16_t trap_flags;
volatile uint32_t trap_source;
volatile uint16_t osc_fail_count;
uint16_t mp_rcon = 3;                           // default RCON state at normal powerup

extern int mp_argc;
extern char **mp_argv;

uint8_t leds[5] = {0, 0, 0, 0, 0};
uint8_t sil_radio_on;

// Functions only included with nv memory.
#if (USE_NV_MEMORY == 1)
UDB_SKIP_FLAGS udb_skip_flags = {0, 0, 0};

void udb_skip_radio_trim(boolean b)
{
	udb_skip_flags.skip_radio_trim = 1;
}

void udb_skip_imu_calibration(boolean b)
{
	udb_skip_flags.skip_imu_cal = 1;
}
#endif

uint8_t udb_cpu_load(void)
{
	return 5; // sounds reasonable for a fake cpu%
}

int16_t udb_servo_pulsesat(int32_t pw)
{
	if (pw > SERVOMAX) pw = SERVOMAX;
	if (pw < SERVOMIN) pw = SERVOMIN;
	return (int16_t)pw;
}

void udb_servo_record_trims(void)
{
	int16_t i;

	for (i = 1; i <= NUM_INPUTS; i++)
	{
		udb_pwTrim[i] = udb_pwIn[i];
//		DPRINT("udb_pwTrim[%i] = %u\r\n", i, udb_pwTrim[i]);
	}
}

void udb_set_action_state(boolean newValue)
{
	// not simulated
	(void)newValue;   // unused parameter
}

void udb_a2d_record_offsets(void)
{
	UDB_XACCEL.offset = UDB_XACCEL.value;
	udb_xrate.offset  = udb_xrate.value;
	UDB_YACCEL.offset = UDB_YACCEL.value - (Y_GRAVITY_SIGN ((int16_t)(2*GRAVITY))); // opposite direction
	udb_yrate.offset  = udb_yrate.value;
	UDB_ZACCEL.offset = UDB_ZACCEL.value;
	udb_zrate.offset  = udb_zrate.value;
	udb_vref.offset   = udb_vref.value;
}

uint16_t get_reset_flags(void)
{
	return mp_rcon;
}

#if (MAG_YAW_DRIFT == 1)

static magnetometer_callback_funcptr magnetometer_callback = NULL;

void rxMagnetometer(magnetometer_callback_funcptr callback)
{
	magnetometer_callback = callback;
}

void I2C_doneReadMagData(void)
{
	magFieldRaw[0] = (magreg[0]<<8) + magreg[1];
	magFieldRaw[1] = (magreg[2]<<8) + magreg[3];
	magFieldRaw[2] = (magreg[4]<<8) + magreg[5];

	if (magMessage == 7)
	{
		udb_magFieldBody[0] = MAG_X_SIGN((__builtin_mulsu((magFieldRaw[MAG_X_AXIS]), magGain[MAG_X_AXIS]))>>14)-(udb_magOffset[0]>>1);
		udb_magFieldBody[1] = MAG_Y_SIGN((__builtin_mulsu((magFieldRaw[MAG_Y_AXIS]), magGain[MAG_Y_AXIS]))>>14)-(udb_magOffset[1]>>1);
		udb_magFieldBody[2] = MAG_Z_SIGN((__builtin_mulsu((magFieldRaw[MAG_Z_AXIS]), magGain[MAG_Z_AXIS]))>>14)-(udb_magOffset[2]>>1);

		if ((abs(udb_magFieldBody[0]) < MAGNETICMAXIMUM) &&
			(abs(udb_magFieldBody[1]) < MAGNETICMAXIMUM) &&
			(abs(udb_magFieldBody[2]) < MAGNETICMAXIMUM))
		{
//			udb_magnetometer_callback();
			if (magnetometer_callback != NULL)
			{
				magnetometer_callback();
			}
			else
			{
				printf("ERROR: magnetometer_callback function pointer not set\r\n");
			}
		}
		else
		{
			magMessage = 0;         // invalid reading, reset the magnetometer
		}
	}
}

void HILSIM_MagData(magnetometer_callback_funcptr callback)
{
	(void)callback;
//	magnetometer_callback = callback;
	magMessage = 7;                 // indicate valid magnetometer data
	I2C_doneReadMagData();          // run the magnetometer computations
}

#endif // MAG_YAW_DRIFT

int16_t FindFirstBitFromLeft(int16_t val)
{
	int16_t i = 0;

	if (val != 0)
	{
		for (i = 1; i <= 16; i++)
		{
			if (val & 0x8000) break;
			val <<= 1;
		}
	}
	return i;
}

void udb_led_toggle(uint8_t x)
{
//    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}
void led_on(uint8_t x)
{
//void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
}
void led_off(uint8_t x)
{
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
}

int32_t gps_baud_rate = 0;
int32_t serial_baud_rate = 0;

void udb_gps_start_sending_data(void) {}
void udb_gps_set_rate(int32_t rate) { gps_baud_rate = rate; }
boolean udb_gps_check_rate(int32_t rate) { return (gps_baud_rate == rate); }

void udb_serial_start_sending_data(void) {}
void udb_serial_set_rate(int32_t rate) { serial_baud_rate = rate; }

uint16_t register_event_p(void (*event_callback)(void), eventPriority priority) { return 0; }
void trigger_event(uint16_t hEvent) {}

void osd_init(void) {}
void osd_reset(void) {}
void osd_spi_init(void) {}
void osd_spi_write(int8_t address, int8_t byte) {}
void osd_spi_write_byte(int8_t byte) {}
void osd_spi_write_location(int16_t loc) {}
void osd_spi_write_string(const uint8_t* str) {}
void osd_spi_write_vertical_string_at_location(int16_t loc, const uint8_t* str) {}
void osd_spi_erase_chars(uint8_t n) {}
void osd_spi_write_number(int32_t val, int8_t num_digits, int8_t decimal_places, int8_t num_flags, int8_t header, int8_t footer) {}

void filesys_init(void) {}

//static jmp_buf default_jmp_buf;

int setjmp(void)
{
	return 0;
}

void __delay32(unsigned long cycles)
{
    /* Insert delay in ms */
//    HAL_Delay(cycles);
}

//#if (CONSOLE_UART == 0)
//void console(void) {}
//#endif // CONSOLE_UART

//void Reset_Handler(void) {} // this must be loosely defined in the startup code and the default seems to call main()
//void mcu_init(void) {} // now provided by main() in the STMCubeMX generated code (redef'd to mcu_init()

void heartbeat(void) // called from MPU6000 ISR
{
}

void udb_init(void)
{
	udb_heartbeat_counter = 0;
	udb_flags.B = 0;
//	MPU6000_init16(&heartbeat);
}

void mcu_init(void)
{
}

void udb_run(void)
{
}

void udb_background_trigger(background_callback callback)
{
	if (callback) callback();
}

////////////////////////////////////////////////////////////////////////////////

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
// CONSOLE_UART
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
//  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
  // we want to signal the process responsible for this uart
//}

//void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);
//void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
//{
//}
/*
void vApplicationIdleHook(void)
{
	{
#if (USE_TELELOG == 1)
		telemetry_log();
#endif
#if (USE_USB == 1)
		USBPollingService();
#endif
#if (CONSOLE_UART != 0 && SILSIM == 0)
		console();
#endif
		udb_run();
	}
}
 */

void ClrError(void)
{
//	if (U##x##STAbits.OERR) U##x##STAbits.OERR = 0;
}

uint8_t buffered_char = 0;
uint8_t buffered_full = 0;

char IsPressed(void)
{
//	HAL_StatusTypeDef status;
	uint8_t Data[2];

// HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

	if (buffered_full == 1)
	{
		return 1;
	}

//	status = HAL_UART_Receive(&huart2, &Data[0], 1, 0);
//	if (status == HAL_OK)
	{
		buffered_char = Data[0];
		buffered_full = 1;
		return 1;
	}

//	if (U##x##STAbits.URXDA)
	return 0;
}

char GetChar(void)
{
//	while (!IsPressed());
//	Temp = U##x##RXREG;

	char Temp;
	uint8_t Data[2];
//	HAL_StatusTypeDef status;

	if (buffered_full == 1)
	{
		Temp = buffered_char;
		buffered_full = 0;
	}
	else
	{
//		status = HAL_UART_Receive(&huart6, &Data[0], 1, 0);
//		if (status == HAL_OK)
		{
			Temp = Data[0];
		}
	}
//	ClrError();
	return Temp;
}

void PutChar(char ch)
{
//HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

//	HAL_StatusTypeDef status;

//	status = HAL_UART_Transmit(&huart6, &ch, 1, 0);
//	if (status == HAL_OK)
//	{
//	}

//	U##x##TXREG = ch;
//	while (U##x##STAbits.TRMT == 0);
}

//void _init(void) {}

#endif // (BOARD_TYPE == PX4_BOARD)

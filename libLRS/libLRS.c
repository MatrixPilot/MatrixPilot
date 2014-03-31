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
//

#if defined( __dsPIC30F__ ) || defined( __dsPIC33F__ ) || defined(__dsPIC33E__)

#include "../libLRS/libLRS.h"

#include "../libUDB/libUDB_internal.h"
#include "../libUDB/oscillator.h"
//#include "../libUDB/interrupt.h"
#include "../libUDB/eeprom_udb4.h"
#include "../libUDB/uart.h"
#include "../libUDB/lrsIn.h"
#include "../libUDB/RFM22B.h"

//uint32_t micros(void);
//uint32_t millis(void);
uint8_t get_serial_bytes(uint8_t* buf, uint8_t buflen);
uint8_t put_serial_bytes(uint8_t* buf, uint8_t buflen);
uint8_t put_serial_byte(uint8_t data);
void lrsIn_setInput(volatile uint16_t PPM[], uint8_t channels);
void echoBindData(void);
void echoRxConfig(void);


void init_micros(void)
{
	// 32 bit Timer 8/9 free runs
	T9CONbits.TON = 0;      // Disable Timer
	T8CONbits.TON = 0;      // Disable Timer
	TMR9 = 0x00;            // Clear timer register
	TMR8 = 0x00;            // Clear timer register
	PR9 = 0xFFFF;           // period 2^32 cycles
	PR8 = 0xFFFF;

#if (MIPS == 16)
	T8CONbits.TCKPS = 1;    // prescaler = 8
#elif (MIPS == 32)
	T8CONbits.TCKPS = 1;    // prescaler = 8
#elif (MIPS == 64)
	T8CONbits.TCKPS = 2;    // prescaler = 64
#else
#error Invalid MIPS setting, must be 16, 32, or 64
#endif // MIPS

	T8CONbits.TGATE = 0;    // not gated
	T8CONbits.TSIDL = 0;    // do not stop the timer during IDLE
	T8CONbits.TCS = 0;      // Select internal instruction cycle clock
	T8CONbits.T32 = 1;      // T8/T9 form a 32 bit timer
	_T8IP = 0;
	_T8IF = 0;
	_T8IE = 0;
	_T9IP = 0;
	_T9IF = 0;
	_T9IE = 0;
	T8CONbits.TON = 1;   // start the timer
}

uint32_t micros(void)
{
	uint32_t t;

	// perform a coherent read of the 32 bit value
	t = TMR8;                           // snapshot the idle counter low word
	t += (((uint32_t)TMR9HLD) << 16);   // corresponding high word
#if (MIPS == 16)
	return t / 2;
#elif (MIPS == 32)
	return t / 4;
#elif (MIPS == 64)
	return t;
#else
#error Invalid MIPS setting, must be 16, 32, or 64
#endif // MIPS
}

uint32_t millis(void)
{
	return micros() / 1000;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



void spiWriteRegister(uint8_t address, uint8_t data)
{
	rfmWriteRegister(address | 0x80, data); // 16 bit SPI blocking write
}
uint8_t spiReadRegister(uint8_t address)
{
	return rfmReadRegister(address); // 16 bit SPI blocking read
}
/*
void spiReceive(uint8_t* data, uint8_t size)
{
    while (size--)
    {
        *(data++) = rfmReadRegister(0x7f);
    }
}
 */
void spiReceive(uint8_t* data, uint8_t size)
{
	rfmReceive(data, size);
}

boolean bindPlugCheck(void)
{
	uint8_t a;

	spiWriteRegister(0x0d, 0x23);    // gpio2 pullup enable direct digital input, 0010-0011 
	a = spiReadRegister(0x0e);       // I/O Port Configuration
	return (a & 0x04) ? 0 : 1;
}

void print_regs(void)
{
  uint8_t i;

  i = spiReadRegister(0x00);   // read device type
  DPRINT("Device type:  %u\r\n", i);
  i = spiReadRegister(0x01);   // read device version
  DPRINT("Device vers:  %u\r\n", i);
  i = spiReadRegister(0x02);   // read device status
  DPRINT("Device stat:  %u\r\n", i);
  i = spiReadRegister(0x03);   // read interrupt status 1
  DPRINT("Int status 1: %u\r\n", i);
  i = spiReadRegister(0x04);   // read interrupt status 2
  DPRINT("Int status 2: %u\r\n", i);
  i = spiReadRegister(0x05);   // read interrupt enabled 1
  DPRINT("Int enable 1: %u\r\n", i);
  i = spiReadRegister(0x06);   // read interrupt enabled 2
  DPRINT("Int enable 2: %u\r\n", i);
  i = spiReadRegister(0x07);   // read function control 1
  DPRINT("Func cntrl 1: %u\r\n", i);
  i = spiReadRegister(0x08);   // read function control 2
  DPRINT("Func cntrl 2: %u\r\n", i);
  DPRINT("\r\n");
}

// The _BV macro
// A macro is a method of performing a series of instructions with a single command,
// and the BV (bit value) macro is used to perform bitwise manipulation using bitmasks.
// Bitwise manipulation is the manipulation of the individual bits of a byte; a byte is
// made up of 8 bits numbered 0–7 with 0 being the least significant bit (LSB).
// Operators can be used with the macro to operate on individual bits. These are some
// of the operators:
// Operator Description
// |= Sets bit
// | Logical or
// &= ~ Clears bit
// Here are a couple of examples. The following line sets bit 0 only:
// PORTC |= _BV(0);
// The next line clears bit 1 only:
// PORTC &=~ _BV(1);
// Using bit operators you can manipulate multiple bits at a time. The following line sets
// bits 0, 2, and 4:
// PORTC |= _BV(0) | _BV(2) | _BV(4);
// The next line clears bits 1 and 3:
// PORTC &=~ _BV(1) | _BV(3);
//uint8_t _BV(uint8_t a) { return (0x01 << a); }
//#define _BV(a) (0x01 << a)


// map the openLRS eeprom functions to those provided in the eeprom_udb4 module
void eeprom_write_byte(uint8_t* addr, uint8_t data)
{
	eeprom_ByteWrite((uint16_t)addr, data);
}

uint8_t eeprom_read_byte(uint8_t* addr)
{
	uint8_t data;
	eeprom_ByteRead((uint16_t)addr, &data);
	return data; 
}

void delay(uint16_t a) { delay_ms(a); }
void delayMicroseconds(uint16_t a) { delay_us(a); }

void cli(void) {}
void sei(void) {}

uint8_t lowByte(uint16_t a)
{
	return a;
}
uint8_t highByte(uint16_t a)
{
	return (a >> 8);
}

void buzzerInit(void) {}
void buzzerOff(void) {}
void buzzerOn(uint16_t freq) {}


// Version number in single uint16 [8bit major][4bit][4bit]
// a.b.c == 0xaabc
static uint16_t version = 0x0353;

void myEEPROMwrite(int16_t addr, uint8_t data);
boolean bindReadEeprom(void);
void bindWriteEeprom(void);
boolean rxReadEeprom(void);
void rxWriteEeprom(void);
boolean fsReadEeprom(uint8_t* ee_buf, int8_t len);
void fsWriteEeprom(uint8_t* ee_buf, int8_t len);

#include "../libLRS/hardware.h"
#include "../libLRS/binding.h"
#include "../libLRS/common.h"
#include "../libLRS/failsafe.h"
#include "../libLRS/RX.h"
#include "../libLRS/storage.h"

void echoBindData(void)
{
  uint8_t i;

  DPRINT("bind_data(%u):\r\n", sizeof(bind_data));
  DPRINT("\tversion %u\r\n", (uint16_t)bind_data.version);
  DPRINT("\tserial_baudrate %lu\r\n", bind_data.serial_baudrate);
  DPRINT("\trf_frequency %lu\r\n", bind_data.rf_frequency);
  //DPRINT("\trf_magic %08X\r\n", bind_data.rf_magic);
  DPRINT("\trf_magic %04X", (uint16_t)(bind_data.rf_magic >> 16));
  DPRINT("%04X\r\n", (uint16_t)(bind_data.rf_magic & 0x0000ffff));
  DPRINT("\trf_power %u\r\n", (uint16_t)bind_data.rf_power);
  DPRINT("\trf_channel_spacing %u\r\n", (uint16_t)bind_data.rf_channel_spacing);
  DPRINT("\thopchannels ");
  for (i = 0; i < MAXHOPS && bind_data.hopchannel[i] != 0; i++) {
//    DPRINT("\t\thopchannel[%u] %u\r\n", i, (uint16_t)bind_data.hopchannel[i]);
    DPRINT(" %u", (uint16_t)bind_data.hopchannel[i]);
  }
  DPRINT("\r\n");
  DPRINT("\tmodem_params %02x\r\n", (uint16_t)bind_data.modem_params);
  DPRINT("\tflags %02x\r\n", (uint16_t)bind_data.flags);
}

void echoRxConfig(void)
{
  uint8_t i;

  DPRINT("rx_config(%u):\r\n", sizeof(rx_config));
  DPRINT("\trx_type %u\r\n", (uint16_t)rx_config.rx_type);
  for (i = 0; i < 13; i++) {
    DPRINT("\t\tpinMapping[%u] %u\r\n", i, (uint16_t)rx_config.pinMapping[i]);
  }
  DPRINT("\tflags %u\r\n", (uint16_t)rx_config.flags);
  DPRINT("\tRSSIpwm %u\r\n", (uint16_t)rx_config.RSSIpwm);
  DPRINT("\tbeacon_frequency %lu\r\n", rx_config.beacon_frequency);
  DPRINT("\tbeacon_deadtime %u\r\n", (uint16_t)rx_config.beacon_deadtime);
  DPRINT("\tbeacon_interval %u\r\n", (uint16_t)rx_config.beacon_interval);
  DPRINT("\tminsync %u\r\n", (uint16_t)rx_config.minsync);
  DPRINT("\tfailsafeDelay %u\r\n", (uint16_t)rx_config.failsafeDelay);
  DPRINT("\tppmStopDelay %u\r\n", (uint16_t)rx_config.ppmStopDelay);
  DPRINT("\tpwmStopDelay %u\r\n", (uint16_t)rx_config.pwmStopDelay);
}

void echoModemParams(struct rfm22_modem_regs* r)
{
//  uint32_t bps;
//  uint8_t  r_1c, r_1d, r_1e, r_20, r_21, r_22, r_23, r_24, r_25, r_2a, r_6e, r_6f, r_70, r_71, r_72;

  DPRINT("modem_params(%u):\r\n", sizeof(modem_params));
  DPRINT("\tbps %lu\r\n", r->bps);
/*
  DPRINT("\tr_1c %u\r\n", (uint16_t)r->r_1c);
  DPRINT("\tr_1d %u\r\n", (uint16_t)r->r_1d);
  DPRINT("\tr_1e %u\r\n", (uint16_t)r->r_1e);
  DPRINT("\tr_20 %u\r\n", (uint16_t)r->r_20);
  DPRINT("\tr_21 %u\r\n", (uint16_t)r->r_21);
  DPRINT("\tr_22 %u\r\n", (uint16_t)r->r_22);
  DPRINT("\tr_23 %u\r\n", (uint16_t)r->r_23);
  DPRINT("\tr_24 %u\r\n", (uint16_t)r->r_24);
  DPRINT("\tr_25 %u\r\n", (uint16_t)r->r_25);
  DPRINT("\tr_2a %u\r\n", (uint16_t)r->r_2a);
  DPRINT("\tr_6e %u\r\n", (uint16_t)r->r_6e);
  DPRINT("\tr_6f %u\r\n", (uint16_t)r->r_6f);
  DPRINT("\tr_70 %u\r\n", (uint16_t)r->r_70);
  DPRINT("\tr_71 %u\r\n", (uint16_t)r->r_71);
  DPRINT("\tr_72 %u\r\n", (uint16_t)r->r_72);
 */
}

#else

void echoBindData(void)
{
}

#endif // dsPIC

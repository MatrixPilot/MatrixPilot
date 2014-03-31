//#if (__AVR_ATmega328P__ != 1)
#if defined( __dsPIC30F__ ) || defined( __dsPIC33F__ ) || defined(__dsPIC33E__)

#define LRS_BOARD_TYPE 7    // MatrixPilot UDB4/5 or AUAV3 Board
//#define COMPILE_TX

#define bool int8_t

#define F(a) a    // on Arduino declares constant strings in program memory


#define SERIAL_PRINT      Puts
#define SERIAL_PRINTLN(a) Puts(a); PutChar('\r');

#define SERIAL_PRINT_CHR      PutChar
#define SERIAL_PRINT_DEC      PutDec
#define SERIAL_PRINT_WRD(a)   printf("%u", a)
#define SERIAL_PRINT_LNG(a)   printf("%lu", a)

#define SERIAL_PRINTLN_CHR(a) PutChar(a);PutChar('\r');
#define SERIAL_PRINTLN_DEC(a) PutDec(a);PutChar('\r');
#define SERIAL_PRINTLN_D16    printf("%u\r", a)
#define SERIAL_PRINTLN_WRD(a) printf("%u\r", a)
#define SERIAL_PRINTLN_LNG(a) printf("%lu\r", a)

#define PUTNL()  Puts("\r\n")
#define DPRINT   printf

#else // !dsPIC

#include <avr/eeprom.h>


#define PUTCHR           Serial.print
#define PUTSTR           Serial.print
#define PUTDEC           Serial.print
#define PUTHEX(a)        Serial.print(a)
#define PUTH16(a)        Serial.print(a)
#define PUTH32(a)        Serial.print(a)
#define PUTWRD(a)        Serial.print(a)
#define PUTLNG(a)        Serial.print(a)


#define SERIAL_BEGIN     Serial.begin

#define SERIAL_PRINT     Serial.print
#define SERIAL_PRINT_CHR Serial.print
#define SERIAL_PRINT_DEC Serial.print
#define SERIAL_PRINT_WRD Serial.print
#define SERIAL_PRINT_LNG Serial.print

#define SERIAL_PRINTLN     Serial.println
#define SERIAL_PRINTLN_CHR Serial.println
#define SERIAL_PRINTLN_DEC Serial.println
#define SERIAL_PRINTLN_D16 Serial.println
#define SERIAL_PRINTLN_WRD Serial.println
#define SERIAL_PRINTLN_LNG Serial.println

#define SERIAL_WRITE     Serial.write

#define PUTNL()
//#define DPRINT(...)
#define DPRINT(a, b)

#define SERIAL_READ      Serial.read
#define SERIAL_AVAILABLE Serial.available

#endif // dsPIC
//#endif // __AVR_ATmeage328P__

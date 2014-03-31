// Generic definitions needed always

typedef struct pinMask {
  uint8_t B,C,D;
} pinMask_t;


#define RX_FLYTRON8CH 0x01
#define RX_OLRSNG4CH  0x02
#define RX_OLRSNG12CH 0x03
#define RX_DTFUHF10CH 0x04
#define RX_MATRIXPILOT 0x05

#define PINMAP_PPM    0x20
#define PINMAP_RSSI   0x21
#define PINMAP_SDA    0x22
#define PINMAP_SCL    0x23
#define PINMAP_RXD    0x24
#define PINMAP_TXD    0x25
#define PINMAP_ANALOG 0x26
#define PINMAP_LBEEP  0x27 // packetloss beeper
#define PINMAP_SPKTRM 0x28 // spektrum satellit output

// Following table is used by the dialog code to
// determine possible extra functions for each output.


struct rxSpecialPinMap {
  uint8_t output;
  uint8_t type;
};

#ifdef COMPILE_TX
// Needed by dialog code
static const char *specialStrs[] = { "PPM","RSSI","SDA","SCL","RXD","TXD","AIN","LBEEP",
                                     "SPKTRM", "", "", "", "", "", "", ""
                                   };
#define SPECIALSTR(x) (specialStrs[(x)&0x0f]) // note must be changed if not 16 strings
#endif

//####### Board Pinouts #########

#if (LRS_BOARD_TYPE == 7) // MatrixPilot UDB4/5 or AUAV3 Board
///////////////////////////////////////////////////////////////////////////////
// BEGIN: dsPIC port
///////////////////////////////////////////////////////////////////////////////

#if defined( __dsPIC30F__ ) || defined( __dsPIC33F__ ) || defined(__dsPIC33E__)
#else // not dsPIC hardware
#error Wrong board selected, Select MatrixPilot based boards only with Microchip build systems
#endif // ( __dsPIC30F__ ) || defined( __dsPIC33F__ ) || defined(__dsPIC33E__)

#define OUTPUTS 13 // outputs available

const pinMask_t OUTPUT_MASKS[OUTPUTS] = {
  {0x00,0x00,0x08},{0x00,0x00,0x20},{0x00,0x00,0x40}, // RSSI, CH1, CH2
  {0x00,0x00,0x80},{0x01,0x00,0x00},{0x02,0x00,0x00}, // CH2, CH3, CH4
  {0x04,0x00,0x00},{0x08,0x00,0x00},{0x10,0x00,0x00}, // CH5, CH6, CH7
  {0x00,0x10,0x00},{0x00,0x20,0x00},{0x00,0x00,0x01}, // SDA, SCL, RXD
  {0x00,0x00,0x02},                                   // TXD
};

const uint8_t OUTPUT_PIN[OUTPUTS] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 , 10, 11, 12 };

#define PPM_OUTPUT  5
#define RSSI_OUTPUT 0
#define ANALOG0_OUTPUT 9
#define ANALOG1_OUTPUT 10
#define SDA_OUTPUT 9
#define SCL_OUTPUT 10
#define RXD_OUTPUT 11
#define TXD_OUTPUT 12

//#define Red_LED_ON     LED_RED = LED_ON;
//#define Red_LED_OFF    LED_RED = LED_OFF;
//#define Green_LED_ON   LED_GREEN = LED_ON;
//#define Green_LED_OFF  LED_GREEN = LED_OFF;
/////////////////////////// TODO: temporary for debugging bind mode


#define PINMAP_PPM    0x20
#define PINMAP_RSSI   0x21
#define PINMAP_SDA    0x22
#define PINMAP_SCL    0x23
#define PINMAP_RXD    0x24
#define PINMAP_TXD    0x25
#define PINMAP_ANALOG 0x26
#define PINMAP_LBEEP  0x27 // packetloss beeper
#define PINMAP_SPKTRM 0x28 // spektrum satellit output

struct rxSpecialPinMap rxSpecialPins[] = {
  {  0, PINMAP_RSSI},
  {  0, PINMAP_LBEEP},
  {  5, PINMAP_PPM},
  {  9, PINMAP_SDA},
  {  9, PINMAP_ANALOG}, // AIN0
  { 10, PINMAP_SCL},
  { 10, PINMAP_ANALOG}, // AIN1
  { 11, PINMAP_RXD},
  { 12, PINMAP_TXD},
  { 12, PINMAP_SPKTRM},
};

///////////////////////////

#define Red_LED_ON     LED_ORANGE = LED_ON;
#define Red_LED_OFF    LED_ORANGE = LED_OFF;
#define Green_LED_ON   LED_BLUE = LED_ON;
#define Green_LED_OFF  LED_BLUE = LED_OFF;

// TODO: determine what is happening in beacon_tone(...)
#define SDI_on
#define SDI_off

///////////////////////////////////////////////////////////////////////////////
// END: dsPIC port
///////////////////////////////////////////////////////////////////////////////
#endif // (LRS_BOARD_TYPE == 7)

//#define HIWORD(a) ((uint16_t)((a >> 16) & 0x0000ffff))
//#define LOWORD(a) ((uint16_t)((a & 0x0000ffff))

#define HIWORD(a) ((uint16_t)(a >> 16))
#define LOWORD(a) ((uint16_t)(a))

//#define HIWORD(a) a
//#define LOWORD(a) a

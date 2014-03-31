#include "libLRS.h"

// OpenLRSng binding

// Factory setting values, modify via the CLI

//####### RADIOLINK RF POWER (beacon is always 100/13/1.3mW) #######
// 7 == 100mW (or 1000mW with M3)
// 6 == 50mW (use this when using booster amp), (800mW with M3)
// 5 == 25mW
// 4 == 13mW
// 3 == 6mW
// 2 == 3mW
// 1 == 1.6mW
// 0 == 1.3mW
//#define DEFAULT_RF_POWER 7

//#define DEFAULT_CARRIER_FREQUENCY 435000000  // Hz  startup frequency
//#define DEFAULT_CHANNEL_SPACING 5 // 50kHz
//#define DEFAULT_HOPLIST 22,10,19,34,49,41
//#define DEFAULT_RF_MAGIC 0xDEADFEED

//  0 -- 4800bps, best range
//  1 -- 9600bps, medium range
//  2 -- 19200bps, medium range
//#define DEFAULT_DATARATE 2

//#define DEFAULT_BAUDRATE 115200

// FLAGS: 8bits

//#define TELEMETRY_OFF       0x00
//#define TELEMETRY_PASSTHRU  0x08
//#define TELEMETRY_FRSKY     0x10 // covers smartport if used with &
//#define TELEMETRY_SMARTPORT 0x18
#define TELEMETRY_MASK      0x18

//#define CHANNELS_4_4  0x01
//#define CHANNELS_8    0x02
//#define CHANNELS_8_4  0x03
//#define CHANNELS_12   0x04
//#define CHANNELS_12_4 0x05
//#define CHANNELS_16   0x06

//#define MUTE_TX       0x20 // do not beep on telemetry loss

//#define INVERTED_PPMIN 0x40
//#define MICROPPM       0x80

//#define DEFAULT_FLAGS (CHANNELS_8 | TELEMETRY_PASSTHRU)

// helper macro for European PMR channels
//#define EU_PMR_CH(x) (445993750L + 12500L * (x)) // valid for ch1-ch8

// helper macro for US FRS channels 1-7
//#define US_FRS_CH(x) (462537500L + 25000L * (x)) // valid for ch1-ch7

#define DEFAULT_BEACON_FREQUENCY 0 // disable beacon
#define DEFAULT_BEACON_DEADTIME 30 // time to wait until go into beacon mode (30s)
#define DEFAULT_BEACON_INTERVAL 10 // interval between beacon transmits (10s)

//#define MIN_DEADTIME 0
//#define MAX_DEADTIME 255

//#define MIN_INTERVAL 1
//#define MAX_INTERVAL 255

#define BINDING_POWER     0x06 // not lowest since may result fail with RFM23BP
#define BINDING_VERSION   9

#define TELEMETRY_PACKETSIZE 9

#define BIND_MAGIC (0xDEC1BE15 + BINDING_VERSION)

// HW frequency limits
#if (defined RFMXX_868)
#  define MIN_RFM_FREQUENCY 848000000
#  define MAX_RFM_FREQUENCY 888000000
#  define DEFAULT_CARRIER_FREQUENCY 868000000  // Hz  (ch 0)
#  define BINDING_FREQUENCY 868000000 // Hz
#elif (defined RFMXX_915)
#  define MIN_RFM_FREQUENCY 895000000
#  define MAX_RFM_FREQUENCY 935000000
#  define DEFAULT_CARRIER_FREQUENCY 915000000  // Hz  (ch 0)
#  define BINDING_FREQUENCY 915000000 // Hz
#else
#  define MIN_RFM_FREQUENCY 413000000
#  define MAX_RFM_FREQUENCY 463000000
#  define DEFAULT_CARRIER_FREQUENCY 435000000  // Hz  (ch 0)
#  define BINDING_FREQUENCY 435000000 // Hz
#endif

#define ALWAYS_BIND       0x02

#define MAXHOPS 24

struct bind_data {
  uint8_t version;
  uint32_t serial_baudrate;
  uint32_t rf_frequency;
  uint32_t rf_magic;
  uint8_t rf_power;
  uint8_t rf_channel_spacing;
  uint8_t hopchannel[MAXHOPS];
  uint8_t modem_params;
  uint8_t flags;
} __attribute__ ((packed)) bind_data;

struct rfm22_modem_regs {
  uint32_t bps;
  uint8_t  r_1c, r_1d, r_1e, r_20, r_21, r_22, r_23, r_24, r_25, r_2a, r_6e, r_6f, r_70, r_71, r_72;
} modem_params[] = {
  { 4800,   0x1a, 0x40, 0x0a, 0xa1, 0x20, 0x4e, 0xa5, 0x00, 0x1b, 0x1e, 0x27, 0x52, 0x2c, 0x23, 0x30 }, // 50000 0x00
  { 9600,   0x05, 0x40, 0x0a, 0xa1, 0x20, 0x4e, 0xa5, 0x00, 0x20, 0x24, 0x4e, 0xa5, 0x2c, 0x23, 0x30 }, // 25000 0x00
  { 19200,  0x06, 0x40, 0x0a, 0xd0, 0x00, 0x9d, 0x49, 0x00, 0x7b, 0x28, 0x9d, 0x49, 0x2c, 0x23, 0x30 }, // 25000 0x01
  { 57600,  0x05, 0x40, 0x0a, 0x45, 0x01, 0xd7, 0xdc, 0x03, 0xb8, 0x1e, 0x0e, 0xbf, 0x00, 0x23, 0x2e },
  { 125000, 0x8a, 0x40, 0x0a, 0x60, 0x01, 0x55, 0x55, 0x02, 0xad, 0x1e, 0x20, 0x00, 0x00, 0x23, 0xc8 },
};

#define DATARATE_COUNT (sizeof(modem_params) / sizeof(modem_params[0]))

struct rfm22_modem_regs bind_params =
{ 9600, 0x05, 0x40, 0x0a, 0xa1, 0x20, 0x4e, 0xa5, 0x00, 0x20, 0x24, 0x4e, 0xa5, 0x2c, 0x23, 0x30 };


// non linear mapping
// 0 - 0
// 1-99    - 100ms - 9900ms (100ms res)
// 100-189 - 10s  - 99s   (1s res)
// 190-209 - 100s - 290s (10s res)
// 210-255 - 5m - 50m (1m res)
uint32_t delayInMs(uint16_t d)
{
  uint32_t ms;
  if (d < 100) {
    ms = d;
  } else if (d < 190) {
    ms = (d - 90) * 10UL;
  } else if (d < 210) {
    ms = (d - 180) * 100UL;
  } else {
    ms = (d - 205) * 600UL;
  }
  return ms * 100UL;
}

// non linear mapping
// 0-89    - 10s - 99s
// 90-109  - 100s - 290s (10s res)
// 110-255 - 5m - 150m (1m res)
uint32_t delayInMsLong(uint8_t d)
{
  return delayInMs((uint16_t)d + 100);
}

struct RX_config {
  uint8_t  rx_type; // RX type filled in by RX, do not change
  uint8_t  pinMapping[13];
  uint8_t  flags;
  uint8_t  RSSIpwm;
  uint32_t beacon_frequency;
  uint8_t  beacon_deadtime;
  uint8_t  beacon_interval;
  uint16_t minsync;
  uint8_t  failsafeDelay;
  uint8_t  ppmStopDelay;
  uint8_t  pwmStopDelay;
} rx_config;

#ifndef COMPILE_TX
// following is only needed on receiver
void rxInitDefaults(bool save)
{
  uint8_t i;
  rx_config.rx_type = RX_MATRIXPILOT;
  for (i = 0; i < 13; i++) {
    rx_config.pinMapping[i] = i; // default to PWM out
  }
  rx_config.flags = ALWAYS_BIND;
  rx_config.RSSIpwm = 255; // off
  rx_config.failsafeDelay = 10; //1s
  rx_config.ppmStopDelay = 0;
  rx_config.pwmStopDelay = 0;
  rx_config.beacon_frequency = DEFAULT_BEACON_FREQUENCY;
  rx_config.beacon_deadtime = DEFAULT_BEACON_DEADTIME;
  rx_config.beacon_interval = DEFAULT_BEACON_INTERVAL;
  rx_config.minsync = 3000;

  SERIAL_PRINTLN("default RXconf loaded\n");
  echoRxConfig();
  if (save) {
    rxWriteEeprom();
    SERIAL_PRINTLN("default RXconf saved\n");
  }
}
#endif

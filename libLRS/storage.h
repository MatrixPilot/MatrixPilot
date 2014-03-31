#include "libLRS.h"

//#define BINDING_VERSION   9
//#define BIND_MAGIC (0xDEC1BE15 + BINDING_VERSION)

#define EEPROM_PROFILE_OFFSET  0x040 // profile number on TX
#ifdef COMPILE_TX
#define EEPROM_OFFSET(no)      (0x100 + (no)*0x40)
#else
#define EEPROM_OFFSET(no)      0x100
#endif
#define EEPROM_RX_OFFSET       0x140 // RX specific config struct
#define EEPROM_FAILSAFE_OFFSET 0x180


uint8_t activeProfile = 0;


// Halt and blink failure code
void fatalBlink(uint8_t blinks)
{
  uint8_t i;
  while (1) {
    for (i = 0; i < blinks; i++) {
      Red_LED_ON;
      delay(100);
      Red_LED_OFF;
      delay(100);
    }
    delay(300);
  }
}

// Save EEPROM by writing just changed data
void myEEPROMwrite(int16_t addr, uint8_t data)
{
  uint8_t retries = 5;
  while ((--retries) && (data != eeprom_read_byte((uint8_t*)addr))) {
    eeprom_write_byte((uint8_t*)addr, data);
  }
  if (!retries) {
    fatalBlink(2);
  }
}

boolean bindReadEeprom(void)
{
  uint32_t temp = 0;
  uint8_t i;

  DPRINT("bindReadEeprom() activeProfile %u\r\n", activeProfile);

  for (i = 0; i < 4; i++) {
    temp = (temp << 8) + eeprom_read_byte((uint8_t*)(EEPROM_OFFSET(activeProfile) + i));
  }
  if (temp != BIND_MAGIC) {
    DPRINT("bindReadEeprom() bad BIND_MAGIC %x%x\r\n", HIWORD(temp), LOWORD(temp));
    return false;
  }
  for (i = 0; i < sizeof(bind_data); i++) {
    *((uint8_t*)&bind_data + i) = eeprom_read_byte((uint8_t*)(EEPROM_OFFSET(activeProfile) + 4 + i));
  }

  echoBindData();

  if (bind_data.version != BINDING_VERSION) {
    DPRINT("bindReadEeprom() bad BINDING_VERSION %x\r\n", bind_data.version);
    return false;
  }

  return true;
}

void bindWriteEeprom(void)
{
  uint8_t i;

//  DPRINT("bindWriteEeprom() : %u\r\n", activeProfile);
  echoBindData();

  for (i = 0; i < 4; i++) {
    myEEPROMwrite(EEPROM_OFFSET(activeProfile) + i, (BIND_MAGIC >> ((3 - i) * 8)) & 0xff);
  }
  for (i = 0; i < sizeof(bind_data); i++) {
    myEEPROMwrite(EEPROM_OFFSET(activeProfile) + 4 + i, *((uint8_t*)&bind_data + i));
  }
}

void rxWriteEeprom(void)
{
  uint8_t i;

  DPRINT("rxWriteEeprom()\r\n");
  echoRxConfig();
  for (i = 0; i < 4; i++) {
    myEEPROMwrite(EEPROM_RX_OFFSET + i, (BIND_MAGIC >> ((3 - i) * 8)) & 0xff);
  }
  for (i = 0; i < sizeof(rx_config); i++) {
    myEEPROMwrite(EEPROM_RX_OFFSET + 4 + i, *((uint8_t*)&rx_config + i));
  }
}

boolean rxReadEeprom(void)
{
  uint32_t temp = 0;
  uint8_t i;

  for (i = 0; i < 4; i++) {
    temp = (temp << 8) + eeprom_read_byte((uint8_t*)(EEPROM_RX_OFFSET + i));
  }

  if (temp != BIND_MAGIC) {
    return false;
  } else {
    for (i = 0; i < sizeof(rx_config); i++) {
      *((uint8_t*)&rx_config + i) = eeprom_read_byte((uint8_t*)(EEPROM_RX_OFFSET + 4 + i));
    }
#if (LRS_BOARD_TYPE == 3)
    if (rx_config.rx_type != RX_FLYTRON8CH) {
    return false;
    }
#elif (LRS_BOARD_TYPE == 5)
    if (rx_config.rx_type != RX_OLRSNG4CH) {
    return false;
    }
#elif (LRS_BOARD_TYPE == 7)
    if (rx_config.rx_type != RX_MATRIXPILOT) {
    return false;
    }
#else
#error FIXME
#endif
    SERIAL_PRINTLN("RXconf loaded\n");
//  rx_config.flags = ALWAYS_BIND;
    echoRxConfig();
  }
  return true;
}

void fsWriteEeprom(uint8_t* ee_buf, int8_t len)
{
  int8_t i;

  for (i = 0; i < len; i++) {
    myEEPROMwrite(EEPROM_FAILSAFE_OFFSET + 4 + i, ee_buf[i]);
  }
  ee_buf[0] = 0xFA;
  ee_buf[1] = 0x11;
  ee_buf[2] = 0x5A;
  ee_buf[3] = 0xFE;
  for (i = 0; i < 4; i++) {
    myEEPROMwrite(EEPROM_FAILSAFE_OFFSET + i, ee_buf[i]);
  }
}

boolean fsReadEeprom(uint8_t* ee_buf, int8_t len)
{
  int8_t i;

  for (i = 0; i < 4; i++) {
    ee_buf[i] = eeprom_read_byte((uint8_t *)(EEPROM_FAILSAFE_OFFSET + i));
  }
  if ((ee_buf[0] == 0xFA) && (ee_buf[1] == 0x11) && (ee_buf[2] == 0x5A) && (ee_buf[3] == 0xFE)) {
    for (i = 0; i < 20; i++) {
      ee_buf[i] = eeprom_read_byte((uint8_t *)(EEPROM_FAILSAFE_OFFSET + 4 + i));
    }
    return true;
  }
  return false;
}

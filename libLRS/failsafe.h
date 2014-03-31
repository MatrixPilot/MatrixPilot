#include "libLRS.h"

uint8_t  failsafeIsValid = 0;
uint16_t failsafePPM[PPM_CHANNELS];

void failsafeSave(void)
{
  uint32_t start = millis();
  uint8_t ee_buf[20];
  int16_t i;

  for (i = 0; i < PPM_CHANNELS; i++) {
    failsafePPM[i] = PPM[i];
  }
  failsafeIsValid = 1;
  packChannels(6, failsafePPM, ee_buf);
  fsWriteEeprom(ee_buf, 20);

  // make this last at least 200ms for user to see it
  // needed as optimized eeprom code can be real fast if no changes are done
  start = millis() - start;
  if (start < 200) {
    delay(200 - start);
  }
}

void failsafeLoad(void)
{
  uint8_t ee_buf[20];

  if (fsReadEeprom(ee_buf, 20)) {
    unpackChannels(6, failsafePPM, ee_buf);
    failsafeIsValid = 1;
  } else {
    failsafeIsValid = 0;
  }
}

void failsafeApply(void)
{
  int16_t i;

  if (failsafeIsValid) {
    for (i = 0; i < PPM_CHANNELS; i++) {
      if (i != rx_config.RSSIpwm) {
        cli();
        PPM[i] = failsafePPM[i];
        sei();
      }
    }
  }
}


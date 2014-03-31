//####### COMMON FUNCTIONS #########

void rfmSetCarrierFrequency(uint32_t f);
uint8_t rfmGetRSSI(void);
void RF22B_init_parameter(void);
uint8_t spiReadRegister(uint8_t address);
void spiWriteRegister(uint8_t address, uint8_t data);
void spiReceive(uint8_t* data, uint8_t size);
void tx_packet(uint8_t* pkt, uint8_t size);
void to_rx_mode(void);
void echoModemParams(struct rfm22_modem_regs* r);


#define PPM_CHANNELS 16
volatile uint16_t PPM[PPM_CHANNELS] = { 512, 512, 512, 512, 512, 512, 512, 512 , 512, 512, 512, 512, 512, 512, 512, 512 };

const static uint8_t pktsizes[8] = { 0, 7, 11, 12, 16, 17, 21, 0 }; // actually the last index is never referenced
// The pktsizes correspond to the configured number of channels and their types as below
//#define CHANNELS_4_4  0x01 ->  7 byte pktsize (? this seems to have one byte too many ??)
//#define CHANNELS_8    0x02 -> 11 byte pktsize
//#define CHANNELS_8_4  0x03 -> 12 byte pktsize
//#define CHANNELS_12   0x04 -> 16 byte pktsize
//#define CHANNELS_12_4 0x05 -> 17 byte pktsize
//#define CHANNELS_16   0x06 -> 21 byte pktsize

#define RF_AVAILABLE    0
#define RF_TRANSMIT_REQ 1
#define RF_TRANSMIT_SIG 2
#define RF_RECEIVE_REQ  3
#define RF_RECEIVE_SIG  4

volatile uint8_t RF_Mode = 0;

void TaskLRS_Trigger(void);

void RFM22B_Int()
{
OLS0 = !OLS0;

//  if (RF_Mode == RF_TRANSMIT_REQ) {
//    RF_Mode = RF_TRANSMIT_SIG;
//  } else {
//    TaskLRS_Trigger();
//  }

    TaskLRS_Trigger();
}

uint8_t getPacketSize(struct bind_data* bd)
{
  return pktsizes[(bd->flags & 0x07)];
}

//uint8_t getChannelCount(struct bind_data *bd)
//{
//  return (((bd->flags & 7) / 2) + 1 + (bd->flags & 1)) * 4;
//}

uint32_t getInterval(struct bind_data* bd)
{
  uint32_t ret;
  // Sending a x byte packet on bps y takes about (emperical)
  // usec = (x + 15) * 8200000 / baudrate
#define BYTES_AT_BAUD_TO_USEC(bytes, bps) ((uint32_t)((bytes) + 15) * 8200000L / (uint32_t)(bps))

  ret = (BYTES_AT_BAUD_TO_USEC(getPacketSize(bd), modem_params[bd->modem_params].bps) + 2000);

  if (bd->flags & TELEMETRY_MASK) {
    ret += (BYTES_AT_BAUD_TO_USEC(TELEMETRY_PACKETSIZE, modem_params[bd->modem_params].bps) + 1000);
  }

  // round up to ms
  ret = ((ret + 999) / 1000) * 1000;

  // enable following to limit packet rate to 50Hz at most
#ifdef LIMIT_RATE_TO_50HZ
  if (ret < 20000) {
    ret = 20000;
  }
#endif

  return ret;
}

uint8_t twoBitfy(uint16_t in)
{
  if (in < 256) {
    return 0;
  } else if (in < 512) {
    return 1;
  } else if (in < 768) {
    return 2;
  } else {
    return 3;
  }
}

void packChannels(uint8_t config, volatile uint16_t PPM[], uint8_t *p)
{
  uint8_t i;
  for (i = 0; i <= (config / 2); i++) { // 4ch packed in 5 bytes
    p[0] = (PPM[0] & 0xff);
    p[1] = (PPM[1] & 0xff);
    p[2] = (PPM[2] & 0xff);
    p[3] = (PPM[3] & 0xff);
    p[4] = ((PPM[0] >> 8) & 3) | (((PPM[1] >> 8) & 3) << 2) | (((PPM[2] >> 8) & 3) << 4) | (((PPM[3] >> 8) & 3) << 6);
    p += 5;
    PPM += 4;
  }
  if (config & 1) { // 4ch packed in 1 byte;
    p[0] = (twoBitfy(PPM[0]) << 6) | (twoBitfy(PPM[1]) << 4) | (twoBitfy(PPM[2]) << 2) | twoBitfy(PPM[3]);
  }
}

void unpackChannels(uint8_t config, volatile uint16_t PPM[], uint8_t *p)
{
  uint8_t i;
  for (i = 0; i <= (config/2); i++) { // 4ch packed in 5 bytes
    PPM[0] = (((uint16_t)p[4] & 0x03) << 8) + p[0];
    PPM[1] = (((uint16_t)p[4] & 0x0c) << 6) + p[1];
    PPM[2] = (((uint16_t)p[4] & 0x30) << 4) + p[2];
    PPM[3] = (((uint16_t)p[4] & 0xc0) << 2) + p[3];
    p+=5;
    PPM+=4;
  }
  if (config & 1) { // 4ch packed in 1 byte;
    PPM[0] = (((uint16_t)p[0] >> 6) & 3) * 333 + 12;
    PPM[1] = (((uint16_t)p[0] >> 4) & 3) * 333 + 12;
    PPM[2] = (((uint16_t)p[0] >> 2) & 3) * 333 + 12;
    PPM[3] = (((uint16_t)p[0] >> 0) & 3) * 333 + 12;
  }
}

// conversion between microseconds 800-2200 and value 0-1023
// 808-1000 == 0 - 11     (16us per step)
// 1000-1999 == 12 - 1011 ( 1us per step)
// 2000-2192 == 1012-1023 (16us per step)
/*
uint16_t servoUs2Bits(uint16_t x)
{
  uint16_t ret;

  if (x < 800) {
    ret = 0;
  } else if (x < 1000) {
    ret = (x - 799) / 16;
  } else if (x < 2000) {
    ret = (x - 988);
  } else if (x < 2200) {
    ret = (x - 1992) / 16 + 1011;
  } else {
    ret = 1023;
  }

  return ret;
}

uint16_t servoBits2Us(uint16_t x)
{
  uint16_t ret;

  if (x < 12) {
    ret = 808 + x * 16;
  } else if (x < 1012) {
    ret = x + 988;
  } else if (x < 1024) {
    ret = 2000 + (x - 1011) * 16;
  } else {
    ret = 2192;
  }

  return ret;
}
 */
uint8_t countSetBits(uint16_t x)
{
  x  = x - ((x >> 1) & 0x5555);
  x  = (x & 0x3333) + ((x >> 2) & 0x3333);
  x  = x + (x >> 4);
  x &= 0x0F0F;
  return (x * 0x0101) >> 8;
}

#define NOP() __asm__ __volatile__("nop")

#define RF22B_PWRSTATE_POWERDOWN    0x00
#define RF22B_PWRSTATE_READY        0x01
#define RF22B_PACKET_SENT_INTERRUPT 0x04
#define RF22B_PWRSTATE_RX           0x05
#define RF22B_PWRSTATE_TX           0x09

#define RF22B_RX_PACKET_RECEIVED_INT    0x02

uint8_t ItStatus1, ItStatus2;

void to_sleep_mode(void);
void rx_reset(void);

// **** RFM22 access functions

void rfmSetChannel(uint8_t ch)
{
  uint8_t magicLSB = (bind_data.rf_magic & 0xff) ^ ch;
  spiWriteRegister(0x79, bind_data.hopchannel[ch]);
  spiWriteRegister(0x3a + 3, magicLSB);
  spiWriteRegister(0x3f + 3, magicLSB);
}

uint8_t rfmGetRSSI(void)
{
  return spiReadRegister(0x26);
}

uint16_t rfmGetAFCC(void)
{
  return (((uint16_t)spiReadRegister(0x2B) << 2) | ((uint16_t)spiReadRegister(0x2C) >> 6));
}

void setModemRegs(struct rfm22_modem_regs* r)
{
  echoModemParams(r);

  spiWriteRegister(0x1c, r->r_1c);
  spiWriteRegister(0x1d, r->r_1d);
  spiWriteRegister(0x1e, r->r_1e);
  spiWriteRegister(0x20, r->r_20);
  spiWriteRegister(0x21, r->r_21);
  spiWriteRegister(0x22, r->r_22);
  spiWriteRegister(0x23, r->r_23);
  spiWriteRegister(0x24, r->r_24);
  spiWriteRegister(0x25, r->r_25);
  spiWriteRegister(0x2a, r->r_2a);
  spiWriteRegister(0x6e, r->r_6e);
  spiWriteRegister(0x6f, r->r_6f);
  spiWriteRegister(0x70, r->r_70);
  spiWriteRegister(0x71, r->r_71);
  spiWriteRegister(0x72, r->r_72);
}

void rfmSetCarrierFrequency(uint32_t f)
{
  uint16_t fb, fc, hbsel;

//  DPRINT("rfmSetCarrierFrequency(%lu)\r\n", f);

  if (f < 480000000) {
    hbsel = 0;
    fb = f / 10000000 - 24;
    fc = (f - (fb + 24) * 10000000) * 4 / 625;
  } else {
    hbsel = 1;
    fb = f / 20000000 - 24;
    fc = (f - (fb + 24) * 20000000) * 2 / 625;
  }
  spiWriteRegister(0x75, 0x40 + (hbsel ? 0x20 : 0) + (fb & 0x1f));
  spiWriteRegister(0x76, (fc >> 8));
  spiWriteRegister(0x77, (fc & 0xff));
}

uint16_t rfmGetIntFlags(void)
{
	uint16_t flags;

	flags  = spiReadRegister(0x03) << 8;   // _rfmGetIntFlags()
	flags |= spiReadRegister(0x04);
	return flags;
}

uint8_t rfmGetInt1(void)
{
	return spiReadRegister(0x03);   // _rfmGetInt1()
}
uint8_t rfmGetInt2(void)
{
	return spiReadRegister(0x04);   // _rfmGetInt2()
}

void init_rfm(uint8_t isbind)
{
  uint8_t i;

//  DPRINT("init_rfm()\r\n");

//  i = spiReadRegister(0x00);   // read device type
//  DPRINT("Device type %u\r\n", i);
//  i = spiReadRegister(0x01);   // read device version
//  DPRINT("Device version %u\r\n", i);

  ItStatus1 = spiReadRegister(0x03);   // _init_rfm()
  ItStatus2 = spiReadRegister(0x04);
  spiWriteRegister(0x06, 0x00);    // disable interrupts
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY); // disable lbd, wakeup timer, use internal 32768,xton = 1; in ready mode
  spiWriteRegister(0x09, 0x7f);   // c = 12.5p
  spiWriteRegister(0x0a, 0x05);
#ifdef SWAP_GPIOS
  spiWriteRegister(0x0b, 0x15);    // gpio0 RX State (output) = 0001-0101
  spiWriteRegister(0x0c, 0x12);    // gpio1 TX State (output) = 0001-0010
#else
  spiWriteRegister(0x0b, 0x12);    // gpio0 TX State (output) = 0001-0010
  spiWriteRegister(0x0c, 0x15);    // gpio1 RX State (output) = 0001-0101
#endif
  spiWriteRegister(0x0d, 0xfd);    // gpio2 micro-controller clk output - RobD: NO IT'S NOT, THIS IS SETTING GPIO2 TO VDD
                                   //   besides, on the dsPIC implementation, this will later be set to an input with pullup, to detect a bind plug short
  spiWriteRegister(0x0e, 0x00);    // gpio    0, 1,2 NO OTHER FUNCTION.

  if (isbind) {
    setModemRegs(&bind_params);
  } else {
    setModemRegs(&modem_params[bind_data.modem_params]);
  }

  // Packet settings
  spiWriteRegister(0x30, 0x8c);    // enable packet handler, msb first, enable crc,
  spiWriteRegister(0x32, 0x0f);    // no broadcast, check header bytes 3,2,1,0
  spiWriteRegister(0x33, 0x42);    // 4 byte header, 2 byte synch, variable pkt size
  spiWriteRegister(0x34, 0x0a);    // 10 nibbles (40 bit preamble)
  spiWriteRegister(0x35, 0x2a);    // preath = 5 (20bits), rssioff = 2
  spiWriteRegister(0x36, 0x2d);    // synchronize word 3
  spiWriteRegister(0x37, 0xd4);    // synchronize word 2
  spiWriteRegister(0x38, 0x00);    // synch word 1 (not used)
  spiWriteRegister(0x39, 0x00);    // synch word 0 (not used)

  uint32_t magic = isbind ? BIND_MAGIC : bind_data.rf_magic;
  for (i = 0; i < 4; i++) {
    spiWriteRegister(0x3a + i, (magic >> 24) & 0xff);   // tx header
    spiWriteRegister(0x3f + i, (magic >> 24) & 0xff);   // rx header
    magic = magic << 8; // advance to next byte
  }

  spiWriteRegister(0x43, 0xff);    // all the bit to be checked
  spiWriteRegister(0x44, 0xff);    // all the bit to be checked
  spiWriteRegister(0x45, 0xff);    // all the bit to be checked
  spiWriteRegister(0x46, 0xff);    // all the bit to be checked

  if (isbind) {
    spiWriteRegister(0x6d, BINDING_POWER);
  } else {
    spiWriteRegister(0x6d, bind_data.rf_power);
  }

  spiWriteRegister(0x79, 0);

  spiWriteRegister(0x7a, bind_data.rf_channel_spacing);   // channel spacing

  spiWriteRegister(0x73, 0x00);
  spiWriteRegister(0x74, 0x00);    // no offset

  rfmSetCarrierFrequency(isbind ? BINDING_FREQUENCY : bind_data.rf_frequency);

}

void to_rx_mode(void)
{
  RF_Mode = RF_RECEIVE_REQ; // _to_rx_mode()
//  DPRINT("to_rx_mode()\r\n");
  ItStatus1 = spiReadRegister(0x03); // _to_rx_mode()
  ItStatus2 = spiReadRegister(0x04);
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
  delay(10);
  rx_reset(); // _to_rx_mode()
  NOP();
}

void rx_reset(void)
{
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
  spiWriteRegister(0x7e, 36);    // threshold for rx almost full, interrupt when 1 byte received
  spiWriteRegister(0x08, 0x03);    //clear fifo disable multi packet
  spiWriteRegister(0x08, 0x00);    // clear fifo, disable multi packet
  spiWriteRegister(0x07, RF22B_PWRSTATE_RX);   // 0x05 - to rx mode
  spiWriteRegister(0x05, RF22B_RX_PACKET_RECEIVED_INT); // 0x02

  spiWriteRegister(0x06, 0x80); // 0x80 RF22B_INT_EN2_SYNC_WORD

//  spiWriteRegister(0x06, 0x40); // 0x40 RF22B_INT_EN2_VALID_PREAMBLE_DETECTED
//  spiWriteRegister(0x06, 0xC0); // 0xC0 RF22B_INT_EN2_SYNC_WORD & RF22B_INT_EN2_VALID_PREAMBLE_DETECTED

  ItStatus1 = spiReadRegister(0x03);   // _rx_reset()
  ItStatus2 = spiReadRegister(0x04);
}

void rx_reset_bind(void)
{
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
  spiWriteRegister(0x7e, 36);    // threshold for rx almost full, interrupt when 1 byte received
  spiWriteRegister(0x08, 0x03);    //clear fifo disable multi packet
  spiWriteRegister(0x08, 0x00);    // clear fifo, disable multi packet
  spiWriteRegister(0x07, RF22B_PWRSTATE_RX);   // 0x05 - to rx mode
  spiWriteRegister(0x05, RF22B_RX_PACKET_RECEIVED_INT); // 0x02

  spiWriteRegister(0x06, 0x00); // 0x80 RF22B_INT_EN2_SYNC_WORD
//  spiWriteRegister(0x06, 0x80); // 0x80 RF22B_INT_EN2_SYNC_WORD

//  spiWriteRegister(0x06, 0x40); // 0x40 RF22B_INT_EN2_VALID_PREAMBLE_DETECTED
//  spiWriteRegister(0x06, 0xC0); // 0xC0 RF22B_INT_EN2_SYNC_WORD & RF22B_INT_EN2_VALID_PREAMBLE_DETECTED

  ItStatus1 = spiReadRegister(0x03);   // _rx_reset()
  ItStatus2 = spiReadRegister(0x04);
}

void to_rx_bind_mode(void)
{
  RF_Mode = RF_RECEIVE_REQ; // _to_rx_mode()
//  DPRINT("to_rx_mode()\r\n");
  ItStatus1 = spiReadRegister(0x03); // _to_rx_mode()
  ItStatus2 = spiReadRegister(0x04);
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
  delay(10);
  rx_reset_bind(); // _to_rx_mode()
  NOP();
}

void tx_packet(uint8_t* pkt, uint8_t size)
{
//  uint32_t tx_start = 0;
  uint8_t i;

  spiWriteRegister(0x3e, size);   // total tx size
  for (i = 0; i < size; i++) {
    spiWriteRegister(0x7f, pkt[i]);
  }
  spiWriteRegister(0x05, RF22B_PACKET_SENT_INTERRUPT); // 0x04
  ItStatus1 = spiReadRegister(0x03);      // _tx_packet()
  ItStatus2 = spiReadRegister(0x04);
//  tx_start = micros();
  spiWriteRegister(0x07, RF22B_PWRSTATE_TX);    // to tx mode

  RF_Mode = RF_TRANSMIT_REQ; // _tx_packet()
}

void beacon_tone(int16_t hz, int16_t len) //duration is now in half seconds.
{
  int16_t d = 500000 / hz; // better resolution

  if (d < 1) {
    d = 1;
  }

  int16_t cycles = (len * 500000 / d);
  int16_t i;

  for (i = 0; i < cycles; i++) {
    SDI_on;
    delayMicroseconds(d);
    SDI_off;
    delayMicroseconds(d);
  }
}

void beacon_send(void)
{
  Green_LED_ON
  ItStatus1 = spiReadRegister(0x03);   // _beacon_send()
  ItStatus2 = spiReadRegister(0x04);
  spiWriteRegister(0x06, 0x00);    // no wakeup up, lbd,
  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);      // disable lbd, wakeup timer, use internal 32768,xton = 1; in ready mode
  spiWriteRegister(0x09, 0x7f);  // (default) c = 12.5p
  spiWriteRegister(0x0a, 0x05);
  spiWriteRegister(0x0b, 0x12);    // gpio0 TX State
  spiWriteRegister(0x0c, 0x15);    // gpio1 RX State
  spiWriteRegister(0x0d, 0xfd);    // gpio 2 micro-controller clk output
  spiWriteRegister(0x0e, 0x00);    // gpio    0, 1,2 NO OTHER FUNCTION.

  spiWriteRegister(0x70, 0x2C);    // disable manchest

  spiWriteRegister(0x30, 0x00);    //disable packet handling

  spiWriteRegister(0x79, 0);    // start channel

  spiWriteRegister(0x7a, 0x05);   // 50khz step size (10khz x value) // no hopping

  spiWriteRegister(0x71, 0x12);   // trclk=[00] no clock, dtmod=[01] direct using SPI, fd8=0 eninv=0 modtyp=[10] FSK
  spiWriteRegister(0x72, 0x02);   // fd (frequency deviation) 2*625Hz == 1.25kHz

  spiWriteRegister(0x73, 0x00);
  spiWriteRegister(0x74, 0x00);    // no offset

  rfmSetCarrierFrequency(rx_config.beacon_frequency);

  spiWriteRegister(0x6d, 0x07);   // 7 set max power 100mW

  delay(10);
  spiWriteRegister(0x07, RF22B_PWRSTATE_TX);    // to tx mode
  delay(10);

  //close encounters tune
  //  G, A, F, F(lower octave), C
  //octave 3:  392  440  349  175   261

  beacon_tone(392, 1);

  spiWriteRegister(0x6d, 0x05);   // 5 set mid power 25mW
  delay(10);
  beacon_tone(440,1);

  spiWriteRegister(0x6d, 0x04);   // 4 set mid power 13mW
  delay(10);
  beacon_tone(349, 1);

  spiWriteRegister(0x6d, 0x02);   // 2 set min power 3mW
  delay(10);
  beacon_tone(175,1);

  spiWriteRegister(0x6d, 0x00);   // 0 set min power 1.3mW
  delay(10);
  beacon_tone(261, 2);


  spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
  Green_LED_OFF
}

// Print version, either x.y or x.y.z (if z != 0)
/*
void printVersion(uint16_t v)
{
  SERIAL_PRINT_DEC(v >> 8);
  SERIAL_PRINT_CHR('.');
  SERIAL_PRINT_DEC((v >> 4) & 0x0f);
  if (version & 0x0f) {
    SERIAL_PRINT_CHR('.');
    SERIAL_PRINT_DEC(v & 0x0f);
  }
}
 */

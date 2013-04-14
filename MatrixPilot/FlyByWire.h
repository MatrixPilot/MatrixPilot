
#ifndef _FLYBYWIRE_H_
#define _FLYBYWIRE_H_

#define LENGTH_OF_HEADER	(3)
#define LENGTH_OF_PAYLOAD	(10)
#define LENGTH_OF_PACKET	(LENGTH_OF_HEADER + LENGTH_OF_PAYLOAD)

uint8_t get_fbw_pos(void);
void fbw_live_begin(void );
void fbw_live_commit(void);
void fbw_live_commit_buf(uint8_t* buf);
boolean fbw_live_received_byte(uint8_t inbyte);
int16_t get_fbw_pwm(int16_t index);


#endif // _FLYBYWIRE_H_

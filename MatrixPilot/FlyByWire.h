
#ifndef _FLYBYWIRE_H_
#define _FLYBYWIRE_H_

#include "GenericTypeDefs.h"

#define LENGTH_OF_HEADER	(3)
#define LENGTH_OF_PAYLOAD	(10)
#define LENGTH_OF_PACKET	(LENGTH_OF_HEADER + LENGTH_OF_PAYLOAD)

BYTE get_fbw_pos(void);
void fbw_live_begin( void );
void fbw_live_commit(void);
void fbw_live_commit_buf(BYTE* buf);
BOOL fbw_live_received_byte( unsigned char inbyte );


#endif // _FLYBYWIRE_H_

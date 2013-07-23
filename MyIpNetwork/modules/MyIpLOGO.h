
#ifndef _MYIPLOGO_H_
#define _MYIPLOGO_H_

//#include "defines.h"
#include "flightplan-logoMsgFormat.h"

#define LOGO_INST_SIZE                  (6)
#define LOGO_HEADER_SIZE                (4)
#define LOGO_MAX_RECIEVE_BUFFER         (LOGO_HEADER_SIZE + LOGO_REMOTE_INSTRUCTIONS_MAX_LENGTH*LOGO_INST_SIZE)

typedef enum
{
  Clear_Mission = 0,
  LogoCmd = 1,
  Execute_Mission = 2,
} LogoDataMsgCmd;

typedef struct
{
  LogoDataMsgCmd cmdtype;
  uint8_t mission;
  uint8_t indexCmd;
  uint8_t length;
} LogoDataMsg;


void MyIpInit_LOGO(const uint8_t s);
void MyIpService_LOGO(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_LOGO(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_LOGO(const uint8_t s);
void MyIpProcessRxData_LOGO(const uint8_t s);
void MyIpOnConnect_LOGO(const uint8_t s);

void processLogoDataMsg(uint8_t* packet);
#endif // _MYIPLOGO_H_


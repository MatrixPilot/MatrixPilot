
#ifndef _MYIPLOGO_H_
#define _MYIPLOGO_H_

//#include "defines.h"
#include "../MatrixPilot/flightplan-logoMsgFormat.h"

#define LOGO_INST_SIZE                  (6)
#define LOGO_HEADER_SIZE                (4)
#define LOGO_MAX_RECIEVE_BUFFER         (LOGO_HEADER_SIZE + LOGO_USER_INSTRUCTIONS_MAX_LENGTH*LOGO_INST_SIZE)

typedef enum
{
  Clear_Mission = 0,
  LogoCmd = 1,
  Execute_Mission = 2,
  Resume_Mission = 3,
  Read_Mission = 4,
  Read_Mission_Response = 5,
  Read_Cmd = 6,
  Read_Cmd_Response = 7,

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

void processLogoDataMsg(const uint8_t s, const uint8_t* packet);
void sendLogoResponse_ReadCmd(const uint8_t s, const uint8_t mission);
void sendLogoResponse_ReadMission(const uint8_t s, const uint8_t mission);

#endif // _MYIPLOGO_H_


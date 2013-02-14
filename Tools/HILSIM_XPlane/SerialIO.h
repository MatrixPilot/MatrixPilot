#pragma once

void OpenComms(void);
void CloseComms(void);

void StartServer(uint16_t PortNum);
void StopServer(void);

void SendToComPort(unsigned long ResponseLength, unsigned char *Buffer);
void ReceiveFromComPort(void);
void ShowMessage(const char *pErrorString);
void HandleMsgByte(char b);

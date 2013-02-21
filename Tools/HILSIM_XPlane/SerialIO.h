#pragma once

int IsConnected(void);

void OpenComms(void);
void CloseComms(void);

void SendToComPort(unsigned long ResponseLength, unsigned char *Buffer);
void ReceiveFromComPort(void);
void ShowMessage(const char *pErrorString);
int HandleMsgByte(char b);

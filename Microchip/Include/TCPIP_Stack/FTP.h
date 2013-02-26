/*********************************************************************
 *
 *                  FTP Server Defs for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        FTP.h
 * Dependencies:    StackTsk.h
 *                  TCP.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     4/23/01  Original        (Rev 1.0)
 ********************************************************************/
#ifndef __FTP_H
#define __FTP_H


#define FTP_USER_NAME_LEN       (10)


/*********************************************************************
 * Function:        BOOL FTPVerify(char *login, char *password)
 *
 * PreCondition:    None
 *
 * Input:           login       - Telnet User login name
 *                  password    - Telnet User password
 *
 * Output:          TRUE if login and password verfies
 *                  FALSE if login and password does not match.
 *
 * Side Effects:    None
 *
 * Overview:        Compare given login and password with internal
 *                  values and return TRUE or FALSE.
 *
 * Note:            This is a callback from Telnet Server to
 *                  user application.  User application must
 *                  implement this function in his/her source file
 *                  return correct response based on internal
 *                  login and password information.
 ********************************************************************/
#ifdef __FTP_C
extern BOOL FTPVerify(BYTE *login, BYTE *password);
#endif


/*********************************************************************
 * Function:        void FTPInit(void)
 *
 * PreCondition:    TCP module is already initialized.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initializes internal variables of Telnet
 *
 * Note:
 ********************************************************************/
void FTPInit(void);


/*********************************************************************
 * Function:        void FTPServer(void)
 *
 * PreCondition:    FTPInit() must already be called.
 *
 * Input:           None
 *
 * Output:          Opened Telnet connections are served.
 *
 * Side Effects:    None
 *
 * Overview:
 *
 * Note:            This function acts as a task (similar to one in
 *                  RTOS).  This function performs its task in
 *                  co-operative manner.  Main application must call
 *                  this function repeatdly to ensure all open
 *                  or new connections are served on time.
 ********************************************************************/
BOOL FTPServer(void);


#endif

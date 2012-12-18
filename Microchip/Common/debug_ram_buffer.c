/*******************************************************************************

    Debug RAM buffer implementation file

Summary:
    This file implements the debug interface using a RAM buffer

Description:
    This file implements the debug interface using a RAM buffer

******************************************************************************/
//DOM-IGNORE-BEGIN
/******************************************************************************
 FileName:     	debug_ram_buffer.c
 Dependencies:	Compiler.h and GenericTypeDefs.h
 Processor:		any
 Hardware:		any
 Complier:  	any
 Company:		Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC® Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*******************************************************************/

/** Include Files *************************************************/
#include <Compiler.h>
#include <GenericTypeDefs.h>

/** Build Options *************************************************/
#if !defined(DEBUG_BUFFER_SIZE)
    #define DEBUG_BUFFER_SIZE   4000
#endif

/** Type Definitions **********************************************/
typedef struct{
    unsigned char data[DEBUG_BUFFER_SIZE];
    BOOL overflow;
    unsigned int position;
} DEBUG_SESSION;

/** Variables *****************************************************/
DEBUG_SESSION debug;

/** Functions *****************************************************/

/**************************************************************************
    Function:
        void DEBUG_Initialize( void );
   
    Summary:
        Initializes the debug module.  This can be I/O pins, memory, etc.
        based on the debug implementation
        
    Description:
        Initializes the debug module.  This can be I/O pins, memory, etc.
        based on the debug implementation
        
    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_Initialize()
{
    debug.position = 0;
    debug.overflow = FALSE;
    memset(debug.data, 0, sizeof(debug.data));
}


/**************************************************************************
    Function:
        void DEBUG_PutChar(char c);
   
    Summary:
        Puts a character into the debug stream.
        
    Description:
        This function puts a single character into the debug stream.

    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_PutChar(char c)
{
    if(debug.position >= sizeof(debug.data))
    {
        debug.overflow = TRUE;
        debug.position = 0;
    }

    debug.data[debug.position++] = c;
}


/**************************************************************************
    Function:
        void DEBUG_PutString(char* data);
   
    Summary:
        Prints a string to the debug stream.
        
    Description:
        Prints a string to the debug stream.
        
    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_PutString(char* data)
{
    UINT16 i;

    i = 0;

    do
    {
        DEBUG_PutChar(data[i]);
    } while(data[i++] != 0);
}


/**************************************************************************
    Function:
        void DEBUG_PutHexUINT8(UINT8 data);
   
    Summary:
        Puts a hexidecimal 8-bit number into the debug stream.
        
    Description:
        Puts a hexidecimal byte of data into the debug stream.  How this 
        is handled is implementation specific.  Some implementations may
        convert this to ASCII.  Others may print the byte directly to save
        memory/time.
        
    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_PutHexUINT8(UINT8 data)
{
    if(debug.position >= sizeof(debug.data))
    {
        debug.overflow = TRUE;
        debug.position = 0;
    }

    debug.data[debug.position++];
}


/**************************************************************************
    Function:
        void DEBUG_PutHexUINT16(UINT16 data);
   
    Summary:
        Puts a hexidecimal 16-bit number into the debug stream.
        
    Description:
        Puts a hexidecimal byte of data into the debug stream.  How this 
        is handled is implementation specific.  Some implementations may
        convert this to ASCII.  Others may print the byte directly to save
        memory/time.
        
    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_PutHexUINT16(UINT16 data)
{
    DEBUG_PutChar((char)((data >> 8) & 0x00FF));
    DEBUG_PutChar((char)( data       & 0x00FF));
}


/**************************************************************************
    Function:
        void DEBUG_PutHexUINT32(UINT32 data);
   
    Summary:
        Puts a hexidecimal 32-bit number into the debug stream.
        
    Description:
        Puts a hexidecimal byte of data into the debug stream.  How this 
        is handled is implementation specific.  Some implementations may
        convert this to ASCII.  Others may print the byte directly to save
        memory/time.
        
    Precondition:
        None
        
    Parameters:
        None
     
    Return Values:
        None
        
    Remarks:
        None
                                                          
  **************************************************************************/
void DEBUG_PutHexUINT32(UINT32 data)
{
    DEBUG_PutChar((char)((data >> 24) & 0x000000FF));
    DEBUG_PutChar((char)((data >> 16) & 0x000000FF));
    DEBUG_PutChar((char)((data >> 8)  & 0x000000FF));
    DEBUG_PutChar((char)( data        & 0x000000FF));
}




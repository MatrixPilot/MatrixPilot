/*****************************************************************************

 Advanced Encryption Standard (AES) Include Header
   128 bit key, 128 bit data block
   For more information see, AN1044

****************************************************************************
 FileName:		AES.h
 Dependencies:	AES.s
 Processor:		PIC24F, PIC24H, dsPIC30F, or dsPIC33F
 Compiler:		MPLAB C30 2.02 or later
 Linker:			MPLAB LINK30 2.02 or later
 Company:		Microchip Technology Incorporated

 Software License Agreement

 The software supplied herewith by Microchip Technology Incorporated
 (the “Company”) for its PICmicro® Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
 exclusively on Microchip PICmicro Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 Microchip Technology Inc. (“Microchip”) licenses this software to 
 you solely for use with Microchip products.  The software is owned 
 by Microchip and is protected under applicable copyright laws.  
 All rights reserved.

 You may not export or re-export Software, technical data, direct 
 products thereof or any other items which would violate any applicable
 export control laws and regulations including, but not limited to, 
 those of the United States or United Kingdom.  You agree that it is
 your responsibility to obtain copies of and to familiarize yourself
 fully with these laws and regulations to avoid violation.

 SOFTWARE IS PROVIDED “AS IS.”  MICROCHIP EXPRESSLY DISCLAIM ANY 
 WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT 
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP
 BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES,
 LOST PROFITS OR LOST DATA, HARM TO YOUR EQUIPMENT, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR 
 INDEMNITY OR CONTRIBUTION, OR OTHER SIMILAR COSTS. 

 Rev		Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 v1.0	Original release - released with AN1044
 v2.0    Updated to support 192-bit and 256-bit modes.  API change to be
         compatible with PIC32 implementation.  This increases RAM usage but
         decreases the decode time.
****************************************************************************/
#ifndef AES_H
#define AES_H

// *****************************************************************************
// *****************************************************************************
// Section: Includes
// *****************************************************************************
// *****************************************************************************

#include "GenericTypeDefs.h"
#include "Compiler.h"

#if defined(COMPILER_MPLAB_C32)
    #include "AES_ECB_PIC32.h"
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************

// Use an AES key length of 128-bits / 16 bytes.
#define AES_KEY_SIZE_128_BIT    16 
// Use an AES key length of 192-bits / 24 bytes.
#define AES_KEY_SIZE_192_BIT    24 
// Use an AES key length of 256-bits / 32 bytes.
#define AES_KEY_SIZE_256_BIT    32 

//Nr * (Nb + 1) 32-bits words
//Nr = 4;
//Nb = 10 for 128-bit
//Nb = 12 for 192-bit
//Nb = 14 for 256-bit

//Definition to simplify the creation of a round key buffer for the 
//  AESCreateRoundKeys() function.
typedef struct
{
    UINT32 key_length;
    #if defined(COMPILER_MPLAB_C32)
        AES_SESSION_KEY session_key;
    #else
        UINT32 data[44];
    #endif
} AES_ROUND_KEYS_128_BIT;

//Definition to simplify the creation of a round key buffer for the 
//  AESCreateRoundKeys() function.
typedef struct
{
    UINT32 key_length;
    #if defined(COMPILER_MPLAB_C32)
        AES_SESSION_KEY session_key;
    #else
        UINT32 data[52];
    #endif
} AES_ROUND_KEYS_192_BIT;

//Definition to simplify the creation of a round key buffer for the 
//  AESCreateRoundKeys() function.
typedef struct
{
    UINT32 key_length;
    #if defined(COMPILER_MPLAB_C32)
        AES_SESSION_KEY session_key;
    #else
        UINT32 data[60];
    #endif
} AES_ROUND_KEYS_256_BIT;

#define AES_BLOCK_SIZE  16

/* AES Errors enumeration

  Summary:
    List of possible AES releated errors

  Description:
    Some AES functions can return with an error.  This enumeration provides the
    list of possible errors.
*/
typedef enum
{
    /* No errors. */
	AES_ERROR_NONE			/*DOM-IGNORE-BEGIN*/ = (0x00000000u)	/*DOM-IGNORE-END*/,

    /* The calling function has requested that more bits be added to the 
       key stream then are available in the buffer alloted for the key stream.
       Since there was not enough room to complete the request, the request
       was not processed. */
	AES_ERROR_KEY_STREAM_GEN_OUT_OF_SPACE,

    /* The requesting call has caused the counter number to run out of unique
       combinations.  In CTR mode it is not safe to use the same counter
       value for a given key.  */
    AES_ERROR_CTR_COUNTER_EXPIRED

} AES_ERRORS;

/* AES options */
/* The following chart gives a visualization of which bits of the options
   parameter are currently being used.  Please note that some options use
   the default value of 0 and thus are not noted on this chart.

    .-----------------------------------31-
    |.----------------------------------30-
    ||.---------------------------------29-
    |||.--------------------------------28-
    ||||.-------------------------------27-
    |||||.------------------------------26- 
    ||||||.-----------------------------25- 
    |||||||.----------------------------24- 
    ||||||||
    |||||||| .--------------------------23- AES_USE_CFB1
    |||||||| |.-------------------------22- AES_USE_CFB8
    |||||||| ||.------------------------21- 
    |||||||| |||.-----------------------20- 
    |||||||| ||||.----------------------19- 
    |||||||| |||||.---------------------18- 
    |||||||| ||||||.--------------------17- 
    |||||||| |||||||.-------------------16- 
    |||||||| ||||||||
    |||||||| |||||||| .-----------------15- AES_STREAM_START
    |||||||| |||||||| |.----------------14- AES_STREAM_COMPLETE
    |||||||| |||||||| ||.---------------13-
    |||||||| |||||||| |||.--------------12-
    |||||||| |||||||| ||||.-------------11-
    |||||||| |||||||| |||||.------------10- AES_PAD_NUMBER
    |||||||| |||||||| ||||||.-----------9-- AES_PAD_8000
    |||||||| |||||||| |||||||.----------8-- AES_PAD_NULLS
    |||||||| |||||||| ||||||||
    |||||||| |||||||| |||||||| .--------7-- AES_PLAIN_TEXT_POINTER_ALIGNED
    |||||||| |||||||| |||||||| |.-------6-- AES_CIPHER_TEXT_POINTER_ALIGNED
    |||||||| |||||||| |||||||| ||.------5-- 
    |||||||| |||||||| |||||||| |||.-----4-- 
    |||||||| |||||||| |||||||| ||||.----3-- AES_CTR_SIZE
    |||||||| |||||||| |||||||| |||||.---2-- AES_CTR_SIZE
    |||||||| |||||||| |||||||| ||||||.--1-- AES_CTR_SIZE
    |||||||| |||||||| |||||||| |||||||.-0-- AES_CTR_SIZE
    |||||||| |||||||| |||||||| ||||||||
    00000000 00000000 00000000 00000000
*/

// The stream is still in progress.
#define AES_STREAM_CONTINUE             /*DOM-IGNORE-BEGIN*/ (0x00000000u)	/*DOM-IGNORE-END*/

// The stream is complete.  Padding will be applied if required.
#define AES_STREAM_COMPLETE             /*DOM-IGNORE-BEGIN*/ (0x00004000u)	/*DOM-IGNORE-END*/

// This should be passed when a new stream is starting
#define AES_STREAM_START                /*DOM-IGNORE-BEGIN*/ (0x00008000u)	/*DOM-IGNORE-END*/

// The cipher text pointer is pointing to data that is aligned to the target machine's word size (32-bit aligned for PIC32, 16-bit aligned for PIC24/dsPIC30/dsPIC33, and 8-bit aligned for PIC18).  Enabling this feature may improve throughput.
#define AES_CIPHER_TEXT_POINTER_ALIGNED /*DOM-IGNORE-BEGIN*/ (0x00000040u)    /*DOM-IGNORE-END*/

// The plain text pointer is pointing to data that is aligned to the target machine's word size (32-bit aligned for PIC32, 16-bit aligned for PIC24/dsPIC30/dsPIC33, and 8-bit aligned for PIC18).  Enabling this feature may improve throughput.
#define AES_PLAIN_TEXT_POINTER_ALIGNED  /*DOM-IGNORE-BEGIN*/ (0x00000080u)    /*DOM-IGNORE-END*/

// Pad with whatever data is already in the RAM.  This flag is normally set only for the last block of data.
#define AES_PAD_NONE                    /*DOM-IGNORE-BEGIN*/ (0x00000000u)	/*DOM-IGNORE-END*/

// Pad with 0x00 bytes if the current and previous data lengths do not end on a block boundary (multiple of 16 bytes).  This flag is normally set only for the last block of data.
#define AES_PAD_NULLS                   /*DOM-IGNORE-BEGIN*/ (0x00000100u)	/*DOM-IGNORE-END*/

// Pad with 0x80 followed by 0x00 bytes (a 1 bit followed by several 0 bits) if the current and previous data lengths do not end on a block boundary (multiple of 16 bytes).  This flag is normally set only for the last block of data.
#define AES_PAD_8000                    /*DOM-IGNORE-BEGIN*/ (0x00000200u)	/*DOM-IGNORE-END*/

// Pad with three 0x03's, four 0x04's, five 0x05's, six 0x06's, etc. set by the number of padding bytes needed if the current and previous data lengths do not end on a block boundary (multiple of 16 bytes).  This flag is normally set only for the last block of data.
#define AES_PAD_NUMBER                  /*DOM-IGNORE-BEGIN*/ (0x00000400u)    /*DOM-IGNORE-END*/

// Mask to determine the padding option that is selected.
#define AES_PAD_MASK                    /*DOM-IGNORE-BEGIN*/ (0x00000700u)    /*DOM-IGNORE-END*/

// Mask to determine the size of the counter in bytes.
#define AES_CTR_SIZE_MASK               /*DOM-IGNORE-BEGIN*/ (0x0000000Fu)    /*DOM-IGNORE-END*/

// Treat the counter as a 32-bit counter.  Leave the remaining section of the counter unchanged
#define AES_CTR_32BIT                   /*DOM-IGNORE-BEGIN*/ (0x00000004u)    /*DOM-IGNORE-END*/

// Treat the counter as a 64-bit counter.  Leave the remaining section of the counter unchanged
#define AES_CTR_64BIT                   /*DOM-IGNORE-BEGIN*/ (0x00000008u)    /*DOM-IGNORE-END*/

// Treat the counter as a full 128-bit counter.  This is the default option.
#define AES_CTR_128BIT                  /*DOM-IGNORE-BEGIN*/ (0x00000000u)    /*DOM-IGNORE-END*/

// Calculate the key stream for CFB1 mode
#define AES_USE_CFB1                    /*DOM-IGNORE-BEGIN*/ (0x00800000u)    /*DOM-IGNORE-END*/

// Calculate the key stream for CFB8 mode
#define AES_USE_CFB8                    /*DOM-IGNORE-BEGIN*/ (0x00400000u)    /*DOM-IGNORE-END*/

// Calculate the key stream for CFB128 mode
#define AES_USE_CFB128                  /*DOM-IGNORE-BEGIN*/ (0x00000000u)    /*DOM-IGNORE-END*/

// A definition to specify the default set of options.
#define AES_OPTIONS_DEFAULT             /*DOM-IGNORE-BEGIN*/ (0x00000000u)    /*DOM-IGNORE-END*/

// *****************************************************************************
// *****************************************************************************
// Section: AES Interface Routines
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void AESCreateRoundKeys(    void* round_keys,
                                UINT8* key, 
                                UINT8 key_size
                            )

  Summary:
    Creates a set of round keys from an AES key to be used in AES encryption and decryption of data blocks.

  Description:
    This routine takes an AES key and performs a key schedule to expand the key into a number of separate
    set of round keys.  These keys are commonly know as the Rijindael key schedule or a session key.

  Precondition:
    None.

  Parameters:
    round_keys - [out] Pointer to the output buffer that will contain the expanded short key (Rijindael) schedule/ session key.  This is to be used in the 
                       encryption and decryption routines.  The round_keys buffer must be
                       word aligned for the target processor.
    key        - [in]  The input key which can be 128, 192, or 256 bits in length.
    key_size   - [in]  Specifies the key length in bytes.  Valid options are\: 
                       * AES_KEY_SIZE_128_BIT
                       * AES_KEY_SIZE_192_BIT
                       * AES_KEY_SIZE_256_BIT
                       The values 16, 24, and 32 may also be used instead of the above definitions.

  Returns:
    None

  Example:
    <code>
    static const UINT8 AESKey128[] = {  0x95, 0xA8, 0xEE, 0x8E, 
                                        0x89, 0x97, 0x9B, 0x9E, 
                                        0xFD, 0xCB, 0xC6, 0xEB, 
                                        0x97, 0x97, 0x52, 0x8D 
                                     };
    AES_ROUND_KEYS_128_BIT round_keys;

    AESCreateRoundKeys(    &round_keys, 
                            AESKey128, 
                            AES_KEY_SIZE_128_BIT
                       );
    </code>

  *****************************************************************************/
void AESCreateRoundKeys(void* round_keys, UINT8* key, UINT8 key_size);

//The includes for the other modes must be included down here so that they are
//  after the defintion for AES_ERROR
#include <TCPIP Stack/AES_CBC.h>
#include <TCPIP Stack/AES_OFB.h>
#include <TCPIP Stack/AES_CTR.h>
#include <TCPIP Stack/AES_ECB.h>
#include <TCPIP Stack/AES_CFB.h>

#endif //AES_H

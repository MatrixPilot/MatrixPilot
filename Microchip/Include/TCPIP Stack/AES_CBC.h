/*****************************************************************************

 Advanced Encryption Standard (AES) Include Header for 
   cipher block chaining (CBC) mode

****************************************************************************
 FileName:		AES_CBC.h
 Dependencies:  AES.h
 Processor:		PIC24F, PIC24H, dsPIC30F, or dsPIC33F
 Compiler:		MPLAB C30 2.02 or later
 Linker:		MPLAB LINK30 2.02 or later
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
#ifndef AES_CBC_H
#define AES_CBC_H

// *****************************************************************************
// *****************************************************************************
// Section: Includes
// *****************************************************************************
// *****************************************************************************

#include <GenericTypeDefs.h>

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************

/* AES_CBC_STATE_DATA, *P_AES_CBC_STATE_DATA

  Summary:
    State information that must be maintained between blocks and initialization
    data.

  Description:
    The AES_CBC_STATE_DATA structure contains state information that needs to be
    retained between blocks of CBC data.  This structure is also used to pass
    some initialization data to the first block of a new stream.
*/
typedef struct
{
    /* The initialization vector must be loaded before the first call to the 
       encrypt or decrypt functions.  After that point this variable should
       not be touched.  This buffer is used to hold intermediate data. */
    UINT32 __attribute__((aligned)) initial_vector[4];

    /* Buffer for any data bytes that may have been remaining from the last
       call to the encrypt or decrypt function */
    /* DOM-IGNORE-BEGIN */
    /* This buffer is 16 bytes in length instead of 15 bytes in length
       because this provides a simple, word aligned buffer for the PIC32
       implementation to use */
    /* DOM-IGNORE-END */
    UINT8 __attribute__((aligned)) remaining_data[16];

    /* The number of valid bytes in the remaining_data buffer */
    UINT8 bytes_remaining;
} AES_CBC_STATE_DATA, *P_AES_CBC_STATE_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: AES Interface Routines
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
      void AESCBCEncrypt(   UINT8 * cipher_text,
                            UINT32 * num_cipher_bytes,
                            UINT8 * plain_text,
                            UINT32 num_plain_bytes, 
                            void * round_keys,
                            AES_CBC_STATE_DATA *p_cbc_state_data,
                            UINT32 options
                        );
    
  Summary:
    Encrypts a specified amount of data using cipher feedback (CFB) mode.
  Description:
    Encrypts a specified amount of data using cipher feedback (CFB) mode.
  Conditions:
      * Round keys must be create by AESCreateRoundKeys() before calling
        this routine.
      * p_cbc_state_data->intial_vector must be populated with the initialization
        vector before the first call of this function on a new data stream.
      * p_cbc_state_data->round_keys must be set to point to a buffer
        containing the round keys for this session.  This buffer must
        maintain the round keys until this session is complete.  The round
        keys can be created using the AESCreateRoundKeys() function.
  Input:
    cipher_text      - [out] pointer to the buffer where the encrypted data
                       sould reside. This buffer must be a multiple of 16 bytes
                       even if the plain text passed in is not.  This buffer should
                       also always be larger than the plain_text buffer.
    num_cipher_bytes - [out] the number of bytes written to the cipher_text buffer.
                       Note that this could be more or less than the num_plain bytes due
                       to buffered data or padding.
    plain_text       - [in] pointer to the data that needs to be encrypted.
    num_plain_bytes  - [in] the number of bytes in the plain_text buffer
    round_keys       - [in] pointer to the round key buffer that will be used
                            for this encryption cycle.
    p_cbc_state_data - [in] pointer to an instance of the AES_CBC_STATE_DATA for this
                       stream. The first call of this function should have the
                       p_cbc_state_data->initial_vector set to the initialization
                       vector.  The same AES_CBC_STATE_DATA object should be used for
                       every call in the same data stream.  The contents of the
                       AES_CBC_STATE_DATA object shouldn't be changed by the user 
                       once an encryption stream has been started.
    options          - [in] Set of options that are ORed together (e.g. \-
                       AES_PAD_NONE | AES_STREAM_START). Valid options are\:
                       * AES_PAD_NONE
                       * AES_PAD_NULLS
                       * AES_PAD_8000
                       * AES_PAD_NUMBER
                       * AES_STREAM_START
                       * AES_STREAM_CONTINUE
                       * AES_STREAM_COMPLETE
                       * AES_CIPHER_TEXT_POINTER_ALIGNED
                       * AES_PLAIN_TEXT_POINTER_ALIGNED
  Return:
    None
  Example:
    <code>
    //Example variables.  The key and initialization vector information
    //  should be customized for the target application.
    static const UINT8 AESKey128[]              = { 0x95, 0xA8, 0xEE, 0x8E,
                                                    0x89, 0x97, 0x9B, 0x9E, 
                                                    0xFD, 0xCB, 0xC6, 0xEB, 
                                                    0x97, 0x97, 0x52, 0x8D
                                                  };
    static const UINT8 initialization_vector[]  = { 0x00, 0x01, 0x02, 0x03, 
                                                    0x04, 0x05, 0x06, 0x07, 
                                                    0x08, 0x09, 0x0A, 0x0B, 
                                                    0x0C, 0x0D, 0x0E, 0x0F
                                                  };
    AES_ROUND_KEYS_128_BIT round_keys;
    AES_CBC_STATE_DATA current_stream;
    UINT32 num_bytes_encrypted;
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys (    &round_keys,
                            AESKey128,
                            AES_KEY_SIZE_128_BIT
                       );
    
    //Before the first call of AESCBCEncrypt() for a stream of data, the
    //  p_cbc_state_data->initial_vector must be loaded with the initialization
    //  vector information for that specific stream of data.
    memcpy(current_stream.initial_vector,initialization_vector,16);
    
    //Encrypt 128 bytes of of the current stream.  The num_bytes field must
    //  be a multiple of 16 unless the options field specifies that it is the
    //  end of the stream.
    AESCBCEncrypt(  cipher_text,
                    &num_bytes_encrypted,
                    plain_text,
                    128,                    //num_bytes
                    &round_keys,
                    &current_stream,        //stream state data
                    AES_STREAM_START        //options
                 );
    
    //At some point later we need to encrypt the rest of the stream.  The
    //  entire stream could have been encrypted in the first call if it was all
    //  available at the time.  Otherwise it can be done in sections, like this
    //  example.
    
    //Encrypt the remaining data of this stream.
    AESCBCEncrypt(  cipher_text_part2,
                    &num_bytes_encrypted,
                    plain_text_part2,
                    53,                                     //num_bytes
                    &round_keys,
                    &current_stream,                        //stream state data
                    AES_PAD_NULLS | AES_STREAM_COMPLETE     //options
                 );
    </code>
                                                                                                                                                                     
  ****************************************************************************/
void AESCBCEncrypt(UINT8 * cipher_text, UINT32 * num_cipher_bytes, UINT8 * plain_text, UINT32 num_plain_bytes, void * round_keys, AES_CBC_STATE_DATA *p_cbc_state_data, UINT32 options);

/******************************************************************************
  Function:
      void AESCBCDecrypt(   UINT8 * plain_text,
                            UINT32 * num_plain_bytes,
                            UINT8 * cipher_text, 
                            UINT32 num_cipher_bytes, 
                            void * round_keys,
                            AES_CBC_STATE_DATA *p_cbc_state_data,
                            UINT32 options
                        );
    
  Summary:
    Decrypts a specified amount of data using cipher block chaining (CBC).
  Description:
    Decrypts a specified amount of data using cipher block chaining.
  Conditions:
      * Round keys must be create by AESCreateRoundKeys() before calling
        this routine.
      * p_cbc_state_data->intial_vector must be populated with the initialization
        vector before the first call of this function on a new data stream.
  Input:
    plain_text -       [out] pointer to the buffer where the decrypted data
                       sould reside. This buffer must be a muliple of 16 bytes
                       even if the plain text passed in is not.
    num_plain_bytes  - [out] Pointer to a variable where the function should write
                       the number of bytes written to the plain_text buffer.
                       Note that this could be more or less than the num_cipher_bytes due
                       to buffered data.
    cipher_text -      [in] pointer to the data that needs to be decrypted. This
                       must be a multipe of 16 bytes unless this is the end of
                       the stream (the AES_STREAM_COMPLETE option is set)
    round_keys       - [in]  pointer to the round key buffer that will be used
                             for this encryption cycle.
    num_cipher_bytes - [in] the number of bytes in the cipher_text buffer
    p_cbc_state_data - [in] pointer to an instance of the AES_CBC_STATE_DATA for this
                       stream. The first call of this function should have the
                       p_cbc_state_data->initial_vector set to the initialization
                       vector.
    options -          [in] Set of options that are ORed together (e.g. \-
                       AES_STREAM_START | AES_STREAM_COMPLETE). Valid options
                       are\:
                       * AES_STREAM_START
                       * AES_STREAM_COMPLETE
                       * AES_CIPHER_TEXT_POINTER_ALIGNED
                       * AES_PLAIN_TEXT_POINTER_ALIGNED

  Return:
    None
  Example:
    <code>
    //Example variables.  The key and initialization vector information
    //  should be customized for the target application.
    static const UINT8 AESKey128[]              = { 0x95, 0xA8, 0xEE, 0x8E,
                                                    0x89, 0x97, 0x9B, 0x9E, 
                                                    0xFD, 0xCB, 0xC6, 0xEB, 
                                                    0x97, 0x97, 0x52, 0x8D
                                                  };
    static const UINT8 initialization_vector[]  = { 0x00, 0x01, 0x02, 0x03, 
                                                    0x04, 0x05, 0x06, 0x07, 
                                                    0x08, 0x09, 0x0A, 0x0B, 
                                                    0x0C, 0x0D, 0x0E, 0x0F
                                                  };
    AES_ROUND_KEYS_128_BIT round_keys;
    AES_CBC_STATE_DATA current_stream;
    UINT32 num_bytes_decrypted;
    
    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AESCreateRoundKeys(     &round_keys,
                            AESKey128,
                            AES_KEY_TYPE_128_BIT
                       );
    
    //Before the first call of AESCBCDecrypt() for a stream of data, the
    //  p_cbc_state_data->initial_vector must be loaded with the initialization
    //  vector information for that specific stream of data.
    memcpy(current_stream.initial_vector,initialization_vector,16);
    
    //Decrypt 128 bytes of of the current stream.  The num_bytes field must
    //  be a multiple of 16 unless the options field specifies that it is the
    //  end of the stream.
    AESCBCDecrypt(  plain_text,
                    &num_bytes_decrypted,
                    cipher_text,
                    128,                    //num_bytes
                    &round_keys,
                    &current_stream,
                    AES_STREAM_START        //options
                 );
    
    //At some point later we need to decrypt the rest of the stream.  The
    //  entire stream could have been decrypted in the first call if it was all
    //  available at the time.  Otherwise it can be done in sections, like this
    //  example.
    
    //Decrypt the remaining data of this stream.
    AESCBCDecrypt(  plain_text_part2,
                    &num_bytes_decrypted,
                    cipher_text_part2,
                    53,                     //num_bytes
                    &round_keys,
                    &current_stream,
                    AES_STREAM_COMPLETE,    //options
                 );
    </code>
                                                                                                                                                       
  ****************************************************************************/
void AESCBCDecrypt(UINT8 * plain_text, UINT32 * num_plain_bytes, UINT8 * cipher_text, UINT32 num_cipher_bytes, void * round_keys, AES_CBC_STATE_DATA *p_cbc_state_data, UINT32 options);

#endif  //AES_CBC_H

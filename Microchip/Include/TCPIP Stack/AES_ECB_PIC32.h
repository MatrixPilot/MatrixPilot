/********************************************************************
 * FileName:		AES_32_bit.h
 * Dependencies:
 * Processor:		PIC32MX
 * Hardware:		N/A
 * Assembler:		N/A
 * Linker:		    N/A
 * Company:		    Microchip Technology Inc..
 *
 * Software License Agreement:
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * $Id: INT.h,v 1.6 2006/11/07 23:29:45 C12923 Exp $
 * $Name:  $

 ********************************************************************/

#ifndef _AES_32_BIT_H
#define _AES_32_BIT_H

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

#ifdef _AES_KEY_128
#define Nb 4			// number of columns in the state & expanded key
#define Nk 4			// number of columns in a key
#define Nr 10			// number of rounds in encryption
#define _AES_KEY_STATIC
#endif

#ifdef _AES_KEY_192
#define Nb 4			// number of columns in the state & expanded key
#define Nk 6			// number of columns in a key
#define Nr 12			// number of rounds in encryption
#define _AES_KEY_STATIC
#endif

#ifdef _AES_KEY_256
#define Nb 4			// number of columns in the state & expanded key
#define Nk 8			// number of columns in a key
#define Nr 14			// number of rounds in encryption
#define _AES_KEY_STATIC
#endif

#define AES_BLOCK_SIZE  16

// *****************************************************************************
/* AES Session Key for 128-bit keys.

  Summary:
    Expanded key schedule for the AES session.

  Description:
    The expanded key schedule that is created by AESCreateSessionKey routine.  This session key is used in the
    encryption and decryption of data blocks.

  Remarks:
    Nr - Number of rounds during the encryption.  
         * Key length of 128-bits:  10 rounds

    Nb - Number of columns in the state and expanded key.  Always constant at 4.


    Structure Size - AES Session Key.
         * Key length of 128-bits:  352 bytes
        
*/
typedef struct
{
    UINT32 key[2][11][4];
}AES_SESSION_KEY_128_BIT;
// *****************************************************************************
/* AES Session Key for 192-bit keys.

  Summary:
    Expanded key schedule for the AES session.

  Description:
    The expanded key schedule that is created by AESCreateSessionKey routine.  This session key is used in the
    encryption and decryption of data blocks.

  Remarks:
    Nr - Number of rounds during the encryption.  
         * Key length of 192-bits:  12 rounds

    Nb - Number of columns in the state and expanded key.  Always constant at 4.


    Structure Size - AES Session Key.
         * Key length of 192-bits:  419 bytes
        
*/
typedef struct
{
    UINT32 key[2][13][4];
}AES_SESSION_KEY_192_BIT;
// *****************************************************************************
/* AES Session Key for 256-bit keys.

  Summary:
    Expanded key schedule for the AES session.

  Description:
    The expanded key schedule that is created by AESCreateSessionKey routine.  This session key is used in the
    encryption and decryption of data blocks.

  Remarks:
    Nr - Number of rounds during the encryption.  
         * Key length of 256-bits:  14 rounds

    Nb - Number of columns in the state and expanded key.  Always constant at 4.


    Structure Size - AES Session Key.
         * Key length of 256-bits:  480 bytes
        
*/
typedef struct
{
    UINT32 key[2][15][4];
}AES_SESSION_KEY_256_BIT;
// *****************************************************************************
/* AES Key Type

  Summary:
    AES key type based on the key length.

  Description:
    AES key type that is based on the key length of 128, 192 or 256-bit length.  
    The key length determines the session key size and the number of rounds during the 
    encryption and decryption.

  Remarks:
    Even when the AES library is compiled using a static key length, the calling routine
    will still have to provide the key type.
*/
typedef enum
{
#ifndef _AES_KEY_STATIC
    AES_KEY_TYPE_128_BIT = 16,       // Use an AES key length of 128-bits.
    AES_KEY_TYPE_192_BIT = 24,       // Use an AES key length of 192-bits.
    AES_KEY_TYPE_256_BIT = 32       // Use an AES key length of 256-bits.
#else

/*DOM-IGNORE-BEGIN*/
#ifdef _AES_KEY_128
    AES_KEY_TYPE_128_BIT = 16,       // Use an AES key length of 128-bits.
#endif

#ifdef _AES_KEY_192
    AES_KEY_TYPE_192_BIT = 24,       // Use an AES key length of 128-bits.
#endif

#ifdef _AES_KEY_256
    AES_KEY_TYPE_256_BIT = 32,       // Use an AES key length of 128-bits.
#endif

/*DOM-IGNORE-END*/
#endif
}AES_KEY_TYPE;
// *****************************************************************************
// *****************************************************************************
// Section: AES Interface Routines
// *****************************************************************************
// *****************************************************************************
#ifndef _AES_KEY_STATIC
/*******************************************************************************
  Function:
    void AESCreateSessionKey(UINT8 *key, AES_SESSION_KEY *session, AES_KEY_TYPE key_type, AES_KEY_TYPE key_type)

  Summary:
    Creates a session key to be used in AES encryption and decryption of data blocks.

  Description:
    This routine performs a key schedule to expand a short key into a number of separate
    round keys.  It is commonly know as the Rijindael key schedule.

  Precondition:
    None.

  Parameters:
    key     - [in]  the short key which can be 128, 192 or 256 bits in length.
    session - [out] the expanded short key (Rijindael) schedule.  This is to be used in the 
            encryption and decryption.
    key_type - [in]  AES key length of 128, 192, 256 bits.

  Returns:
    None

  Example:
    <code>
    static const UINT8 AESKey128[]    = { 0x95, 0xA8, 0xEE, 0x8E, 0x89, 0x97, 0x9B, 0x9E, 0xFD, 0xCB, 0xC6, 0xEB, 0x97, 0x97, 0x52, 0x8D };
    AES_SESSION_KEY_128_BIT session_key;

    AESCreateSessionKey(AESKey128, &session_key, AES_KEY_TYPE_128_BIT);
    </code>

  *****************************************************************************/
void AESCreateSessionKey(UINT8 *key, void *session, AES_KEY_TYPE key_type);

/*******************************************************************************
  Function:
    void AESECBEncryptBlock(UINT8 *plain_text, AES_SESSION_KEY *session, UINT8 *cipher_text, AES_KEY_TYPE key_type)

  Summary:
    Encrypts a data block using the session (Rijindael) key.

  Description:
    This routine performs an AES encryption on a block of data using the session (Rijindael) key.

  Precondition:
    Session key must be create by AESCreateSessionKey before calling this routine.

  Parameters:
    plain_text      - [in]  data to be encrypted.
    session         - [in]  the expanded short key (Rijindael) schedule.
    cipher_text     - [out] the encrypted block.
    key_type        - [in]  AES key length of 128, 192, 256-bits.

  Returns:
    None.

  Example:
    <code>
    const UINT8 AESDataBlock[] = { 0x4E, 0xC1, 0x37, 0xA4, 0x26, 0xDA, 0xBF, 0x8A, 0xA0, 0xBE, 0xB8, 0xBC, 0x0C, 0x2B, 0x89, 0xD6 };
    AES_SESSION_KEY_128_BIT session_key;
    UINT8   cipher_text[AES_BLOCK_SIZE];

    AESECBEncryptBlock(AESDataBlock, &session_key, cipher_text, AES_KEY_TYPE_128_BIT);
    </code>

   Remarks:
    The data block must be 16 bytes in length.  If the data block is less than
    16 bytes in length, it must be padded with zero's.  
 *****************************************************************************/
void AESECBEncryptBlock(UINT8 *plain_text, void *session, UINT8 *cipher_text, AES_KEY_TYPE key_type);

/*******************************************************************************
  Function:
    void AESECBDecryptBlock(UINT8 *cipher_text, AES_SESSION_KEY *session, UINT8 *plain_text, AES_KEY_TYPE key_type)

  Summary:
    Decrypts a data block using the session (Rijindael) key.

  Description:
    This routine performs an AES decryption on a block of data using the session (Rijindael) key.

  Precondition:
    Session key must be create by AESCreateSessionKey before calling this routine.

  Parameters:
    cipher_text     - [in]  encrypted data block.
    session         - [in]  the expanded short key (Rijindael) schedule.
    plain_text      - [out] the decrypted data block.
    key_type        - [in]  AES key length of 128, 192, 256-bits.

  Returns:
    None

  Example:
    <code>
    AES_SESSION_KEY_128_BIT session_key;
    UINT8   plain_text[AES_BLOCK_SIZE];
    
    AESECBDecryptBlock(cipher_text, &session_key, plain_text, AES_KEY_TYPE_128_BIT);
    </code>

 *****************************************************************************/
void AESECBDecryptBlock(UINT8 *cipher_text, void *session, UINT8 *plain_text, AES_KEY_TYPE key_type);

#else

// *****************************************************************************
// *****************************************************************************
// Section:When the AES library is compiled for static AES key length
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// void AESCreateSessionKeyStaticKey(UINT8 *key, void *session);
// *****************************************************************************
void AESCreateSessionKeyStaticKey(UINT8 *key, void *session);
// *****************************************************************************
// void AESECBEncryptBlockStaticKey(UINT8 *plain_text, void *session, UINT8 *cipher_text);
// *****************************************************************************
void AESECBEncryptBlockStaticKey(UINT8 *plain_text, void *session, UINT8 *cipher_text);
// *****************************************************************************
// void AESECBDecryptBlockStaticKey(UINT8 *cipher_text, void *session, UINT8 *plain_text);
// *****************************************************************************
void AESECBDecryptBlockStaticKey(UINT8 *cipher_text, void *session, UINT8 *plain_text);

// *****************************************************************************
// *****************************************************************************
// Section:  Helper macros
// *****************************************************************************
// *****************************************************************************
#define AESCreateSessionKey(k, s, k_t)          AESCreateSessionKeyStaticKey(k, s)
#define AESECBEncryptBlock(p_t, s, c_t, k_t)    AESECBEncryptBlockStaticKey(p_t, s, c_t)
#define AESECBDecryptBlock(c_t, s, p_t, k_t)    AESECBDecryptBlockStaticKey(c_t, s, p_t)
#endif

#define AESEncryptBlock(p_t, s, c_t)   AESECBEncryptBlock(p_t, (s+4), c_t, (AES_KEY_TYPE)*(DWORD*)s)
#define AESDecryptBlock(p_t, s, c_t)   AESECBDecryptBlock(p_t, (s+4), c_t, (AES_KEY_TYPE)*(DWORD*)s)

#ifdef _AES_KEY_STATIC

#ifdef _AES_KEY_128
typedef AES_SESSION_KEY_128_BIT     AES_SESSION_KEY;
#endif

#ifdef _AES_KEY_192
typedef AES_SESSION_KEY_192_BIT     AES_SESSION_KEY;
#endif

#ifdef _AES_KEY_256
typedef AES_SESSION_KEY_256_BIT     AES_SESSION_KEY;
#endif

#else
typedef AES_SESSION_KEY_256_BIT     AES_SESSION_KEY;
#endif

#endif  //_H


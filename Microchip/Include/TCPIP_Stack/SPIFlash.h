/*********************************************************************
 *
 *  SPI Flash Memory Driver Header
 *	- Tested to be compatible with SST25VF016B
 *  - Expected compatibility with other SST (Microchip) SST25 series 
 *    devices
 *
 *********************************************************************
 * FileName:        SPIFlash.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2010 Microchip Technology Inc.  All rights
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
 * E. Wood				3/20/08	Original
********************************************************************/
#ifndef __SPIFLASH_H
#define __SPIFLASH_H

#include "HardwareProfile.h"

#define SPI_FLASH_SECTOR_SIZE		(4096ul)
#define SPI_FLASH_PAGE_SIZE			(0ul)		// SST has no page boundary requirements

#define SPI_FLASH_SECTOR_MASK		(SPI_FLASH_SECTOR_SIZE - 1)


#if defined(SPIFLASH_CS_TRIS)
	void SPIFlashInit(void);		
	void SPIFlashReadArray(DWORD dwAddress, BYTE *vData, WORD wLen);
	void SPIFlashBeginWrite(DWORD dwAddr);
	void SPIFlashWrite(BYTE vData);
	void SPIFlashWriteArray(BYTE *vData, WORD wLen);
	void SPIFlashEraseSector(DWORD dwAddr);
#else
	// If you get any of these linker errors, it means that you either have an 
	// error in your HardwareProfile.h or TCPIPConfig.h definitions.  The code 
	// is attempting to call a function that can't possibly work because you 
	// have not specified what pins and SPI module the physical SPI Flash chip 
	// is connected to.  Alternatively, if you don't have an SPI Flash chip, it 
	// means you have enabled a stack feature that requires SPI Flash hardware.
	// In this case, you need to edit TCPIPConfig.h and disable this stack 
	// feature.  The linker error tells you which object file this error was 
	// generated from.  It should be a clue as to what feature you need to 
	// disable.
	void You_cannot_call_the_SPIFlashInit_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_SPIFlashReadArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_SPIFlashBeginWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_SPIFlashWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_SPIFlashWriteArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	void You_cannot_call_the_SPIFlashEraseSector_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first(void);
	#define SPIFlashInit()				You_cannot_call_the_SPIFlashInit_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define SPIFlashReadArray(a,b,c)	You_cannot_call_the_SPIFlashReadArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define SPIFlashBeginWrite(a)		You_cannot_call_the_SPIFlashBeginWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define SPIFlashWrite(a)			You_cannot_call_the_SPIFlashWrite_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define SPIFlashWriteArray(a,b)		You_cannot_call_the_SPIFlashWriteArray_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
	#define SPIFlashEraseSector(a)		You_cannot_call_the_SPIFlashEraseSector_function_without_defining_SPIFLASH_CS_TRIS_in_HardwareProfile_h_first()
#endif

#endif

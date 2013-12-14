/*********************************************************************
 *
 *                  dsPIC33E/PIC24E Bootloader
 *
 *********************************************************************
 * FileName:        NVMem.c
 * Dependencies:
 * Processor:       dsPIC33E/PIC24E
 *
 * Compiler:        MPLAB C30
 *                  MPLAB IDE
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its dsPIC3E/PIC24E Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip dsPIC3E/PIC24E Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
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
 *
 * $Id:  $
 * $Name: $
 *
 **********************************************************************/

#include "GenericTypeDefs.h"
#include "NVMem.h"
#if defined(__PIC24E__)
	#include <p24Exxxx.h>
#elif defined(__dsPIC33E__)
	#include <p33Exxxx.h>
#endif

/*********************************************************************
 * Function:        unsigned int NVMErasePage(void* address)
 *
 * Description:     Block Erases Program Memory
 * PreCondition:    None
 *
 * Inputs:          address:  Destination page address to Erase.
 *
 * Output:          '0' if operation completed successfully.
 *
 * Example:         NVMemErasePage(UINT32 0xBD000000)
 ********************************************************************/
UINT NVMemBlockErase(void)
{
	NVMCON = 0x400D;                // Bulk erase on next WR
	INTCON2bits.GIE = 0;            // Disable interrupts for next few instructions for unlock sequence
	__builtin_write_NVM();
    while (NVMCONbits.WR == 1){}
	INTCON2bits.GIE = 1;            // Re-enable the interrupts (if required).

	return NVMCONbits.WRERR;        // Return WRERR state.
}	

/*********************************************************************
 * Function:        unsigned int NVMErasePage(void* address)
 *
 * Description:     A page erase will erase a single page of program flash,
 *                  which equates to 1k instructions (3KBytes). The page to
 *                  be erased is selected using NVMADDR. The lower bytes of
 *                  the address given by NVMADDR are ignored in page selection.
 *
 * PreCondition:    None
 *
 * Inputs:          address:  Destination page address to Erase.
 *
 * Output:          '0' if operation completed successfully.
 *
 * Example:         NVMemErasePage(UINT32 0xBD000000)
 ********************************************************************/
UINT NVMemErasePage(UINT32 address)
{
	DWORD_VAL eraseAddress;
	eraseAddress.Val = address;

	TBLPAG = eraseAddress.byte.UB;
	NVMADRU = eraseAddress.word.HW;
    NVMADR = eraseAddress.word.LW;
	__builtin_tblwtl(eraseAddress.word.LW, 0xFFFF);
	NVMCON = 0x4003;                // Erase page on next WR

	INTCON2bits.GIE = 0;            // Disable interrupts for next few instructions for unlock sequence
	__builtin_write_NVM();
    while(NVMCONbits.WR == 1){}
	INTCON2bits.GIE = 1;            // Re-enable the interrupts (if required).

	return NVMCONbits.WRERR;        // Return WRERR state.
}

/*********************************************************************
 * Function:        unsigned int NVMWriteWord(UINT32 address, UINT32 data)
 *
 * Description:     The word at the location pointed to by NVMADDR is programmed.
 *
 * PreCondition:    None
 *
 * Inputs:          address:   Destination address to write.
 *                  data:      Word to write.
 *
 * Output:          '0' if operation completed successfully.
 *
 * Example:         NVMWriteWord(0xBD000000, 0x12345678)
 ********************************************************************/
UINT NVMemWriteWord(UINT32 address, UINT32 data)
{
   	DWORD_VAL writeAddress;
   	DWORD_VAL writeData;
   	
   	writeAddress.Val = address;
   	writeData.Val = data;

    NVMCON = 0x4001;                // Perform WORD write next time WR gets set = 1.
    NVMADRU = writeAddress.word.HW;
    NVMADR = writeAddress.word.LW;

	// Set the table address of "Latch". The data is programmed into the FLASH from a temporary latch. 
	TBLPAG = 0xFA;
    // The smallest block of data that can be programmed in
    // a single operation is 2 instruction words (6 Bytes + 2 Phantom Bytes).
    // Mask the high or low instruction words depending on the address and write either high or low instruction word.
	if (address % 4)
	{
		__builtin_tblwtl(0, 0xFFFF);				// Mask the low word of 1-st instruction into the latch.
		__builtin_tblwth(1, 0x00FF);				// Mask the high word of 1-st instruction into the latch. (8 bits of data + 8 bits of "phantom data" (phantom byte is always 0))
															
		__builtin_tblwtl(2, writeData.word.LW);		// Write the low word of 2-nd instruction into the latch
		__builtin_tblwth(3, writeData.word.HW);		// Write the high word of 2-nd instruction into the latch 		
	}
	else
	{
		__builtin_tblwtl(0, writeData.word.LW);		// Write the low word of 1-st instruction into the latch
		__builtin_tblwth(1, writeData.word.HW);		// Write the high word of 1-st instruction into the latch 
		__builtin_tblwtl(2, 0xFFFF);				// Mask the low word of 2-nd instruction into the latch.
		__builtin_tblwth(3, 0x00FF);				// Mask the high word of 2-nd instruction into the latch. (8 bits of data + 8 bits of "phantom data" (phantom byte is always 0))
	}		

	INTCON2bits.GIE = 0;							// Disable interrupts for next few instructions for unlock sequence
	__builtin_write_NVM();
    while(NVMCONbits.WR == 1){}
	INTCON2bits.GIE = 1;							// Re-enable the interrupts (if required).

	return NVMCONbits.WRERR;                        // Return WRERR state.
}

/***********************End of File*************************************************************/

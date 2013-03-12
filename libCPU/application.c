/*******************************************************************************
  16-bit CPU Self-Test Library User Application Example 

  Company:
    Microchip Technology Inc.

  File Name:
    Application.c

  Summary:
    User application that demonstrates the usage of the library

  Description:
    User application that calls each test subset function every 2 ms in a periodic loop. 
    The test subsets and corresponding ISRs are in separate files.
*******************************************************************************/

/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

//#include <xc.h>		// Header file for the device selected

#ifdef __dsPIC33FJ256GP710A__
#include "p33FJ256GP710A.h"
#endif

#ifdef __dsPIC33EP512MU810__
#include "p33EP512MU810.h"
#endif

#ifdef __dsPIC33FJ128MC708__
#include "p33FJ128MC708.h"
#endif


#if 0
/* ------------------------------------------------------------------------------------- */
/* Device configuration registers                                                        */
/* NOTE: The following settings should be changed as per application system requirements */
/* ------------------------------------------------------------------------------------- */
_FOSCSEL(FNOSC_FRC & IESO_OFF);	// Primary (XT, HS, EC) Oscillator 
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT);  
								// Clock Switching is enabled and Fail Safe Clock Monitor is disabled
								// OSC2 Pin Function: OSC2 is Clock Output
								// Primary Oscillator Mode: XT Crystal
_FWDT(FWDTEN_OFF);              // Watchdog Timer enabled/disabled by user software
#if defined(__dsPIC33EP64MC506__)
	_FICD(ICS_PGD2 & JTAGEN_OFF); // Use PGEC2/PGED2 for debugging on Explorer 16 Board 
#else
	_FICD(ICS_PGD1 & JTAGEN_OFF); // Use PGEC1/PGED1 for debugging on Explorer 16 Board
#endif
#endif // 0

// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************
// Declaration for TestSubsets
   extern int TestSubset_1(void);
   extern int TestSubset_2(void);
   extern int TestSubset_3(void);
   extern int TestSubset_4(void);
   extern int TestSubset_5(void);
   extern int TestSubset_6(void);
#if defined(__dsPIC33F__) || defined(__dsPIC33E__)
   extern int TestSubset_7(void);   // These declarations to be included when
   extern int TestSubset_8(void);   // dsPIC33F device is present.
#endif

// Declare the Timer T2 Interrupt Service Routine                            
   void __attribute__((__interrupt__)) _T2Interrupt( void );
   
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
// Define global variables Execute_Subset and Subset_Called                  
   unsigned int Execute_Subset;
   unsigned int Subset_Called;

/* ----------------------------------------------------------------------- */
/* Define arrays to hold the results as well as the expected test results. */
/* NOTE: Some ExpectedResults[] values will vary for other device variants */
/* ----------------------------------------------------------------------- */
#if defined(__dsPIC33FJ256GP710A__)
   int TestResults[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   int ExpectedResults[8] = {0x694D, 0x6003, 0xC2A7, 0x00F8, 0x1BD2, 0xAE40, 0xE340, 0x1D7D};
#elif defined(__PIC24HJ256GP610A__)
   int TestResults[6] = {0, 0, 0, 0, 0, 0};
   int ExpectedResults[6] = {0x694D, 0x5F7F, 0xC2A7, 0x00F8, 0x1BD2, 0xAE40};
#elif defined(__dsPIC33EP512MU810__)
   int TestResults[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   int ExpectedResults[8] = {0x694D, 0x7476, 0xC2AB, 0x00F8, 0x1BD2, 0xAE40, 0x7222, 0x1D7D};
#elif defined(__PIC24EP512GU810__)
   int TestResults[6] = {0, 0, 0, 0, 0, 0};
   int ExpectedResults[6] = {0x694D, 0x743A, 0xC2AB, 0x00F8, 0x1BD2, 0xAE40};
#elif defined(__dsPIC33EP64MC506__)
   int TestResults[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   int ExpectedResults[8] = {0x694D, 0xCE7E, 0xC2AB, 0x00F8, 0x1BD2, 0xAE40, 0xABA4, 0x1D7D};
#endif

/************************** Timer T2 Interrupt Handler ***********************/
/* Description : The Timer T2 interrupt Flag (T2IF) is cleared and the       */
/*               variable "Execute_Subset" is initialized to 1.              */
/*****************************************************************************/
void __attribute__((interrupt,no_auto_psv)) _T2Interrupt( void )
{
   IFS0bits.T2IF = 0;
   Execute_Subset = 1;
}

/*****************************************************************************/
/*                      MAIN PROGRAM STARTS HERE                             */
/*****************************************************************************/

int main_cpu_test (void)
{
#if 0
	// Configure Oscillator to operate the device at  
	// Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	// Customize based on device operating speed
#if defined(__dsPIC33E__) || defined(__PIC24E__)
	// Fosc= 8M*60/(2*2)=120Mhz for 8M input clock
	PLLFBD=58;					// M=60
	CLKDIVbits.PLLPOST=0;		// N1=2
	CLKDIVbits.PLLPRE=0;		// N2=2
	OSCTUN=0;					// Tune FRC oscillator, if FRC is used
#elif defined(__dsPIC33F__) || defined(__PIC24H__)
	// Fosc= 8M*40/(2*2)=80Mhz for 8M input clock
	PLLFBD=38;					// M=40
	CLKDIVbits.PLLPOST=0;		// N1=2
	CLKDIVbits.PLLPRE=0;		// N2=2
	OSCTUN=0;					// Tune FRC oscillator, if FRC is used
#endif

	// Disable Watch Dog Timer
	RCONbits.SWDTEN=0;

	// Clock switch to incorporate PLL
	__builtin_write_OSCCONH(0x03);		// Initiate Clock Switch to Primary
										// Oscillator with PLL (NOSC=0b011)
	__builtin_write_OSCCONL(0x01);	// Start clock switching
	while (OSCCONbits.COSC != 0b011);	// Wait for Clock switch to occur

	// Wait for PLL to lock
	while(OSCCONbits.LOCK!=1) {};	

// Initialize LEDs to an Off state
        TRISA = 0;
        LATA = 0x0000;

// Initializing the Execute_Subset and Subset_Called
	Execute_Subset = 1;            // If Execute_Subset = 1,
                                  // TestSubset can be executed.
                                  // If Execute_Subset = 0,
                                  // TestSubset can't be executed.
	Subset_Called = 1;             // Subset_Called Can have values from 1 to 8
                                  // depending on TestSubset called by
                                  // application.

// Load Timer T2 Period Register(PR2) to have period value = 2ms
// Customize based on device operating speed
#if defined(__dsPIC33F__) || defined(__PIC24H__)
	T2CON = 0x0000;
	PR2 = 0x9C40;
#elif defined(__dsPIC33E__) || defined(__PIC24E__)
	T2CON = 0x0001;
	PR2 = 0x4E20;
#endif

// Enable Timer T2 interrupt.
	_T2IE = 1;

// Assign the lowest priority to Timer T2 interrupt.
	_T2IP = 1;

// Clearing the Timer T2.
	TMR2 = 0;

// Starting Timer T2 with Internal Clock.
	T2CONbits.TON = 1;

// Execute_Subset and Subset_Called are initialized to 1
// in order to start the execution of TestSubset_1 with
// the start of the Timer T2.
	while(1)                      // Continue in an infinite loop
	{
		if (Execute_Subset == 1)  // Keep waiting for T2 ISR to set Execute_Subset.
		{
			Execute_Subset = 0;   // Execute_Subset is cleared to prevent
		                          // execution of next TestSubset immediately

#else
	while(1)                      // Continue in an infinite loop
	{
#endif // 0

		switch(Subset_Called) // after the Current TestSubset.
		{
			   case 1 :
			         TestResults[0] = TestSubset_1();
			         break;
			   case 2 :
 					 TestResults[1] = TestSubset_2();
					 break;
			   case 3 :
			         TestResults[2] = TestSubset_3();
			         break;
			   case 4 :
			         TestResults[3] = TestSubset_4();
			         break;
			   case 5 :
			         TestResults[4] = TestSubset_5();
			         break;
			   case 6 :
			         TestResults[5] = TestSubset_6();
			         break;
#if defined(__dsPIC33F__) || defined(__dsPIC33E__)
			   case 7 :
			         TestResults[6] = TestSubset_7(); 
			         break;                
			   case 8 :
			         TestResults[7] = TestSubset_8();
			         break;                
#endif
			   default :
			         Nop();
		}                            // End of Switch(Subset_Called)

// ****************************************************************************
// If the device selected is PIC24H/PIC24E   - Subset_Called vary from 1 to 6.
// If the device selected is dsPIC33F/dsPIC33E - Subset_Called vary from 1 to 8.
// ****************************************************************************

#if defined(__dsPIC33F__) || defined(__dsPIC33E__)
		if (Subset_Called >= 8)    	// This condition is checked if dsPIC device is present
		{
		   	if ( (TestResults[0] != ExpectedResults[0]) || (TestResults[1] != ExpectedResults[1]) \
					|| (TestResults[2] != ExpectedResults[2]) || (TestResults[3] != ExpectedResults[3]) \
					|| (TestResults[4] != ExpectedResults[4]) || (TestResults[5] != ExpectedResults[5]) \
					|| (TestResults[6] != ExpectedResults[6]) || (TestResults[7] != ExpectedResults[7]) )
#else                             	
		if (Subset_Called >= 6)  // This condition is checked if PIC24 device is present
		{
		   	if ( (TestResults[0] != ExpectedResults[0]) || (TestResults[1] != ExpectedResults[1]) \
					|| (TestResults[2] != ExpectedResults[2]) || (TestResults[3] != ExpectedResults[3]) \
					|| (TestResults[4] != ExpectedResults[4]) || (TestResults[5] != ExpectedResults[5]) ) 
#endif                            	
		   	{
//		   		LATAbits.LATA0 = 0;
//				LATAbits.LATA1 = 1;
//				while(1);
              printf("CPU test failed\r\n");
              return -1;
		   	}
		   	else
		   	{
//				LATAbits.LATA0 = 1;
//				LATAbits.LATA1 = 0;				
              printf("CPU test passed\r\n");
              return 0;
		   	}
		    Subset_Called = 1;
		}
		else
		{
		    Subset_Called ++;
		}

#if 0
		}                             // End of if(Execute_Subset == 1)
#endif // 0
	}                              // End for while(1)
}

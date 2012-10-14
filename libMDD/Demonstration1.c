/*****************************************************************************
 
                Microchip Memory Disk Drive File System
 
 *****************************************************************************
 FileName:        Demonstration1.c
 Dependencies:    FSIO.h
 Processor:       dsPIC33E, PIC24E
 Compiler:        C30
 Company:         Microchip Technology, Inc.

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

 THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.


   Note:  This file is included to give you a basic demonstration of how the
           functions in this library work.  Prototypes for these functions,
           along with more information about them, can be found in FSIO.h
*****************************************************************************/

//DOM-IGNORE-BEGIN
/********************************************************************
 Change History:
  Rev            Description
  ----           -----------------------
  1.3.4          Initial Revision
********************************************************************/
//DOM-IGNORE-END

/*******************************************************************************
//NOTE : DISABLE MACRO "SUPPORT_LFN" IN "FSconfig.h" FILE TO WORK WITH THIS DEMO
         EFFECTIVELY. DISABLING "SUPPORT_LFN" WILL SAVE LOT OF MEMORY FOR THIS
         DEMO.
********************************************************************************/

#include "FSIO.h"
#include "../libUDB/delay.h"

char sendBuffer[] = "Time for bed Ptolemy";
char send2[] = "2";

char receiveBuffer[50];

//void delay_ms(unsigned long ms);

int fs_test(void)
{
   FSFILE * pointer;
   char path[30];
   char count = 30;
   char * pointer2;
   SearchRec rec;
   unsigned char attributes;
   unsigned char size = 0, i;

#define LED_OFF 1

//	PLLFBDbits.PLLDIV = 30 ; // FOSC = 32 MHz (XT = 8.00MHz, N1=2, N2=4, M = 32)
//	_TRISE1 = _TRISE2 = _TRISE3 = _TRISE4 = 0 ; // init LED's
//	_LATE1 = _LATE2 = _LATE3 = _LATE4 = LED_OFF ;

//	uart1_init();
//	uart1_set_rate(57600) ;
//	uart1_puts("hello world\r\n");
//	printf("Hello World %u\r\n", 55566);

	MDD_InitIO();

//   while (!MDD_MediaDetect());
   	do {
//		Delayms(250);
	} while (!MDD_MediaDetect());

   // Initialize the library
//   while (!FSInit());
   do {
		delay_ms(250L);
		printf("calling FSInit()\r\n");
	} while (!FSInit());
	printf("FSInit() completed.\r\n");

#ifdef ALLOW_WRITES
   // Create a file
   pointer = FSfopen ("FILE3.TXT", "w");
   if (pointer == NULL)
      while(1);

   // Write 21 1-byte objects from sendBuffer into the file
   if (FSfwrite (sendBuffer, 1, 21, pointer) != 21)
      while(1);

   // FSftell returns the file's current position
   if (FSftell (pointer) != 21)
      while(1);

   // FSfseek sets the position one byte before the end
   // It can also set the position of a file forward from the
   // beginning or forward from the current position
   if (FSfseek(pointer, 1, SEEK_END))
      while(1);

   // Write a 2 at the end of the string
   if (FSfwrite (send2, 1, 1, pointer) != 1)
      while(1);

   // Close the file
   if (FSfclose (pointer))
      while(1);

   // Create a second file
   pointer = FSfopen ("FILE1.TXT", "w");
   if (pointer == NULL)
      while(1);

   // Write the string to it again
   if (FSfwrite ((void *)sendBuffer, 1, 21, pointer) != 21)
      while(1);

   // Close the file
   if (FSfclose (pointer))
      while(1);
#endif

   // Open file 1 in read mode
   pointer = FSfopen ("FILE3.TXT", "r");
   if (pointer == NULL)
      while(1);

   if (FSrename ("FILE2.TXT", pointer))
      while(1);

   // Read one four-byte object
   if (FSfread (receiveBuffer, 4, 1, pointer) != 1)
      while(1);

   // Check if this is the end of the file- it shouldn't be
   if (FSfeof (pointer))
      while(1);

   // Close the file
   if (FSfclose (pointer))
      while(1);

   // Make sure we read correctly
   if ((receiveBuffer[0] != 'T') ||
         (receiveBuffer[1] != 'h')  ||
         (receiveBuffer[2] != 'i')  ||
         (receiveBuffer[3] != 's'))
   {
      while(1);
   }

#ifdef ALLOW_DIRS
   // Create a small directory tree
   // Beginning the path string with a '.' will create the tree in
   // the current directory.  Beginning with a '..' would create the
   // tree in the previous directory.  Beginning with just a '\' would
   // create the tree in the root directory.  Beginning with a dir name
   // would also create the tree in the current directory
   if (FSmkdir (".\\ONE\\TWO\\THREE"))
      while(1);

   // Change to directory THREE in our new tree
   if (FSchdir ("ONE\\TWO\\THREE"))
      while(1);

   // Create another tree in directory THREE
   if (FSmkdir ("FOUR\\FIVE\\SIX"))
      while(1);

   // Create a third file in directory THREE
   pointer = FSfopen ("FILE3.TXT", "w");
   if (pointer == NULL)
      while(1);

   // Get the name of the current working directory
   /* it should be "\ONE\TWO\THREE"       */
   pointer2 = FSgetcwd (path, count);
   if (pointer2 != path)
      while(1);

   // Simple string length calculation
   i = 0;
   while(*(path + i) != 0x00)
   {
      size++;
      i++;
   }
   // Write the name to FILE3.TXT
   if (FSfwrite (path, size, 1, pointer) != 1)
      while(1);

   // Close the file
   if (FSfclose (pointer))
      while(1);

   // Create some more directories
   if (FSmkdir ("FOUR\\FIVE\\SEVEN\\..\\EIGHT\\..\\..\\NINE\\TEN\\..\\ELEVEN\\..\\TWELVE"))
      while(1);

   /*******************************************************************
      Now our tree looks like this

      \ -> ONE -> TWO -> THREE -> FOUR -> FIVE -> SIX
                                                 -> SEVEN
                                                 -> EIGHT
                                            NINE -> TEN
                                                 -> ELEVEN
                                                 -> TWELVE
   ********************************************************************/

   // This will delete only directory eight
   // If we tried to delete directory FIVE with this call, the FSrmdir
   // function would return -1, since FIVE is non-empty
   if (FSrmdir ("\\ONE\\TWO\\THREE\\FOUR\\FIVE\\EIGHT", FALSE))
      while(1);

   // This will delete directory NINE and all three of its sub-directories
   if (FSrmdir ("FOUR\\NINE", TRUE))
      while(1);

   // Change directory to the root dir
   if (FSchdir ("\\"))
      while(1);
#endif

#ifdef ALLOW_FILESEARCH
   // Set attributes
   attributes = ATTR_ARCHIVE | ATTR_READ_ONLY | ATTR_HIDDEN;

   // Functions "FindFirst" & "FindNext" can be used to find files
   // and directories with required attributes in the current working directory.

   // Find the first TXT file with any (or none) of those attributes that
   // has a name beginning with the letters "FILE"
   // These functions are more useful for finding out which files are
   // in your current working directory
   if (FindFirst ("FILE*.TXT", attributes, &rec))
      while(1);

   // Keep finding files until we get FILE2.TXT
   while(rec.filename[4] != '2')
   {
      if (FindNext (&rec))
         while(1);
   }

   // Delete file 2
   // NOTE : "FSremove" function deletes specific file not directory.
   //        To delete directories use "FSrmdir" function
   if (FSremove (rec.filename))
      while(1);
#endif

/*********************************************************************
   The final contents of our card should look like this:
   \ -> FILE1.TXT
      -> ONE       -> TWO -> THREE -> FILE3.TXT
                                   -> FOUR      -> FIVE -> SIX
                                                        -> SEVEN
*********************************************************************/

//   while(1);
	return 0;
}









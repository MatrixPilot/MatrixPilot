#include "Compiler.h"
#include "FSIO.h"
#include "GenericTypeDefs.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "FSDefs.h"


extern DISK gDiskData;         // Global structure containing device information.


void DisplayFS(void)
{
// Summary: A structure containing information about the device.
// Description: The DISK structure contains information about the device being accessed.

	printf("firsts %u\r\n", (unsigned int)gDiskData.firsts);
	printf("fat %u\r\n", (unsigned int)gDiskData.fat );
	printf("root %u\r\n", (unsigned int)gDiskData.root );
	printf("data %u\r\n", (unsigned int)gDiskData.data );
	printf("maxroot %u\r\n", (unsigned int)gDiskData.maxroot );
	printf("maxcls %u\r\n", (unsigned int)gDiskData.maxcls );
	printf("sectorSize %u\r\n", (unsigned int)gDiskData.sectorSize );
	printf("fatsize %u\r\n", (unsigned int)gDiskData.fatsize );
	printf("fatcopy %u\r\n", (unsigned int)gDiskData.fatcopy );
	printf("SecPerClus %u\r\n", (unsigned int)gDiskData.SecPerClus );
	printf("type %u\r\n", (unsigned int)gDiskData.type );
	printf("mount %u\r\n", (unsigned int)gDiskData.mount );
/*
    BYTE    *   buffer;         // Address of the global data buffer used to read and write file information
    DWORD       firsts;         // Logical block address of the first sector of the FAT partition on the device
    DWORD       fat;            // Logical block address of the FAT
    DWORD       root;           // Logical block address of the root directory
    DWORD       data;           // Logical block address of the data section of the device.
    WORD        maxroot;        // The maximum number of entries in the root directory.
    DWORD       maxcls;         // The maximum number of clusters in the partition.
    DWORD       sectorSize;     // The size of a sector in bytes
    DWORD       fatsize;        // The number of sectors in the FAT
    BYTE        fatcopy;        // The number of copies of the FAT in the partition
    BYTE        SecPerClus;     // The number of sectors per cluster in the data region
    BYTE        type;           // The file system type of the partition (FAT12, FAT16 or FAT32)
    BYTE        mount;          // Device mount flag (TRUE if disk was mounted successfully, FALSE otherwise)
 */
}


static FSFILE * logfile;

int fs_log(char* str)
{
	int len = strlen(str);

	if (logfile != NULL) {
		if (FSfwrite(str, 1, strlen(str), logfile) != len) {
			printf("ERROR: fswrite failed\n");
			return -1;
		}
	}
	return 0;
}

int fs_openlog(void)
{
	logfile = FSfopen("logfile.txt", "w");
	if (logfile != NULL) {
		printf("logfile opened\n");
	} else {
		printf("ERROR: fsopen failed\n");
		return -1;
	}
	return 0;
}


char sendBuffer[] = "This is test string 1";
char send2[] = "2";
char receiveBuffer[50];

int TestFS(void)
{
   FSFILE * pointer;
   char path[30];
   char count = 30;
   char * pointer2;
   SearchRec rec;
   unsigned char attributes;
   unsigned char size = 0, i;

#ifdef ALLOW_WRITES
printf("1\r\n");
   // Create a file
   pointer = FSfopen ("FILE3.TXT", "w");
   if (pointer == NULL)
      while(1);

printf("2\r\n");
   // Write 21 1-byte objects from sendBuffer into the file
   if (FSfwrite (sendBuffer, 1, 21, pointer) != 21)
      while(1);

printf("3\r\n");
   // FSftell returns the file's current position
   if (FSftell (pointer) != 21)
      while(1);

printf("4\r\n");
   // FSfseek sets the position one byte before the end
   // It can also set the position of a file forward from the
   // beginning or forward from the current position
   if (FSfseek(pointer, 1, SEEK_END))
      while(1);

printf("5\r\n");
   // Write a 2 at the end of the string
   if (FSfwrite (send2, 1, 1, pointer) != 1)
      while(1);

printf("6\r\n");
   // Close the file
   if (FSfclose (pointer))
      while(1);

printf("7\r\n");
   // Create a second file
   pointer = FSfopen ("FILE1.TXT", "w");
   if (pointer == NULL)
      while(1);

printf("8\r\n");
   // Write the string to it again
   if (FSfwrite ((void *)sendBuffer, 1, 21, pointer) != 21)
      while(1);

printf("9\r\n");
   // Close the file
   if (FSfclose (pointer))
      while(1);
#endif

printf("a\r\n");
   // Open file 1 in read mode
   pointer = FSfopen ("FILE3.TXT", "r");
   if (pointer == NULL)
      while(1);

printf("b\r\n");
   if (FSrename ("FILE2.TXT", pointer))
      while(1);

printf("c\r\n");
   // Read one four-byte object
   if (FSfread (receiveBuffer, 4, 1, pointer) != 1)
      while(1);

printf("d\r\n");
   // Check if this is the end of the file- it shouldn't be
   if (FSfeof (pointer))
      while(1);

printf("e\r\n");
   // Close the file
   if (FSfclose (pointer))
      while(1);

printf("f\r\n");
   // Make sure we read correctly
   if ((receiveBuffer[0] != 'T') ||
         (receiveBuffer[1] != 'h')  ||
         (receiveBuffer[2] != 'i')  ||
         (receiveBuffer[3] != 's'))
   {
      while(1);
   }

#ifdef ALLOW_DIRS
printf("g\r\n");
   // Create a small directory tree
   // Beginning the path string with a '.' will create the tree in
   // the current directory.  Beginning with a '..' would create the
   // tree in the previous directory.  Beginning with just a '\' would
   // create the tree in the root directory.  Beginning with a dir name
   // would also create the tree in the current directory
   if (FSmkdir (".\\ONE\\TWO\\THREE"))
      while(1);

printf("h\r\n");
   // Change to directory THREE in our new tree
   if (FSchdir ("ONE\\TWO\\THREE"))
      while(1);

printf("i\r\n");
   // Create another tree in directory THREE
   if (FSmkdir ("FOUR\\FIVE\\SIX"))
      while(1);

printf("j\r\n");
   // Create a third file in directory THREE
   pointer = FSfopen ("FILE3.TXT", "w");
   if (pointer == NULL)
      while(1);

printf("k\r\n");
   // Get the name of the current working directory
   /* it should be "\ONE\TWO\THREE"       */
   pointer2 = FSgetcwd (path, count);
   if (pointer2 != path)
      while(1);

printf("l\r\n");
   // Simple string length calculation
   i = 0;
   while(*(path + i) != 0x00)
   {
      size++;
      i++;
   }

printf("m\r\n");
   // Write the name to FILE3.TXT
   if (FSfwrite (path, size, 1, pointer) != 1)
      while(1);

printf("n\r\n");
   // Close the file
   if (FSfclose (pointer))
      while(1);

printf("o\r\n");
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

printf("p\r\n");
   // This will delete only directory eight
   // If we tried to delete directory FIVE with this call, the FSrmdir
   // function would return -1, since FIVE is non-empty
   if (FSrmdir ("\\ONE\\TWO\\THREE\\FOUR\\FIVE\\EIGHT", FALSE))
      while(1);

printf("q\r\n");
   // This will delete directory NINE and all three of its sub-directories
   if (FSrmdir ("FOUR\\NINE", TRUE))
      while(1);

printf("r\r\n");
   // Change directory to the root dir
   if (FSchdir ("\\"))
      while(1);
#endif

#ifdef ALLOW_FILESEARCH
   // Set attributes
   attributes = ATTR_ARCHIVE | ATTR_READ_ONLY | ATTR_HIDDEN;

   // Functions "FindFirst" & "FindNext" can be used to find files
   // and directories with required attributes in the current working directory.

printf("s\r\n");
   // Find the first TXT file with any (or none) of those attributes that
   // has a name beginning with the letters "FILE"
   // These functions are more useful for finding out which files are
   // in your current working directory
   if (FindFirst ("FILE*.TXT", attributes, &rec))
      while(1);

printf("t\r\n");
   // Keep finding files until we get FILE2.TXT
   while(rec.filename[4] != '2')
   {
      if (FindNext (&rec))
         while(1);
   }

printf("u\r\n");
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

printf("TestFS complete\r\n");
   return 0;
}

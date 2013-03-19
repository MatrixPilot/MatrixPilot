#include "GenericTypeDefs.h"
#include "FSconfig.h"
#include "FSDefs.h"

#define FALSE	0
#define TRUE	!FALSE

#define INITIALIZATION_VALUE		0x55

BYTE MDD_AT45D_MediaDetect(void);
MEDIA_INFORMATION * MDD_AT45D_MediaInitialize(void);
BYTE MDD_AT45D_SectorRead(DWORD sector_addr, BYTE* buffer);
BYTE MDD_AT45D_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero);
WORD MDD_AT45D_ReadSectorSize(void);
DWORD MDD_AT45D_ReadCapacity(void);
BYTE MDD_AT45D_WriteProtectState(void);

#if !defined(MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT)
    #define MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT 16
#endif

//Note: If only 1 FAT sector is used, assuming 12-bit (1.5 byte) FAT entry size 
//(ex: FAT12 filesystem), then the total FAT entries that can fit in a single 512 
//byte FAT sector is (512 bytes) / (1.5 bytes/entry) = 341 entries.  This allows 
//the FAT table to reference up to 341*512 = ~174kB of space.  Therfore, more 
//FAT sectors are needed if creating an MSD volume bigger than this.
#define MDD_AT45D_FLASH_NUM_RESERVED_SECTORS 1          
#define MDD_AT45D_FLASH_NUM_VBR_SECTORS 1       
#define MDD_AT45D_FLASH_NUM_FAT_SECTORS 1                
#define MDD_AT45D_FLASH_NUM_ROOT_DIRECTORY_SECTORS ((MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT+15)/16) //+15 because the compiler truncates
#define MDD_AT45D_FLASH_OVERHEAD_SECTORS (\
            MDD_AT45D_FLASH_NUM_RESERVED_SECTORS + \
            MDD_AT45D_FLASH_NUM_VBR_SECTORS + \
            MDD_AT45D_FLASH_NUM_ROOT_DIRECTORY_SECTORS + \
            MDD_AT45D_FLASH_NUM_FAT_SECTORS)

#define MDD_AT45D_FLASH_TOTAL_DISK_SIZE (\
            MDD_AT45D_FLASH_OVERHEAD_SECTORS + \
            MDD_AT45D_FLASH_DRIVE_CAPACITY)

#define MDD_AT45D_FLASH_PARTITION_SIZE (DWORD)(MDD_AT45D_FLASH_TOTAL_DISK_SIZE - 1)  //-1 is to exclude the sector used for the MBR 


//---------------------------------------------------------
//Do some build time error checking
//---------------------------------------------------------
#if defined(__C30__)
    #if(MDD_AT45D_FLASH_TOTAL_DISK_SIZE % 2)
        #warning "MSD volume overlaps flash erase page with firmware program memory.  Please change your FSconfig.h settings to ensure the MSD volume cannot share an erase page with the firmware."
        //See code comments in FSconfig.h, and adjust the MDD_AT45D_FLASH_DRIVE_CAPACITY definition until the warning goes away.
    #endif
#endif

#if (MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT>64)
    #if defined(__C30__)
        #error "PSV only allows 32KB of memory.  The drive options selected result in more than 32KB of data.  Please reduce the number of root directory entries possible"
    #endif
#endif

#if (MEDIA_SECTOR_SIZE != 512)
    #error "The current implementation of internal flash MDD only supports a media sector size of 512.  Please modify your selected value in the FSconfig.h file."
#endif

#if (MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT != 16) && \
    (MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT != 32) && \
    (MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT != 48) && \
    (MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT != 64)
    #error "Number of root file entries must be a multiple of 16.  Please adjust the definition in the FSconfig.h file."
#endif


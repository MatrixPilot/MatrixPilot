#include "GenericTypeDefs.h"
#include "FSconfig.h"
#include "FSDefs.h"

#define FALSE	0
#define TRUE	!FALSE

#define MDD_AT45D_FLASH_TOTAL_DISK_SIZE 8192

#define INITIALIZATION_VALUE		0x55

BYTE MDD_AT45D_InitIO(void);
BYTE MDD_AT45D_MediaDetect(void);
MEDIA_INFORMATION * MDD_AT45D_MediaInitialize(void);
BYTE MDD_AT45D_SectorRead(DWORD sector_addr, BYTE* buffer);
BYTE MDD_AT45D_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero);
WORD MDD_AT45D_ReadSectorSize(void);
DWORD MDD_AT45D_ReadCapacity(void);
BYTE MDD_AT45D_WriteProtectState(void);

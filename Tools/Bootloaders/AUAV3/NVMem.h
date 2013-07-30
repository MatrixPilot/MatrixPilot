#ifndef __NVMEM_H__
#define __NVMEM_H__


UINT NVMemWriteWord(UINT32 address, UINT32 data);
UINT NVMemErasePage(UINT32 address);
UINT NVMemBlockErase(void);


#endif // __NVMEM_H__

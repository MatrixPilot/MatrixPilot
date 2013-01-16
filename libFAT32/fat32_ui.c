
#include "thinfat32.h"

#ifdef USE_LIBFAT32

#include "fat32_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Reimplement this function correctly
int fat_type(BPB_struct *s) {
	return TYPE_FAT32;
}

void printBPB(BPB_struct *s) {
	int i,j;

	printf("==== BIOS PARAMETER BLOCK ====\n");
	printf("   Boot Instruction: 0x%02x%02x%02x\n", s->BS_JumpBoot[0], s->BS_JumpBoot[1], s->BS_JumpBoot[2]);
	printf("           OEM Name: '%8s'\n", s->BS_OEMName);
	printf("   Bytes per Sector: %d\n", s->BytesPerSector);
	printf("Sectors per Cluster: %d\n", s->SectorsPerCluster);
	printf("   Reserved Sectors: %d\n", s->ReservedSectorCount);
	printf("               FATS: %d\n", s->NumFATs);
	printf("       Root Entries: %d\n", s->RootEntryCount);
	printf("              Media: "); print_media(s->Media); printf("\n");
	printf("     Fat Size (16b): %d\n", s->FATSize16);
	printf("  Sectors per Track: %d\n", s->SectorsPerTrack);
	printf("    Number of Heads: %d\n", s->NumberOfHeads);
	printf("     Hidden Sectors: %d\n", s->HiddenSectors);
	printf("Total Sectors (16b): %d\n", s->TotalSectors16);
	printf("Total Sectors (32b): %d\n", s->TotalSectors32);
	switch(fat_type(s)) {
		case TYPE_FAT12: printf("           FAT Type: FAT12\n"); break;
		case TYPE_FAT16: printf("           FAT Type: FAT16\n"); break;
		case TYPE_FAT32: printf("           FAT Type: FAT32\n"); break;
		default: printf("FAT TYPE UNRECOGNIZED!\n"); break;
	}
	if((fat_type(s) == TYPE_FAT12) | (fat_type(s) == TYPE_FAT16)) {
		printf("       Drive Number: %d\n", s->FSTypeSpecificData.fat16.BS_DriveNumber);
		printf("     Boot Signature: 0x%02x\n", s->FSTypeSpecificData.fat16.BS_BootSig);
		printf("          Volume ID: 0x%04x\n", s->FSTypeSpecificData.fat16.BS_VolumeID);
		printf("       Volume Label: '%.*s'\n", 11, s->FSTypeSpecificData.fat16.BS_VolumeLabel);
		printf("     FileSystemType: '%.*s'\n", 8, s->FSTypeSpecificData.fat16.BS_FileSystemType);
	}
	else {
		printf("     FAT Size (32b): %d\n", s->FSTypeSpecificData.fat32.FATSize);
	   	printf("          FAT Flags: 0x%04x\n", s->FSTypeSpecificData.fat32.ExtFlags);
		printf("         FS Version: 0x%04x\n", s->FSTypeSpecificData.fat32.FSVersion);
		printf("       Root Cluster: %d\n", s->FSTypeSpecificData.fat32.RootCluster);
		printf("      FSINFO Sector: %d\n", s->FSTypeSpecificData.fat32.FSInfo);
		printf("Bkup BootRec Sector: %d\n", s->FSTypeSpecificData.fat32.BkBootSec);
		j=0;
		for(i=0; i<12; i++) {
			if(s->FSTypeSpecificData.fat32.Reserved[i] != 0x00) j=1;
		}
		if(j) printf("      Reserved Area: NONZERO!\n");
		else  printf("      Reserved Area: Ok\n");
		printf("       Drive Number: %d\n", s->FSTypeSpecificData.fat32.BS_DriveNumber);
		printf("     Boot Signature: 0x%02x\n", s->FSTypeSpecificData.fat32.BS_BootSig);
		printf("          Volume ID: 0x%04x\n", s->FSTypeSpecificData.fat32.BS_VolumeID);
		printf("       Volume Label: '%.*s'\n", 11, s->FSTypeSpecificData.fat32.BS_VolumeLabel);
		printf("     FileSystemType: '%.*s'\n", 8, s->FSTypeSpecificData.fat32.BS_FileSystemType);
	}

	printf("\n");
}

void print_media(unsigned char mediatype) {
	switch(mediatype) {
		case 0xf0: printf("Generic media type (0xf0)"); break;
		case 0xf8: printf("Hard disk (0xf8)"); break;
		case 0xf9: printf("3.5\" double sided 720k or 5.25 double sided 1.2MB (0xf9)"); break;
		case 0xfa: printf("5.25\" single sided 320k (0xfa)"); break;
		case 0xfb: printf("3.5\" double sided 640k (0xfb)"); break;
		case 0xfc: printf("5.25\" single sided 180k (0xfc)"); break;
		case 0xfd: printf("5.25\" double sided 360k (0xfd)"); break;
		case 0xfe: printf("5.25\" single sided 180k (0xfe)"); break;
		case 0xff: printf("5.25\" double sided 320k (0xff)"); break;
		default: printf("Unknown (0x%02x)",mediatype); break;
	}
}

void print_sector(unsigned char *sector) {
       int i;

	for(i=0; i<(512-8); i+=8) {
		printf(" %02x%02x%02x%02x %02x%02x%02x%02x        %c%c%c%c %c%c%c%c\n", sector[i], sector[i+1], sector[i+2], sector[i+3], sector[i+4], sector[i+5], sector[i+6], sector[i+7], sector[i], sector[i+1], sector[i+2], sector[i+3], sector[i+4], sector[i+5], sector[i+6], sector[i+7]);
	}	
}

void print_tf_info(TFInfo *t) {
	printf("    TFInfo Structure\n    ----------------\n");
	switch(t->type) {
		case TF_TYPE_FAT16: printf("               Type: FAT16\n"); break;
		case TF_TYPE_FAT32: printf("               Type: FAT32\n"); break;
		default: printf("               Type: UNRECOGNIZED! (0x%02x)\n", t->type); break;
	}
	printf("Sectors Per Cluster: %d\n", t->sectorsPerCluster);
	printf("      Total Sectors: %d\n", t->totalSectors);
	printf("   Reserved Sectors: %d\n", t->reservedSectors);
	printf("  First Data Sector: %d\n", t->firstDataSector);
}

void print_TFFile(TFFile *fp) {
	printf("     TFFile Structure\n    ----------------\n");
	printf("    currentCluster: %d\n", fp->currentCluster);
	printf(" currentClusterIdx: %d\n", fp->currentClusterIdx);
	printf("parentStartCluster: %d\n", fp->parentStartCluster);
	printf("      startCluster: %d\n", fp->startCluster);
	printf("     currentSector: %d\n", fp->currentSector);
	printf("       currentByte: %d\n", fp->currentByte);
	printf("               pos: %d\n", fp->pos);
	printf("             flags: 0x%02x\n", fp->flags);
	printf("              size: %d bytes\n", fp->size);
}

void print_FatFileEntry(FatFileEntry *entry) {
	printf("    FatFile Structure\n    ---------------\n");
	if(entry->msdos.attributes == 0x0f) {
		print_FatFileLFN(&(entry->lfn));
	}
	else {
		print_FatFile83(&(entry->msdos));
	}
}
void print_FatFile83(FatFile83 *entry) {
	printf("         Type: 8.3 Filename\n");
	printf("     Filename: %.*s\n", 8, entry->filename);
	printf("    Extension: %.*s\n", 3,  entry->extension);
	printf("   Attributes: 0x%02x\n", entry->attributes);
	printf("First Cluster: %d\n", ((entry->eaIndex & 0xffff) << 16) | ((entry->firstCluster & 0xffff)));
	printf("Creation Time: %d/%d/%d\n", ((entry->creationDate & 0xfe00) >> 9) + 1980, ((entry->creationDate & 0x1e0) >> 5), (entry->creationDate & 0xf)*2);
}


void print_FatFileLFN(FatFileLFN *entry) {
	printf("         Type: Long Filename (LFN)\n");
}

/*
typedef struct struct_FatFile83 {
	char filename[8];
	char extension[3];
	unsigned char attributes;
	unsigned char reserved;
	unsigned char creationTimeMs;
	unsigned short creationTime;
	unsigned short creationDate;
	unsigned short lastAccessTime;
	unsigned short eaIndex;
	unsigned short modifiedTime;
	unsigned short modifiedDate;
	unsigned short firstCluster;
	uint32_t fileSize;
} FatFile83;

typedef struct struct_FatFileLFN {
	unsigned char sequence_number;
	unsigned short name1[5];      // 5 Chars of name (UTF 16???)
	unsigned char attributes;     // Always 0x0f
	unsigned char reserved;       // Always 0x00
	unsigned char checksum;       // Checksum of DOS Filename.  See Docs.
	unsigned short name2[6];      // 6 More chars of name (UTF-16)
        unsigned short firstCluster;  // Always 0x0000
	unsigned short name3[2];
} FatFileLFN;
 */

#endif // USE_LIBFAT32

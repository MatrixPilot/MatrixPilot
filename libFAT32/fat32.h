#ifndef __FAT32_H
#define __FAT32_H
#include <stdlib.h>
#include <stdio.h>

#define TF_MAX_PATH 256
#define TF_FILE_HANDLES 4

#define TF_FLAG_DIRTY 0x01
#define TF_FLAG_OPEN 0x02
#define TF_FLAG_SIZECHANGED 0x04
#define TF_FLAG_ROOT 0x08

#define TYPE_FAT12 0
#define TYPE_FAT16 1
#define TYPE_FAT32 2

#define TF_MARK_BAD_CLUSTER32 0x0ffffff7
#define TF_MARK_BAD_CLUSTER16 0xfff7
#define TF_MARK_EOC32 0x0ffffff8
#define TF_MARK_EOC16 0xfff8

#define TF_ATTR_DIRECTORY 0x10

#define TF_DEBUG 1

#ifdef TF_DEBUG
typedef struct struct_TFStats {
	unsigned long sector_reads;
	unsigned long sector_writes;
} TFStats;
#endif

// Ultimately, once the filesystem is checked for consistency, you only need a few
// things to keep it up and running.  These are:
// 1) The type (fat16 or fat32, no fat12 support)
// 2) The number of sectors per cluster
// 3) Everything needed to compute indices into the FATs, which includes:
//    * Bytes per sector, which is fixed at 512
//    * The number of reserved sectors (pulled directly from the BPB)
// 4) The current sector in memory.  No sense reading it if it's already in memory!
typedef struct struct_tfinfo {
	// FILESYSTEM INFO PROPER
	unsigned char type; // 0 for FAT16, 1 for FAT32.  FAT12 NOT SUPPORTED
	unsigned char sectorsPerCluster;
	unsigned int firstDataSector;
	unsigned int totalSectors;
	unsigned short reservedSectors;
	// "LIVE" DATA
	unsigned int currentSector;
	unsigned char sectorFlags;
	unsigned int rootDirectorySize;
	unsigned char buffer[512];
} TFInfo;

// Starting at offset 36 into the BPB, this is the structure for a FAT12/16 FS
typedef struct struct_BPBFAT1216_struct {
	unsigned char     BS_DriveNumber;           // 1
	unsigned char     BS_Reserved1;             // 1
	unsigned char     BS_BootSig;               // 1
	unsigned int      BS_VolumeID;              // 4
	         char     BS_VolumeLabel[11];       // 11
		 char     BS_FileSystemType[8];     // 8
} BPB1216_struct;

// Starting at offset 36 into the BPB, this is the structure for a FAT32 FS
typedef struct struct_BPBFAT32_struct { 
	unsigned int      FATSize;             // 4
	unsigned short    ExtFlags;              // 2
	unsigned short    FSVersion;             // 2
	unsigned int      RootCluster;           // 4
	unsigned short    FSInfo;                // 2
	unsigned short    BkBootSec;             // 2
	unsigned char     Reserved[12];          // 12
	unsigned char     BS_DriveNumber;            // 1
	unsigned char     BS_Reserved1;              // 1
	unsigned char     BS_BootSig;                // 1
	unsigned int      BS_VolumeID;               // 4
	         char     BS_VolumeLabel[11];        // 11
	         char     BS_FileSystemType[8];      // 8
} BPB32_struct;

typedef struct struct_BPB_struct {
	unsigned char     BS_JumpBoot[3];            // 3
	         char     BS_OEMName[8];             // 8
	unsigned short    BytesPerSector;        // 2
	unsigned char     SectorsPerCluster;     // 1
	unsigned short    ReservedSectorCount;   // 2
	unsigned char     NumFATs;               // 1
	unsigned short    RootEntryCount;        // 2
	unsigned short    TotalSectors16;        // 2
	unsigned char     Media;                 // 1
	unsigned short    FATSize16;             // 2
	unsigned short    SectorsPerTrack;       // 2
	unsigned short    NumberOfHeads;         // 2
	unsigned int      HiddenSectors;         // 4
	unsigned int      TotalSectors32;        // 4
	union { 
		BPB1216_struct fat16;
		BPB32_struct fat32;
	} FSTypeSpecificData;
} BPB_struct;

typedef struct struct_TFFILE {
	int parentStartCluster;
	int startCluster;
	int currentClusterIdx;
	int currentCluster;
	short currentSector;
	short currentByte;
	unsigned int pos;
	unsigned char flags;
	unsigned char attributes;
	unsigned char mode;
	unsigned int size;
	char filename[TF_MAX_PATH];
} TFFile;

#define TF_MODE_READ 0x01
#define TF_MODE_WRITE 0x02
#define TF_MODE_APPEND 0x04
#define TF_MODE_OVERWRITE 0x08

#define TF_ATTR_READ_ONLY 0x01
#define TF_ATTR_HIDDEN 0x02
#define TF_ATTR_SYSTEM 0x04
#define TF_ATTR_VOLUME_LABEL 0x08
#define TF_ATTR_DIRECTORY 0x10
#define TF_ATTR_ARCHIVE 0x20
#define TF_ATTR_DEVICE 0x40 // Should never happen!
#define TF_ATTR_UNUSED 0x80

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
	unsigned int fileSize;
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

typedef union struct_FatFileEntry {
	FatFile83 msdos;
	FatFileLFN lfn;
} FatFileEntry;

// "Legacy" functions
int fat_size(BPB_struct *bpb);
int total_sectors(BPB_struct *bpb);
int root_dir_sectors(BPB_struct *bpb);
int cluster_count(BPB_struct *bpb);
int fat_type(BPB_struct *bpb);
int first_data_sector(BPB_struct *bpb);
int first_sector_of_cluster(BPB_struct *bpb, int N);
int data_sectors(BPB_struct *bpb); 
int fat_sector_number(BPB_struct *bpb, int N);
int fat_entry_offset(BPB_struct *bpb, int N);
int fat_entry_for_cluster(BPB_struct *bpb, unsigned char *buffer, int N); 
int read_sector(char *data, int blocknum);
int write_sector(char *data, int blocknum);
// New error codes
#define TF_ERR_NO_ERROR 0
#define TF_ERR_BAD_BOOT_SIGNATURE 1
#define TF_ERR_BAD_FS_TYPE 2

#define TF_ERR_INVALID_SEEK 1

// FS Types
#define TF_TYPE_FAT16 0
#define TF_TYPE_FAT32 1

// New backend functions
int tf_init();
int tf_fetch(int sector);
int tf_store();
int tf_get_fat_entry(int cluster);
int tf_set_fat_entry(int cluster, unsigned int value);
int tf_unsafe_fseek(TFFile *fp, int base, long offset); 
TFFile *tf_fnopen(char *filename, const char *mode, int n);
int tf_free_clusterchain(int cluster);
int tf_create(char *filename);
void tf_release_handle(TFFile *fp);
TFFile *tf_parent(char *filename, const char *mode);
	// New frontend functions
int tf_init();
int tf_fflush(TFFile *fp);
int tf_fseek(TFFile *fp, int base, long offset);
int tf_fclose(TFFile *fp);
int tf_fread(char *dest,  int size,  TFFile *fp);
int tf_find_file(TFFile *current_directory, char *name);
int tf_compare_filename(TFFile *fp, char *name);
int tf_first_sector(int cluster); 
char *tf_walk(char *filename, TFFile *fp);
TFFile *tf_fopen(char *filename, const char *mode);
int tf_fwrite(void *src, int size, int count, TFFile *fp);
int tf_fputs(char *src, TFFile *fp);
unsigned int tf_find_free_cluster(); 

// New Datas
extern TFInfo tf_info;
extern TFFile tf_file;
#endif

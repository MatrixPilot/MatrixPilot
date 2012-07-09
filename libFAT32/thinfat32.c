#include "thinfat32.h"
#include "fat32_ui.h"
#include "strncasecmp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// USERLAND
int read_sector(char *data, uint32_t sector) {
	FILE *fp;
	fp = fopen("test.fat32", "r+b");
	fseek(fp, sector*512, 0);
	fread(data, 1, 512, fp);
	fclose(fp);
	return 0;
}


int write_sector(char *data, uint32_t blocknum) {
	FILE *fp;
	fp = fopen("test.fat32", "r+");
	fseek(fp, blocknum*512, 0);
	fwrite(data, 1, 512, fp);
	fclose(fp);
	return 0;
}

TFInfo tf_info;
TFFile tf_file_handles[TF_FILE_HANDLES];
#ifdef TF_DEBUG
TFStats tf_stats;
#endif

/*
 * Fetch a single sector from disk.
 * ARGS
 *   sector - the sector number to fetch.
 * SIDE EFFECTS
 *   tf_info.buffer contains the 512 byte sector requested
 *   tf_info.currentSector contains the sector number retrieved
 *   if tf_info.buffer already contained a fetched sector, and was marked dirty, that sector is
 *   tf_store()d back to its appropriate location before executing the fetch.
 * RETURN
 *   the return code given by the users read_sector() (should be zero for NO ERROR, nonzero otherwise)
 */
int tf_fetch(uint32_t sector) {
	int rc=0;
	// Don't actually do the fetch if we already have it in memory
	if(sector == tf_info.currentSector) return 0;
	
	// If the sector we already have prefetched is dirty, write it before reading out the new one
	if(tf_info.sectorFlags & TF_FLAG_DIRTY) {
		rc |= tf_store();
		#ifdef TF_DEBUG
		printf("[DEBUG] Current sector (%d) dirty... storing to disk.\n", tf_info.currentSector);
		tf_stats.sector_writes += 1;
		#endif
	}
	#ifdef TF_DEBUG
	printf("[DEBUG] Fetching sector (%d) from disk.\n", sector);
	tf_stats.sector_reads += 1;
	#endif
	// Do the read, pass up the error flag
	rc |= read_sector(tf_info.buffer, sector);
	if(!rc) tf_info.currentSector = sector;
	return rc;
}

/*
 * Store the current sector back to disk
 * SIDE EFFECTS
 *   512 bytes of tf_info.buffer are stored on disk in the sector specified by tf_info.currentSector
 * RETURN
 *   the error code given by the users write_sector() (should be zero for NO ERROR, nonzero otherwise)
 */
int tf_store() {
	#ifdef TF_DEBUG
	printf("[DEBUG] Writing sector (%d) to disk.\n", tf_info.currentSector);
	#endif
	tf_info.sectorFlags &= ~TF_FLAG_DIRTY;
	return write_sector(tf_info.buffer, tf_info.currentSector);
}

/*
 * Initialize the filesystem
 * Reads filesystem info from disk into tf_info and checks that info for validity
 * SIDE EFFECTS
 *   Sector 0 is fetched into tf_info.buffer
 *   If TF_DEBUG is specified tf_stats is initialized
 * RETURN
 *   0 for a successfully initialized filesystem, nonzero otherwise.
 */
int tf_init() {
	BPB_struct *bpb;
	uint32_t fat_size, root_dir_sectors, data_sectors, cluster_count, temp;
	TFFile *fp;
	FatFileEntry e;

	// Initialize the runtime portion of the TFInfo structure, and read sec0
	tf_info.currentSector = -1;
	tf_info.sectorFlags = 0;
	tf_fetch(0);

	// Cast to a BPB, so we can extract relevant data
	bpb = (BPB_struct *) tf_info.buffer;

	/* Some sanity checks to make sure we're really dealing with FAT here
	 * see fatgen103.pdf pg. 9ff. for details */

	/* BS_jmpBoot needs to contain specific instructions */
	if (!(bpb->BS_JumpBoot[0] == 0xEB && bpb->BS_JumpBoot[2] == 0x90) && !(bpb->BS_JumpBoot[0] == 0xE9))
		return TF_ERR_BAD_FS_TYPE;

	/* Only specific bytes per sector values are allowed
	 * FIXME: Only 512 bytes are supported by thinfat at the moment */
	if (bpb->BytesPerSector != 512)
		return TF_ERR_BAD_FS_TYPE;

	if (bpb->ReservedSectorCount == 0)
		return TF_ERR_BAD_FS_TYPE;

	/* Valid media types */
	if ((bpb->Media != 0xF0) && ((bpb->Media < 0xF8) || (bpb->Media > 0xFF)))
		return TF_ERR_BAD_FS_TYPE;

	// See the FAT32 SPEC for how this is all computed
	fat_size                  = (bpb->FATSize16 != 0) ? bpb->FATSize16 : bpb->FSTypeSpecificData.fat32.FATSize;
	root_dir_sectors          = ((bpb->RootEntryCount*32) + (bpb->BytesPerSector-1))/(512); // The 512 here is a hardcoded bpb->bytesPerSector (TODO: Replace /,* with shifts?)
	tf_info.totalSectors      = (bpb->TotalSectors16 != 0) ? bpb->TotalSectors16 : bpb->TotalSectors32;
	data_sectors              = tf_info.totalSectors - (bpb->ReservedSectorCount + (bpb->NumFATs * fat_size) + root_dir_sectors);
	tf_info.sectorsPerCluster = bpb->SectorsPerCluster;
	cluster_count             = data_sectors/tf_info.sectorsPerCluster;
	tf_info.reservedSectors   = bpb->ReservedSectorCount;
	tf_info.firstDataSector    = bpb->ReservedSectorCount + (bpb->NumFATs * fat_size) + root_dir_sectors;
	
	// Now that we know the total count of clusters, we can compute the FAT type
	if(cluster_count < 65525) return TF_ERR_BAD_FS_TYPE;
	else tf_info.type = TF_TYPE_FAT32;

	#ifdef TF_DEBUG
	tf_stats.sector_reads = 0;
	tf_stats.sector_writes = 0;
	#endif

	// TODO ADD SANITY CHECKING HERE (CHECK THE BOOT SIGNATURE, ETC... ETC...)
	tf_info.rootDirectorySize = 0xffffffff;
	temp = 0;

	// Like recording the root directory size!
	// TODO, THis probably isn't necessary.  Remove later
	fp = tf_fopen("/", "r");
	do {
		temp += sizeof(FatFileEntry);
		tf_fread(&e, sizeof(FatFileEntry), fp);
	} while(e.msdos.filename[0] != '\x00');
	tf_fclose(fp);
	tf_info.rootDirectorySize = temp;
	#ifdef TF_DEBUG
	printf("[DEBUG] Size of root directory: %d bytes\n", tf_info.rootDirectorySize);
	#endif
	return 0;	
}

/*
 * Return the FAT entry for the given cluster
 * ARGS
 *   cluster - The cluster number for the requested FAT entry
 * SIDE EFFECTS
 *   Retreives whatever sector happens to contain that FAT entry (if it's not already in memory)
 * RETURN
 *   The value of the fat entry for the specified cluster.
 */
uint32_t tf_get_fat_entry(uint32_t cluster) {
	uint32_t offset=cluster*4;
	tf_fetch(tf_info.reservedSectors + (offset/512)); // 512 is hardcoded bpb->bytesPerSector
	return *((uint32_t *) &(tf_info.buffer[offset % 512]));
}

/*
 * Sets the fat entry on disk for a given cluster to the specified value.
 * ARGS
 *   cluster - The cluster number for which to set the FAT entry
 *     value - The new value for the FAT entry  
 * SIDE EFFECTS
 *   Fetches whatever sector happens to contain the pertinent fat entry (if it's not already in memory)
 * RETURN
 *   0 for no error, or nonzero for error with fetch
 * TODO
 *   Does the sector modified here need to be flagged as dirty?
 */
int tf_set_fat_entry(uint32_t cluster, uint32_t value) {
	uint32_t offset;
	int rc;
	offset=cluster*4; // FAT32
	rc = tf_fetch(tf_info.reservedSectors + (offset/512)); // 512 is hardcoded bpb->bytesPerSector
	if (*((uint32_t *) &(tf_info.buffer[offset % 512])) != value) {
		tf_info.sectorFlags |= TF_FLAG_DIRTY; // Mark this sector as dirty
		*((uint32_t *) &(tf_info.buffer[offset % 512])) = value;
	}
	return rc;
}


/*
 * Return the index of the first sector for the provided cluster
 * ARGS
 *   cluster - The cluster of interest
 * RETURN
 *   The first sector of the provided cluster
 */
uint32_t tf_first_sector(uint32_t cluster) {
	return ((cluster-2)*tf_info.sectorsPerCluster) + tf_info.firstDataSector;
}

/*
 * Walks the path provided, returning a valid file pointer for each successive level in the path.
 *
 * example:  tf_walk("/home/ryan/myfile.txt", fp to "/")
 *           Call once: returns pointer to string: home/ryan/myfile.txt, fp now points to directory for /
 *          Call again: returns pointer to string: ryan/myfile.txt, fp now points to directory for /home
 *          Call again: returns pointer to string: myfile.txt, fp now points to directory for /home/ryan
 *          Call again: returns pointer to the end of the string, fp now points to /home/ryan/myfile.txt
 *          Call again: returns NULL pointer. fp is unchanged
 * ARGS
 *   filename - a string containing the full path
 *
 * SIDE EFFECTS
 *   The filesystem is traversed, so files are opened and closed, sectors are read, etc...
 * RETURN
 *   A string pointer to the next level in the path, or NULL if this is the end of the path
 */
char *tf_walk(char *filename, TFFile *fp) {
	FatFileEntry entry;
	#ifdef TF_DEBUG
	printf("[DEBUG] Walking path '%s'\n", filename);
	#endif
	// We're out of path. this walk is COMPLETE
	if(*filename == '/') {
		filename++;
		if(*filename == '\x00') return NULL;
	}
	// There's some path left
	if(*filename != '\x00') {
		// fp is the handle for the current directory
		// filename is the name of the current file in that directory
		// Go fetch the FatFileEntry that corresponds to the current file
		// Remember that tf_find_file is only going to search from the beginning of the filename
		// up until the first path separation character
		if(tf_find_file(fp, filename)) {
			// This happens when we couldn't actually find the file
			fp->flags = 0xff;
			return NULL;
		}
		tf_fread(&entry, sizeof(FatFileEntry), fp);
		// Walk over path separators
		while((*filename != '/') && (*filename != '\x00')) filename+=1;
		if(*filename == '/') filename +=1 ;
		// Set up the file pointer now that we've got information for the next level in the path hierarchy
		fp->parentStartCluster = fp->startCluster;
		fp->startCluster = ((uint32_t)(entry.msdos.eaIndex & 0xffff) << 16) | (entry.msdos.firstCluster & 0xffff);
		fp->attributes = entry.msdos.attributes;
	       	fp->currentCluster = fp->startCluster;
		fp->currentClusterIdx=0;
		fp->currentSector=0;
		fp->currentByte=0;
		fp->pos=0;
		fp->flags=TF_FLAG_OPEN;
		fp->size=(entry.msdos.attributes & TF_ATTR_DIRECTORY) ? 0xffffffff :entry.msdos.fileSize;
		if(*filename == '\x00') return NULL;
		return filename;
	}
	// We're out of path.  This walk is COMPLETE.
	return NULL;
}

/*
 * Searches the list of system file handles for a free one, and returns it.
 * RETURN
 *   NULL if no system file handles are free, or the free handle if one is available.
 */
TFFile *tf_get_free_handle() {
	int i;
	TFFile *fp;
	for(i=0; i<TF_FILE_HANDLES; i++) {
		fp = &tf_file_handles[i];
		if(fp->flags & TF_FLAG_OPEN) continue;
		// We get here if we find a free handle
		fp->flags = TF_FLAG_OPEN;
		return fp;
	}
	return NULL;
}

/*
 * Release a filesystem handle (mark as available)
 */
void tf_release_handle(TFFile *fp) {
	fp->flags &= ~TF_FLAG_OPEN;
}

// Convert a character to uppercase
// TODO: Re-do how filename conversions are done.
char upper(char c) {
	if(c >= 'a' && c <= 'z') {
		return c + ('A'-'a');
	}
	else {
		return c;
	}
}

/*
 * Take the long filename (filename only, not full path) specified by src,
 * and convert it to an 8.3 compatible filename, storing the result at dst
 * TODO: This should return something, an error code for conversion failure.
 * TODO: This should handle special chars etc.
 * TODO: Test for short filenames, (<7 characters)
 * TODO: Modify this to use the basis name generation algorithm described in the FAT32 whitepaper.
 */
void tf_shorten_filename(char *dest, char *src) {
	int l = strlen(src);
	int i=0;
    int lossy_flag=0;
    #ifdef TF_DEBUG
        char *orig_dest = dest;
        char *orig_src = src;
    #endif
    // Copy the basename
	while(1) {
		if(i==8) break;
		if((i==6) || (*src == '.') || (*src == '\x00'))break;
		if((*dest == ' '))  {lossy_flag = 1; } else {
            *(dest++) = upper(*(src++));
        }
		i+=1;
    }
    // Funny tail if basename was too long
    if(i==6) {
		*(dest++) = '~';
		*(dest++) = '1';
        i+=2;
    }
    // Or Not
    else {
        while(i<8) {
            *(dest++) = ' ';
            i++;
        }
    }

    // Last . in the filename
	src = strrchr(src, '.');
	
    *(dest++) = ' ';
    *(dest++) = ' ';
    *(dest++) = ' ';
    *(dest++) = '\x00';
    dest -= 4;
    if(src != NULL) {
        src +=1;
	    while(i < 12) {
		    if(*src == '\x00') break;
		*(dest++) = upper(*(src++));
		i+=1;
        }
	}
}

/*
 * Create a LFN entry from the filename provided.
 * - The entry is constructed from all, or the first 13 characters in the filename (whichever is smaller)
 * - If filename is <=13 bytes long, the NULL pointer is returned
 * - If the filename >13 bytes long, an entry is constructed for the first 13 bytes, and a pointer is 
 *   returned to the remainder of the filename.
 * ARGS
 *   filename - string containing the filename for which to create an entry
 *   entry - pointer to a FatFileEntry structure, which is populated as an LFN entry
 * RETURN
 *   NULL if this is the last entry for this file, or a string pointer to the remainder of the string
 *   if the entire filename won't fit in one entry
 * WARNING
 *   Because this function works in forward order, and LFN entries are stored in reverse, it does
 *   NOT COMPUTE LFN SEQUENCE NUMBERS.  You have to do that on your own.  Also, because the function
 *   acts on partial filenames IT DOES NOT COMPUTE LFN CHECKSUMS.  You also have to do that on your own.  
 * TODO
 *   Test and further improve on this function
 */
char *tf_create_lfn_entry(char *filename, FatFileEntry *entry) {
	int i;
	for(i=0; i<5; i++) {
		if(*filename != '\x00')
			entry->lfn.name1[i] = (unsigned short) *(filename++);
		else
			entry->lfn.name1[i] = 0;
	}
	for(i=0; i<6; i++) {
		if(*filename != '\x00')
			entry->lfn.name2[i] = (unsigned short) *(filename++);
		else
			entry->lfn.name2[i] = 0;
	}
	for(i=0; i<2; i++) {
		if(*filename != '\x00')
			entry->lfn.name3[i] = (unsigned short) *(filename++);
		else
			entry->lfn.name3[i] = 0;
	}
	entry->lfn.attributes = 0x0f;
	entry->lfn.reserved = 0;
	entry->lfn.firstCluster = 0;
	if(*filename) return filename;
	else return NULL;
}
// Taken from http://en.wikipedia.org/wiki/File_Allocation_Table
//
unsigned char tf_lfn_checksum(const unsigned char *pFcbName)
{
	int i;
	unsigned char sum=0;
 
	for (i=11; i; i--)
		sum = ((sum & 1) << 7) + (sum >> 1) + *pFcbName++;
	return sum;
}

int tf_place_lfn_chain(TFFile *fp, char *filename) {
	char *strptr = filename;
	int entries=1;
	int i,j;
	char *last_strptr = filename;
	FatFileEntry entry;
	char seq;
	char sfn[12];
	tf_shorten_filename(sfn, filename);
	while(strptr = tf_create_lfn_entry(strptr, &entry)) {
		last_strptr = strptr;
		entries += 1;
	}
	seq = entries | 0x40;
	for(i=0; i<entries; i++) {
		tf_create_lfn_entry(last_strptr, &entry);
		entry.lfn.sequence_number = seq;
		entry.lfn.checksum = tf_lfn_checksum(sfn);
		#ifdef TF_DEBUG
		printf("[DEBUG] Placing LFN chain entry @ %d\n", fp->pos);
		#endif
		tf_fwrite(&entry, sizeof(FatFileEntry), 1, fp);
		seq = ((seq & ~0x40)-1);
		last_strptr -= 13;	
	}
	return 0;
}

int tf_create(char *filename) {
	TFFile *fp = tf_parent(filename, "r");
	FatFileEntry entry;
	uint32_t cluster;
	char *temp;	
	#ifdef TF_DEBUG
	printf("[DEBUG] Creating new file: '%s'\n", filename);
	#endif
	if(!fp) return 1;
	tf_fclose(fp);
	fp = tf_parent(filename, "r+");
	// Now we have the directory in which we want to create the file, open for overwrite
	do {
		//"seek" to the end
		tf_fread(&entry, sizeof(FatFileEntry), fp);
		printf("Skipping to end of file... %d\n", fp->pos);
	} while(entry.msdos.filename[0] != '\x00');
	// Back up one entry, this is where we put the new filename entry
	tf_fseek(fp, -(uint32_t)sizeof(FatFileEntry), fp->pos);
	cluster = tf_find_free_cluster();
	tf_set_fat_entry(cluster, TF_MARK_EOC32); // Marks the new cluster as the last one (but no longer free)
	// TODO shorten these entries with memset
	entry.msdos.attributes = 0;
      	entry.msdos.creationTimeMs = 0;
	entry.msdos.creationTime = 0;
	entry.msdos.creationDate = 0;
	entry.msdos.lastAccessTime = 0;
	entry.msdos.eaIndex = (cluster >> 16) & 0xffff;
	entry.msdos.modifiedTime = 0;
	entry.msdos.modifiedDate = 0;
	entry.msdos.firstCluster = cluster & 0xffff;
	entry.msdos.fileSize = 0;
	temp = strrchr(filename, '/')+1;
	printf("[DEBUG] FILENAME CONVERSION: %s\n", temp);
	tf_place_lfn_chain(fp, temp);
	tf_shorten_filename(entry.msdos.filename, temp);
	tf_fwrite(&entry, sizeof(FatFileEntry), 1, fp);
	memset(&entry, 0, sizeof(FatFileEntry));
	//entry.msdos.filename[0] = '\x00';
	tf_fwrite(&entry, sizeof(FatFileEntry), 1, fp);
	tf_fclose(fp);
	return 0;
}

TFFile *tf_fopen(char *filename, const char *mode) {
	TFFile *fp;
	#ifdef TF_DEBUG 
	printf("[DEBUG] tf_fopen(%s, %s)\n", filename, mode);
	#endif
	fp = tf_fnopen(filename, mode, strlen(filename));
	if(fp == NULL) {
		if(strchr(mode, '+') || strchr(mode, 'w') || strchr(mode, 'a')) {
		      tf_create(filename); 
		}	
		return tf_fnopen(filename, mode, strlen(filename));
	}
	return fp;
}

//
// Just like fopen, but only look at n characters of the path
TFFile *tf_fnopen(char *filename, const char *mode, int n) {
	// Request a new file handle from the system
	TFFile *fp = tf_get_free_handle();
	char myfile[256];
	char *temp_filename = myfile;
	uint32_t cluster;

	if (fp == NULL)
		return NULL;

	strncpy(myfile, filename, n);
	myfile[n] = 0;
	fp->currentCluster=2;
	fp->startCluster=2;
	fp->parentStartCluster=0xffffffff;
	fp->currentClusterIdx=0;
	fp->currentSector=0;
	fp->currentByte=0;
	fp->attributes = 0x10;
	fp->pos=0;
	fp->flags |= TF_FLAG_ROOT;
	fp->size = 0xffffffff;
	//fp->size=tf_info.rootDirectorySize;
	fp->mode=TF_MODE_READ | TF_MODE_WRITE | TF_MODE_OVERWRITE;


	
	while(temp_filename != NULL) {
		temp_filename = tf_walk(temp_filename, fp);
		if(fp->flags == 0xff) {
			tf_release_handle(fp);
			return NULL;
		}
	}
	
	if(strchr(mode, 'r')) {
		fp->mode |= TF_MODE_READ;
	}
	if(strchr(mode, 'a')) { 
		#ifdef TF_DEBUG
		printf("[DEBUG] File opened for APPEND.  Seeking to offset 0+%d\n", fp->size);
		#endif
		tf_unsafe_fseek(fp, fp->size, 0);
		fp->mode |= TF_MODE_WRITE | TF_MODE_OVERWRITE;
	}
	if(strchr(mode, '+')) fp->mode |= TF_MODE_OVERWRITE | TF_MODE_WRITE;
	if(strchr(mode, 'w')) {
		/* Opened for writing. Truncate file only if it's not a directory*/
		if (!(fp->attributes & TF_ATTR_DIRECTORY)) {
			fp->size = 0;
			tf_unsafe_fseek(fp, 0, 0);
			/* Free the clusterchain starting with the second one if the file
			 * uses more than one */
			if ((cluster = tf_get_fat_entry(fp->startCluster)) != TF_MARK_EOC32) {
				tf_free_clusterchain(cluster);
				tf_set_fat_entry(fp->startCluster, TF_MARK_EOC32);
			}
		}
		fp->mode |= TF_MODE_WRITE;
	}

	strncpy(fp->filename, myfile, n);
 		
	fp->filename[n] = 0;
	return fp;
}

int tf_free_clusterchain(uint32_t cluster) {
	uint32_t fat_entry;
	#ifdef TF_DEBUG
	printf("[DEBUG] Freeing clusterchain starting at cluster %d...\n", cluster);
	#endif
	fat_entry = tf_get_fat_entry(cluster);
	while(fat_entry < TF_MARK_EOC32) {
		#ifdef TF_DEBUG
		printf("[DEBUG] Freeing cluster %d...\n", fat_entry);
		#endif
		tf_set_fat_entry(cluster, 0x00000000);
		fat_entry = tf_get_fat_entry(fat_entry);
		cluster = fat_entry;
	}
	return 0;
}



int tf_fseek(TFFile *fp, int32_t base, long offset) {
	long pos = base+offset;
	if (pos >= fp->size) return TF_ERR_INVALID_SEEK;
	return tf_unsafe_fseek(fp, base, offset); 
}

/*
 * TODO: Make it so seek fails aren't destructive to the file handle
 */
int tf_unsafe_fseek(TFFile *fp, int32_t base, long offset) {
	uint32_t cluster_idx;
	long pos = base + offset;
	uint32_t mark = tf_info.type ? TF_MARK_EOC32 : TF_MARK_EOC16;
	uint32_t temp;
	// We're only allowed to seek one past the end of the file (For writing new stuff)
	if(pos > fp->size) {
		#ifdef TF_DEBUG
		printf("[DEBUG] SEEK ERROR (pos=%ld > fp.size=%d)\n", pos, fp->size);
		#endif
		return TF_ERR_INVALID_SEEK;
	}
	if(pos == fp->size) {
		fp->size += 1;
		fp->flags |= TF_FLAG_SIZECHANGED;
	}
	#ifdef TF_DEBUG
	//printf("[DEBUG] SEEK %d+%ld\n", base, offset);
	#endif
	// Compute the cluster index of the new location
	cluster_idx = pos / (tf_info.sectorsPerCluster*512); // The cluster we want in the file
	//print_TFFile(fp);	
	// If the cluster index matches the index we're already at, we don't need to look in the FAT
	// If it doesn't match, we have to follow the linked list to arrive at the correct cluster 
	if(cluster_idx != fp->currentClusterIdx) {
		temp = cluster_idx;
		/* Shortcut: If we are looking for a cluster that comes *after* the current we don't
		 * need to start at the beginning */
		if (cluster_idx > fp->currentClusterIdx) {
			cluster_idx -= fp->currentClusterIdx;
		} else {
			fp->currentCluster = fp->startCluster;
		}
		fp->currentClusterIdx = temp;
		while(cluster_idx > 0) {
			// TODO Check file mode here for r/w/a/etc...
			temp = tf_get_fat_entry(fp->currentCluster); // next, next, next
			if(temp&0x0fffffff != mark) fp->currentCluster = temp;
			else {
				// We've reached the last cluster in the file (omg)
				// If the file is writable, we have to allocate new space
				// If the file isn't, our job is easy, just report an error
				// Also, probably report an error if we're out of space
				temp = tf_find_free_cluster_from(fp->currentCluster);
				tf_set_fat_entry(fp->currentCluster, temp); // Allocates new space
				tf_set_fat_entry(temp, mark); // Marks the new cluster as the last one
				fp->currentCluster = temp;
			}
			cluster_idx--;
			if(fp->currentCluster >= mark) {
				if(cluster_idx > 0) {
					return 	TF_ERR_INVALID_SEEK;
				}
			}
		}
		// We now have the correct cluster number (whether we had to fetch it from the fat, or realized we already had it)
		// Now we need just compute the correct sector and byte index into the cluster
	}
	fp->currentByte = pos % (tf_info.sectorsPerCluster*512); // The offset into the cluster
	fp->pos = pos;
	return 0;
}

/*
 * Given a file handle to the current directory and a filename, populate the provided FatFileEntry with the
 * file information for the given file.
 * SIDE EFFECT: the position in current_directory will be set to the beginning of the fatfile entry (for overwriting purposes)
 */
int tf_find_file(TFFile *current_directory, char *name) {
	int rc;
	tf_fseek(current_directory, 0, 0);
	#ifdef TF_DEBUG
	printf("[DEBUG] Searching for filename: '%s' in directory '%s'\n", name, current_directory->filename);
	#endif

	while(1) {
		rc = tf_compare_filename(current_directory, name);
		if(rc < 0) break;
		else if(rc == 1)  return 0;
	}
	return -1;
}

int tf_compare_filename_segment(FatFileEntry *entry, char *name) {
	int i,j;
	char reformatted_file[16];
	char *entryname = entry->msdos.filename;
	if(entry->msdos.attributes != 0x0f) {
		#ifdef TF_DEBUG
		printf("[DEBUG] 8.3 Segment:\n");
		#endif
		// Filename
		j=0;
		for(i=0; i<8; i++) {
			if(entryname[i] != ' ') {
				reformatted_file[j++] = entryname[i];
			}
		}
		reformatted_file[j++] = '.';
		// Extension
		for(i=8; i<11; i++) {
			if(entryname[i] != ' ') {
				reformatted_file[j++] = entryname[i];
			}
		}
	}
	else {
		#ifdef TF_DEBUG
		printf("[DEBUG] LFN Segment:\n");
		#endif
		j=0;
		for(i=0; i<5; i++) {
			reformatted_file[j++] = (char) entry->lfn.name1[i];
		}
		for(i=0; i<6; i++) {
			reformatted_file[j++] = (char) entry->lfn.name2[i];
		}
		for(i=0; i<2; i++) {
			reformatted_file[j++] = (char) entry->lfn.name3[i];
		}
	}
	reformatted_file[j++] = '\x00';
	while((name[i] != '/') && (name[i] != '\x00')) i++;
	#ifdef TF_DEBUG
	printf("[DEBUG] Comparing filename segment '%s' (given) to '%s' (from disk)\n", name, reformatted_file);
	#endif
	if(strncasecmp(name, reformatted_file, i > 13 ? 13 : i)) {
		return 0; }
	else { 
		return 1; }		
}
// 
// Reads a single FatFileEntry from fp, compares it to the MSDOS filename specified by *name
// Returns:
//   1 for entry matches filename.  Side effect: fp seeks to that entry
//   0 for entry doesn't match filename.  Side effect: fp seeks to next entry
//   -1 for couldn't read an entry, due to EOF or other fread error
//
int tf_compare_filename(TFFile *fp, char *name) {
	uint32_t i,j=0;
	FatFileEntry entry;
	char *compare_name=name;
	uint32_t lfn_entries;
	
	// Read a single directory entry
	#ifdef TF_DEBUG
	printf("[DEBUG] Comparing filename @ %d\n", fp->pos);
	#endif
	tf_fread(&entry, sizeof(FatFileEntry), fp);
	
	// Fail if its bogus
	if(entry.msdos.filename[0] == 0x00) return -1;

	// If it's a DOS entry, then:
	if(entry.msdos.attributes != 0x0f) {
		// If it's a match, seek back an entry to the beginning of it, return 1
		if(tf_compare_filename_segment(&entry, name)) {
			tf_fseek(fp, -(int32_t)sizeof(FatFileEntry), fp->pos);
			return 1;
		}else {
			return 0;
		}
	}
	else {
		// If this is the first LFN entry, mask off the extra bit (0x40) and you get the number of entries in the chain
		lfn_entries = entry.lfn.sequence_number & ~0x40;
		// Seek to the last entry in the chain
		tf_fseek(fp, (int32_t)sizeof(FatFileEntry)*(lfn_entries-1), fp->pos);
		for(i=0; i<lfn_entries; i++) {
			// Seek back one and read it
			tf_fseek(fp, -(int32_t)sizeof(FatFileEntry), fp->pos);
			tf_fread(&entry, sizeof(FatFileEntry), fp);

			// Compare it.  If it's not a match, jump to the end of the chain, return failure
			// Otherwise, continue looping until there's no entries left.
			if(!tf_compare_filename_segment(&entry, compare_name)) {
				tf_fseek(fp, (int32_t)((lfn_entries-i-1))*sizeof(FatFileEntry), fp->pos);
				return 0;	
			}
			tf_fseek(fp, -(int32_t)sizeof(FatFileEntry), fp->pos);
			compare_name+=13;
		}
		// If we made it here, match was a success!  Return so.
		tf_fseek(fp, (int32_t)sizeof(FatFileEntry)*lfn_entries, fp->pos);
		return 1;
	}
	return -1;
}

int tf_fread(char *dest, int size, TFFile *fp) {
	while(size > 0) {
		tf_fetch(tf_first_sector(fp->currentCluster) + (fp->currentByte / 512));
		*dest++ = tf_info.buffer[fp->currentByte % 512];
		size--;
		if(fp->attributes & TF_ATTR_DIRECTORY) {
			//printf("READING DIRECTORY");
			if(tf_fseek(fp, 0, fp->pos+1)) {
				return -1;
			}
		} else {
		if(tf_fseek(fp, 0, fp->pos +1)) {
			return -1;	
		}
		}
	}
	return 0;
}

int tf_fwrite(void *src, int size, int count, TFFile *fp) {
	int i;
	#ifdef TF_DEBUG
	printf("[DEBUG] Call to tf_fwrite() size=%d count=%d\n", size, count);
	#endif
	fp->flags |= TF_FLAG_DIRTY;
	while(count > 0) {
		i=size;
		while(i > 0) {
			tf_fetch(tf_first_sector(fp->currentCluster) + (fp->currentByte / 512));
			tf_info.buffer[fp->currentByte % 512] = *((unsigned char *) src++);
			tf_info.sectorFlags |= TF_FLAG_DIRTY; // Mark this sector as dirty
			i--;
			if(tf_unsafe_fseek(fp, 0, fp->pos +1)) {
				return -1;	
			}
		}
		count--;
	}
	return 0;
}

int tf_fputs(char *src, TFFile *fp) {
	return tf_fwrite(src, 1, strlen(src), fp);
}

int tf_fclose(TFFile *fp) {
	int rc;
	#ifdef TF_DEBUG
	printf("[DEBUG] Closing file...\n");
	#endif
	rc =  tf_fflush(fp);
	fp->flags &= ~TF_FLAG_OPEN; // Mark the file as available for the system to use
	return rc;
}

TFFile *tf_parent(char *filename, const char *mode) {
	char *f2;
	#ifdef TF_DEBUG
	printf("[DEBUG] Opening parent of '%s'\n", filename);
	#endif
	f2 = strrchr(filename, '/');
	printf("%d\n", (int) (f2-filename)); 
	return tf_fnopen(filename, "rw", (int)(f2-filename));
}

int tf_fflush(TFFile *fp) {
	int rc = 0;
	TFFile *dir;
	FatFileEntry entry;
	char *filename=entry.msdos.filename;

	if(!(fp->flags & TF_FLAG_DIRTY)) return 0;


	#ifdef TF_DEBUG
	printf("[DEBUG] Flushing file...\n");
	#endif
	// First write any pending data to disk
	if(tf_info.sectorFlags & TF_FLAG_DIRTY) {
		rc = tf_store();
	}
	// Now go modify the directory entry for this file to reflect changes in the file's size
	// (If they occurred)
	if(fp->flags & TF_FLAG_SIZECHANGED) {

		if(fp->attributes & 0x10) {
			// TODO Deal with changes in the root directory size here
		}
		else {
			// Open the parent directory
			dir = tf_parent(fp->filename, "r+");
			filename = strrchr(fp->filename, '/');

			#ifdef TF_DEBUG
			printf("[DEBUG] Opened %s's parent for directory entry modification...\n", fp->filename);
			#endif
			// Seek to the entry we want to modify and pull it from disk
			tf_find_file(dir, filename+1);
			tf_fread(&entry, sizeof(FatFileEntry), dir);
			tf_fseek(dir, -(int32_t)sizeof(FatFileEntry), dir->pos);
			#ifdef TF_DEBUG
			printf("[DEBUG] Updating file size from %d to %d\n", entry.msdos.fileSize, fp->size-1);
			#endif
			// Modify the entry in place to reflect the new file size
			entry.msdos.fileSize = fp->size-1; 
			tf_fwrite(&entry, sizeof(FatFileEntry), 1, dir); // Write fatfile entry back to disk
			tf_fclose(dir);
		}
		fp->flags &= ~TF_FLAG_SIZECHANGED;
	}
	#ifdef TF_DEBUG
	printf("[DEBUG] Flushed.\n");
	#endif
	fp->flags &= ~TF_FLAG_DIRTY;
	return rc;
}

/*
 * Remove a file from the filesystem
 * @param filename - The full path of the file to be removed
 * @return 
 */
int tf_remove(char *filename) {
	TFFile *fp;
	FatFileEntry entry;
	int rc;
	uint32_t startCluster;

	// Sanity check
	fp = tf_fopen(filename, "r");
	if(fp == NULL) return -1; // return an error if we're removing a file that doesn't exist
	startCluster = fp->startCluster; // Remember first cluster of the file so we can remove the clusterchain
	tf_fclose(fp);

	// TODO Don't leave an orphaned LFN
	fp = tf_parent(filename, "r+");
	rc = tf_find_file(fp, (strrchr(filename, '/')+1));
	if(!rc) {
		while(1) {
			rc = tf_fseek(fp, sizeof(FatFileEntry), fp->pos);
			if(rc) break;
			tf_fread(&entry, sizeof(FatFileEntry), fp); // Read one entry ahead
			tf_fseek(fp, -(int32_t)2*sizeof(FatFileEntry), fp->pos);
			tf_fwrite(&entry, sizeof(FatFileEntry), 1, fp);
			if(entry.msdos.filename[0] == 0) break;
		}
		fp->size-=sizeof(FatFileEntry);
		fp->flags |= TF_FLAG_SIZECHANGED; 
	}
	tf_fclose(fp);
	tf_free_clusterchain(startCluster); // Free the data associated with the file

	return 0;

}


// Walk the FAT from the very first data sector and find a cluster that's available
// Return the cluster index 
// TODO: Rewrite this function so that you can start finding a free cluster at somewhere other than the beginning
uint32_t tf_find_free_cluster() {
	uint32_t i, entry;
	#ifdef TF_DEBUG
	printf("[DEBUG] Searching for a free cluster...\n");
	#endif
	for(i=tf_info.firstDataSector;i<tf_info.totalSectors; i++) {
		entry = tf_get_fat_entry(i);
		if((entry & 0x0fffffff) == 0) break;
	}
	#ifdef TF_DEBUG		
	printf("[DEBUG] Free cluster number: %d\n", i);
	#endif
	return i;
}

/* Optimize search for a free cluster */
uint32_t tf_find_free_cluster_from(uint32_t c) {
	uint32_t i, entry;
	#ifdef TF_DEBUG
	printf("[DEBUG] Searching for a free cluster...\r\n");
	#endif
	for(i=c;i<tf_info.totalSectors; i++) {
		entry = tf_get_fat_entry(i);
		if((entry & 0x0fffffff) == 0) break;
	}

	/* We couldn't find anything here so search from the beginning */
	if (i == tf_info.totalSectors) {
		return tf_find_free_cluster();
	}

	#ifdef TF_DEBUG
	printf("[DEBUG] Free cluster number: %d\r\n", i);
	#endif
	return i;
}

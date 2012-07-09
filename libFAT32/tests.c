#if 0

#include "thinfat32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fat32_ui.h"

#define NO_ERROR 0
#define FILE_OPEN_ERROR 1
#define DATA_READ_ERROR 2
#define DATA_WRITE_ERROR 3
#define DATA_MISMATCH_ERROR 4

// Tests
int basic_read(char *input_fle, char *expected);
int basic_write(char *input_file, char *write_string);
int basic_append(char *input_file, char *write_string);

int main(int argc, char **argv) {
	TFFile *fp;
	char data;
	int rc;

	printf("FAT32 Filesystem Test\n");
	printf("-----------------------\n");
	tf_init();

	// BASIC WRITE, Root directory, LFN
	if(rc = test_basic_write("/test_longfilename0.txt", "Hello, World!")) {
		printf("[TEST] Basic LFN write test failed with error code 0x%x\n", rc) ;
	}else { printf("[TEST] Basic LFN write test PASSED.\n"); }

	// BASIC READ, Root directory, LFN
	if(rc = test_basic_read("/test_longfilename0.txt", "Hello, World!")) {
		printf("[TEST] Basic LFN read test failed with error code 0x%x\n", rc) ;
	}else { printf("[TEST] Basic LFN read test PASSED.\n"); }

	// BASIC WRITE, Root directory, 8.3 filename
	if(rc = test_basic_write("/test0.txt", "Hello, World!")) {
		printf("[TEST] Basic 8.3 write test failed with error code 0x%x\n", rc);
	}else { printf("[TEST] Basic 8.3 write test PASSED.\n"); }
	// BASIC READ, Root directory, 8.3 filename
	if(rc = test_basic_read("/test0.txt", "Hello, World!")) {
		printf("[TEST] Basic 8.3 read test failed with error code 0x%x\n", rc) ;
	}else { printf("[TEST] Basic 8.3 read test PASSED.\n"); }
	
	return 0;
}

/*
 * Open a file, read its contents, if the contents match, return 0
 *If the contents don't match, or any other error occurs, return 
 * an appropriate error code.
 */
int test_basic_read(char *input_file, char *expected) {
	TFFile *fp;
	char data[128];
	int size = strlen(expected);
	fp = tf_fopen(input_file, "r");
	int i=0;

	if(fp) {
		while(!tf_fread(&(data[i]), 1, fp)) {i+=1;}
		data[i+1] = '\x00';
		if(strcmp(data, expected)) {
			tf_fclose(fp);
			return DATA_MISMATCH_ERROR;
		}
		else {
			tf_fclose(fp);
			return NO_ERROR;
		}

	}
	else {
		return FILE_OPEN_ERROR;
	}
}

/*
 * Open a file, write a string to it, return 0.
 * Return an appropriate error code if there's any problem.
 */
int test_basic_write(char *input_file, char *write_string) {
	TFFile *fp;
	int rc;

	fp = tf_fopen(input_file, "w");
	
	if(fp) {
		rc = tf_fwrite(write_string, 1, strlen(write_string), fp);
		if(rc) {
			tf_fclose(fp);
			return DATA_WRITE_ERROR;
		}
		else {
			tf_fclose(fp);
			return NO_ERROR;
		}
		
	}
	else {
		return FILE_OPEN_ERROR;
	}
}


/*
 * Open a file, append a string to it, return 0.
 * Return an appropriate error code if there's any problem.
 */
int test_basic_append(char *input_file, char *write_string) {
	TFFile *fp;
	int rc;

	fp = tf_fopen(input_file, "a");
	
	if(fp) {
		rc = tf_fwrite(write_string, 1, strlen(write_string), fp);
		if(rc) {
			tf_fclose(fp);
			return DATA_WRITE_ERROR;
		}
		else {
			tf_fclose(fp);
			return NO_ERROR;
		}
		
	}
	else {
		return FILE_OPEN_ERROR;
	}
}

#endif // 0

/*
 * fControl.h
 *
 */

#include "fileSystem.h"

#define DIRECTORY 1
#define FILE 0
#define FREE -1

#define NEXT_BLOCK 1
#define BLOCK_END -1
#define ROOT "/"

#define DATA_SIZE BLOCK_SIZE - DATA_P

// Creates a File Control Block
int createFCB(int parentFCBID, char* name);

// Creates a file
int createFile(int fcBlockID, char* name);

// Deletes a directory
int deleteDir(int fcBlockID, char* name);

// Deletes a file
int deleteFile(int fcBlockID, char* name);

// Writes to a file
int writeFile(int blockID, int start, int length, char* mem_pointer);

// Reads a file
int readFile(int blockID, int start, int length, char* mem_pointer);

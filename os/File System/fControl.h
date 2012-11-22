/*
 * fControl.h
 *
 */

#define FREE_BLOCK -1
#define DIRECTORY 1
#define FILE 0
#define END_BLOCK -1
#define ROOT "/"

// Creates a File Control Block
int createFCB(int parentFCBID, char* name);

// Creates a file
int createFile(int fcBlockID, char* name);

/*
 * fControl.h
 *
 */

#define DIRECTORY 1
#define FILE 0
#define FREE -1

#define NEXT_BLOCK 1
#define END_BLOCK -1
#define ROOT "/"

// Creates a File Control Block
int createFCB(int parentFCBID, char* name);

// Creates a file
int createFile(int fcBlockID, char* name);

// Deletes a directory
int deleteDir(int fcBlockID, char* name);

// Deletes a file
int deleteFile(int fcBlockID, char* name);

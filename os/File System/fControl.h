/*
 * fControl.h
 *
 */

// Creates a File Control Block
int createFCB(int parentFCB, char* name);

// Creates a file
int createFile(int parentFCB, char* name);

// Get file type of a file from the file control block
int getType(int* type, int fcBlock, char* name);

// Get the starting block of the file path component from the file control block.
int getStart(int* blockId, int fcBlockID, char* name);

// Get file size of a file from the file control block
int getSize(int* size, int fcBlock, char* name);

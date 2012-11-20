/*
 * fControl.h
 *
 */

#define BLOCK_SIZE 512

// Get the starting block of the file path component from the file control block.
int getStart(int* blockId, int fcBlockID, char* name);

// Creates a File Control Block
int createFCB(int blockID, char* name);

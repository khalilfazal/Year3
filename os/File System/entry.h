/*
 * entry.h
 *
 */

#define BLOCK_START 0
#define ENTRY_START 1
#define ENTRY_LENGTH 9
#define ENTRY_END -1

#define TYPE_P 0
#define NAME_P 1
#define START_P 7
#define DATA_P 3

// Find a free block in the file system
int getFreeBlock(int* blockID);

// Adds an entry to a fcb
int addEntry(int* start, int fcBlockID, char* name, int type);

// Removes an entry from a fcb
int removeEntry(int* start, int fcBlockID, char* name);

// Get file type of a file from the file control block
int getType(int* type, int fcBlockID, char* name);

// Get the starting block of the file path component from the file control block.
int getStart(int* blockID, int fcBlockID, char* name);

// Get file size of a regular file
int getSize(int* size, int blockID);

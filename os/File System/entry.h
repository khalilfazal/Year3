/*
 * entry.h
 *
 */

#define START 3
#define ENTRY_START 7
#define ENTRY_LENGTH 9
#define ENTRY_END -1

// Adds an entry to a fcb
int addEntry(int* start, int* fcBlockID, char* name, int type);

// Get file type of a file from the file control block
int getType(int* type, int fcBlockID, char* name);

// Get the starting block of the file path component from the file control block.
int getStart(int* blockId, int fcBlockID, char* name);

// Get file size of a file from the file control block
int getSize(int* size, int fcBlockID, char* name);

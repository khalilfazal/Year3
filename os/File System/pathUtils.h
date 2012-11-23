/*
 * pathUtils.h
 *
 */

#define MAX_PATH 512
#define MAX_DIRNAME 7
#define ROOT_BLOCKID 0

// Parses a path string into an array of path components.
int parsePath(char** path, char* pathname);

// Traverse through the file system and retrieve the blockID of the last component.
int traverse(int* blockID, char** path);

// Strip last component from file name
int dirname(char** dir, char** path);

// Finds the length of an array terminated with a null char
int arrayLen(char** array);

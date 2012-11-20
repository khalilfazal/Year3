/*
 * pathUtils.h
 *
 */

#define MAX_PATH 512
#define MAX_DIRNAME 7
#define ROOT_BLOCK 0

#define LENGTH(x) sizeof(x)/sizeof(*x)

// Parses a path string into an array of path components.
int parsePath(char** path, char* pathname);

// Traverse through the file system and retrieve the blockID of the last component.
int traverse(int* blockID, char** path);

// Strip last component from file name
int dirname(char** dir, char** path);

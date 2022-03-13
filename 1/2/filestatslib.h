#ifndef FILESTATSLIB
#define FILESTATSLIB

#include <stdlib.h>
#include <stdio.h>

void createBlocks(int blocksCount);
void gatherStats(char** filePaths, int filesCount, char* tempFilePath);
int loadFileToMemory(char* filePath);
void freeBlock(int blockId);
long getFileSize(FILE* filePointer);
void printBlock(int blockId);
void freeAll();

#endif

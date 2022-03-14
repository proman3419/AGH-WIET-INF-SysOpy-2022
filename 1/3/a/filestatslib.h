#ifndef FILESTATSLIB
#define FILESTATSLIB

#include <stdlib.h>
#include <stdio.h>

void createBlocks(int blocksCount);
void gatherStats(char** filePaths, int filesCount, const char* tempFilePath);
void usewc(char* filePath, const char* tempFilePath);
int loadFileToMemory(const char* filePath);
void freeBlock(int blockId);
long getFileSize(FILE* filePointer);
void printBlock(int blockId);
void freeAllBlocks();

#endif

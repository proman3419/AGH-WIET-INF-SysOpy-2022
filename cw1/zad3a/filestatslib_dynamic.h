#ifndef FILESTATSLIB
#define FILESTATSLIB

#include <stdlib.h>
#include <stdio.h>

void createBlocksPlaceholders(int blocksCount);
void createBlocks(int blocksCount, int blockByteSize);
void gatherStats(char** filePaths, int filesCount, const char* tempFilePath);
void usewc(char* filePath, const char* tempFilePath);
int loadFileToMemory(const char* filePath);
void removeBlock(int blockId);
long getFileSize(FILE* filePointer);
void printBlock(int blockId);
void removeAllBlocks();

#endif

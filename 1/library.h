#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdlib.h>
#include <stdio.h>

void createResultsBlocks(int blocksCount);
void gatherStats(char** filePaths, int filesCount, char* tempFilePath);
int loadStatsToMemory(char* tempFilePath);
void removeBlock(int blockId);
long getFileSize(FILE* filePointer);

#endif

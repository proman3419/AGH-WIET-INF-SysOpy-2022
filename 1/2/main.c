#include "filestatslib.h"
#include <string.h>

int getFilePathsCount(int argc, char** argv, int i)
{
    int j = i;
    while (j < argc-1)
    {
        if (strcmp(argv[j+1], "create_table") == 0 ||
            strcmp(argv[j+1], "wc_files") == 0 ||
            strcmp(argv[j+1], "remove_block") == 0 ||
            strcmp(argv[j+1], "print_block") == 0)
            break;
        j++;
    }

    return j - i + 1;
}

void moveToArray(char** arrFrom, char** arrTo, int iFrom, int iTo)
{
    for (int i = iFrom; i <= iTo; i++)
        arrTo[i-iFrom] = arrFrom[i];
}

int main(int argc, char** argv)
{
    int i = 0;

    while (i < argc)
    {
        if (strcmp(argv[i], "create_table") == 0)
        {
            i++;
            int blocksCount = atoi(argv[i]);
            if (blocksCount >= 0)
                createBlocks(blocksCount);
        }
        else if (strcmp(argv[i], "wc_files") == 0)
        {
            i++;
            int filesCount = getFilePathsCount(argc, argv, i);
            char** filePaths = calloc(filesCount, sizeof(char *));
            moveToArray(argv, filePaths, i, i+filesCount-1);

            gatherStats(filePaths, filesCount, "temp.tmp");

            free(filePaths);
            i = i + filesCount - 1;
        }
        else if (strcmp(argv[i], "remove_block") == 0)
        {
            i++;
            int blockId = atoi(argv[i]);
            if (blockId >= 0)
                freeBlock(blockId);
        }
        else if (strcmp(argv[i], "print_block") == 0)
        {
            i++;
            int blockId = atoi(argv[i]);
            if (blockId >= 0)
                printBlock(blockId);
        }
        i++;
    }

    freeAll();

    return 0;
}

#include "filetypeocclib.h"
#include <stdio.h>

void printFileTypeOccuranceField(char* fileType, int occurances)
{
    printf("|%10s |%11d |\n", fileType, occurances);
}

void printFileTypeOccurance(struct FileTypeOccurance fto)
{
    int tableWidth = 26;
    printHorizontalLine('-', tableWidth);
    printf("|%10s |%11s |\n", "File type", "Occurances");
    printHorizontalLine('-', tableWidth);
    printFileTypeOccuranceField("sock", fto.sock);
    printFileTypeOccuranceField("slink", fto.slink);
    printFileTypeOccuranceField("file", fto.file);
    printFileTypeOccuranceField("block dev", fto.blockDev);
    printFileTypeOccuranceField("dir", fto.dir);
    printFileTypeOccuranceField("char dev", fto.charDev);
    printFileTypeOccuranceField("fifo", fto.fifo);
    printHorizontalLine('-', tableWidth);
    printf("\n");
}

void printHorizontalLine(char repr, int width)
{
    for (int i = 0; i < width; i++)
        printf("%c", repr);
    printf("\n");
}

#define _XOPEN_SOURCE 500
#include "filetypeocclib.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <string.h>
#include <time.h>
#include <ftw.h>

struct FileTypeOccurance FTO = {0};
char ROOT_DIR_ABSOLUTE_PATH[PATH_MAX];

int setRootDirAbsolutePath(char* rootDirPath)
{
    if (realpath(rootDirPath, ROOT_DIR_ABSOLUTE_PATH) == NULL)
    {
        printf("[ERROR] Couldn't find the absolute path of %s\n", rootDirPath);
        return -1;
    }
    return 0;
}

void printFileType(mode_t fileTypeFlag)
{
    switch (fileTypeFlag & S_IFMT)
    {
        case S_IFSOCK: printf("sock");      FTO.sock++;     return;
        case S_IFLNK:  printf("slink");     FTO.slink++;    return;
        case S_IFREG:  printf("file");      FTO.file++;     return;
        case S_IFBLK:  printf("block dev"); FTO.blockDev++; return;
        case S_IFDIR:  printf("dir");       FTO.dir++;      return;
        case S_IFCHR:  printf("char dev");  FTO.charDev++;  return;
        case S_IFIFO:  printf("fifo");      FTO.fifo++;     return;
    }
}

void printTimeReadable(time_t timet, char* timeStr)
{
    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", localtime(&timet));
    printf("%s", timeStr);
}

int printFileStats(const char* absoluteFilePath, const struct stat* fileStats, int fileFlags, struct FTW* ftwPtr)
{
    char timeStr[20];
    printf("Absolute file path: %s\n", absoluteFilePath);
    printf("Hard links: %ld\n", fileStats->st_nlink);
    printf("File type: "); printFileType(fileStats->st_mode); printf("\n");
    printf("File size: %ld[B]\n", fileStats->st_size);
    printf("Last access: "); printTimeReadable(fileStats->st_atime, timeStr); printf("\n");
    printf("Last modified: "); printTimeReadable(fileStats->st_mtime, timeStr); printf("\n");
    printf("\n");

    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2)
        return -1;

    char* rootDirPath = argv[1];

    if (setRootDirAbsolutePath(rootDirPath) == -1)
        return -1;

    printf("[INFO] The absolute path of the root directory is %s\n\n", ROOT_DIR_ABSOLUTE_PATH);

    nftw(ROOT_DIR_ABSOLUTE_PATH, printFileStats, 100, FTW_PHYS);
    printFileTypeOccurance(FTO);

    return 0;
}

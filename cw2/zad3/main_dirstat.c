#include "filetypeocclib.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

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

int printFileStats(char* absoluteFilePath)
{
    struct stat fileStats;
    if (lstat(absoluteFilePath, &fileStats) == -1)
    {
        printf("[ERROR] Couldn't read stats of the %s file\n\n", 
               absoluteFilePath);
        return -1;
    }

    char timeStr[20];
    printf("Absolute file path: %s\n", absoluteFilePath);
    printf("Hard links: %ld\n", fileStats.st_nlink);
    printf("File type: "); printFileType(fileStats.st_mode); printf("\n");
    printf("File size: %ld[B]\n", fileStats.st_size);
    printf("Last access: "); printTimeReadable(fileStats.st_atime, timeStr); printf("\n");
    printf("Last modified: "); printTimeReadable(fileStats.st_mtime, timeStr); printf("\n");
    printf("\n");

    return 0;
}

int exploreDir(char* absoluteDirPath)
{
    DIR* dir = opendir(absoluteDirPath);

    if (dir == NULL)
    {
        printf("[ERROR] Couldn't open the %s directory\n\n", absoluteDirPath);
        return -1;
    }

    struct dirent* dirEntPtr;
    char* absoluteFilePath = calloc(PATH_MAX, sizeof(char));
    while ((dirEntPtr = readdir(dir)) != NULL)
    {
        if (strcmp(dirEntPtr->d_name, ".") == 0 || 
            strcmp(dirEntPtr->d_name, "..") == 0)
            continue;

        snprintf(absoluteFilePath, PATH_MAX, "%s/%s", 
                 absoluteDirPath, dirEntPtr->d_name);
        printFileStats(absoluteFilePath);

        if (dirEntPtr->d_type == DT_DIR)
        {
            exploreDir(absoluteFilePath);
        }
    }

    free(absoluteFilePath);
    closedir(dir);

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
    exploreDir(ROOT_DIR_ABSOLUTE_PATH);
    printFileTypeOccurance(FTO);

    return 0;
}

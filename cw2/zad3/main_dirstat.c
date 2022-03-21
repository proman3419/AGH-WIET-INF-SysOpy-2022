#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <dirent.h>
#include <string.h>

int printAbsoluteFilePath(char* filePath)
{
    char absoluteFilePath [PATH_MAX+1];
    if (realpath(filePath, absoluteFilePath) == NULL)
    {
        printf("[ERROR] Couldn't find the absolute path of %s", filePath);
        return -1;
    }
    
    printf("%s", absoluteFilePath);
    return 0;
}

void printFileType(mode_t fileTypeFlag)
{
    switch (fileTypeFlag & S_IFMT)
    {
        case S_IFSOCK: printf("sock"); return;
        case S_IFLNK: printf("slink"); return;
        case S_IFREG: printf("file"); return;
        case S_IFBLK: printf("block dev"); return;
        case S_IFDIR: printf("dir"); return;
        case S_IFCHR: printf("char dev"); return;
        case S_IFIFO: printf("fifo"); return;
    }
}

int printFileStats(char* filePath)
{
    struct stat fileStats;
    if (lstat(filePath, &fileStats) == -1)
    {
        printf("[ERROR] Couldn't read stats of the %s file\n", filePath);
        return -1;
    }

    printf("\tAbsolute file path: "); printAbsoluteFilePath(filePath); printf("\n");
    printf("\tHard links: %ld\n", fileStats.st_nlink);
    printf("\tFile type: "); printFileType(fileStats.st_mode); printf("\n");
    printf("\tFile size: %ld[B]\n", fileStats.st_size);
    printf("\tLast access: %ld\n", fileStats.st_atime);
    printf("\tLast modified: %ld\n", fileStats.st_mtime);
    printf("\n");

    return 0;
}

int exploreDir(char* dirPath)
{
    DIR* dir = opendir(dirPath);

    if (dir == NULL)
    {
        printf("[ERROR] Couldn't open the %s directory\n", dirPath);
        return -1;
    }

    struct dirent* dirEntPtr;
    while ((dirEntPtr = readdir(dir)) != NULL)
    {
        if (strcmp(dirEntPtr->d_name, ".") == 0 || 
            strcmp(dirEntPtr->d_name, "..") == 0)
            continue;
        
        printf("[INFO] Processing %s\n", dirEntPtr->d_name);
        printFileStats(dirEntPtr->d_name);

        if (dirEntPtr->d_type == DT_DIR)
            exploreDir(dirEntPtr->d_name);
    }

    closedir(dir);

    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2)
        return -1;

    char* rootDirPath = argv[1];

    printFileStats(rootDirPath);
    exploreDir(rootDirPath);

    return 0;
}

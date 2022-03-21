#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <dirent.h>
#include <string.h>

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

    printf("\tAbsolute file path: %s\n", filePath);
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
    char* filePath = calloc(PATH_MAX, sizeof(char));
    while ((dirEntPtr = readdir(dir)) != NULL)
    {
        if (strcmp(dirEntPtr->d_name, ".") == 0 || 
            strcmp(dirEntPtr->d_name, "..") == 0)
            continue;

        snprintf(filePath, PATH_MAX, "%s/%s", dirPath, dirEntPtr->d_name);
        printf("[INFO] Processing %s\n", dirEntPtr->d_name);
        printFileStats(filePath);

        if (dirEntPtr->d_type == DT_DIR)
        {
            printf("[INFO] Entering the %s directory\n", filePath);
            exploreDir(filePath);
        }
    }

    free(filePath);
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

    printf("%s\n", ROOT_DIR_ABSOLUTE_PATH);

    printFileStats(ROOT_DIR_ABSOLUTE_PATH);
    exploreDir(ROOT_DIR_ABSOLUTE_PATH);

    return 0;
}

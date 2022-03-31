#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <linux/limits.h>
#include <unistd.h>

#define MAX_BUFFER_LEN 256

int checkIfTextFile(char* filePath)
{
    const char *dot = strrchr(filePath, '.');
    if (dot != NULL)
        if (strcmp(dot+1, "txt") == 0)
            return 1;
    return 0;
}

FILE* openFile(char* fileName, char* mode)
{
    FILE* fPtr = fopen(fileName, mode);
    if (fPtr == NULL)
    {
        printf("[ERROR] Couldn't open the file %s\n", fileName);
        exit(2);
    }

    return fPtr;
}

int checkIfFileContains(char* filePath, char* str)
{
    FILE* fPtr = openFile(filePath, "r");
    char* buffer = calloc(MAX_BUFFER_LEN, sizeof(char));
    int contains = 0;
    size_t bufferLen;

    while (getline(&buffer, &bufferLen, fPtr) != -1 && contains == 0)  
    {
        if (strstr(buffer, str) != NULL)
        {
            contains = 1;
            break;
        }
    }

    fclose(fPtr);
    free(buffer);

    return contains;
}

int exploreDir(char* dirPath, char* wantedStr, int maxDepth)
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

        if (dirEntPtr->d_type == DT_REG)
        {
            printf("[%d] Checking %s\n", getpid(), filePath);
            if (checkIfTextFile(filePath) == 1)
                printf("[%d] %s is a text file\n", getpid(), filePath);
            if (checkIfFileContains(filePath, wantedStr) == 1)
                printf("[%d] File %s contains %s\n", getpid(), filePath, wantedStr);
        }

        if (dirEntPtr->d_type == DT_DIR && maxDepth > 0)
        {
            if (fork() == 0) 
            {
                exploreDir(filePath, wantedStr, maxDepth-1);
                return 0;
            }
        }
    }

    free(filePath);
    closedir(dir);

    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        printf("[ERROR] Required parameters: rootDirPath, wantedStr, maxDepth\n");
        return 0;
    }

    char* rootDirPath = argv[1];
    char* wantedStr = argv[2];
    int maxDepth = atoi(argv[3]);

    exploreDir(rootDirPath, wantedStr, maxDepth);

    return 0;
}

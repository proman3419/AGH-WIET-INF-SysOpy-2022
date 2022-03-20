#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARG_LENGTH 256

int READ_INPUT_FROM_USER = 0;

int readInputFromargv(char** fileFromPath, char** fileToPath, int argc, char** argv)
{
    if (argc >= 3)
    {
        (*fileFromPath) = argv[1];
        (*fileToPath) = argv[2];
        return 1;
    }
    return 0;
}

void readArgFromUser(char** destVariable, char* message)
{
    (*destVariable) = calloc(MAX_ARG_LENGTH, sizeof(char));
    printf("%s: ", message);
    fgets((*destVariable), MAX_ARG_LENGTH, stdin);
    (*destVariable)[strcspn((*destVariable), "\n")] = 0;
}

void readInputFromUser(char** fileFromPath, char** fileToPath)
{
    readArgFromUser(fileFromPath, "Path of the file to copy from");
    readArgFromUser(fileToPath, "Path of the file to copy to");
    READ_INPUT_FROM_USER = 1;
}

void copyLine(FILE* fileFrom, FILE* fileTo, char* buffer, long int startLinePosition, long int endLinePosition)
{
    fseek(fileFrom, startLinePosition, SEEK_SET);
    for (int i = startLinePosition; i <= endLinePosition; ++i)
    {
        fread(buffer, 1, 1, fileFrom);
        fwrite(buffer, 1, 1, fileTo);
    }
}

void processLine(FILE* fileFrom, FILE* fileTo)
{
    char buffer[1];
    long int startLinePosition, endLinePosition;
    int nonWhiteCharFound = 0;

    startLinePosition = ftell(fileFrom);
    do
    {
        if (fread(buffer, 1, 1, fileFrom) != 1)
            break;
        if (isspace(buffer[0]) == 0)
            nonWhiteCharFound = 1;
    } while (buffer[0] != '\n');
    endLinePosition = ftell(fileFrom) - 1;

    if (nonWhiteCharFound == 1)
        copyLine(fileFrom, fileTo, buffer, startLinePosition, endLinePosition);
}

int copyFile(char* fileFromPath, char* fileToPath)
{
    FILE* fileFrom = fopen(fileFromPath, "r");
    FILE* fileTo = fopen(fileToPath, "w+");

    if (fileFrom == NULL)
    {
        printf("[ERROR] Couldn't open the file to copy from\n");
        return -1;
    }

    while (!feof(fileFrom))
        processLine(fileFrom, fileTo);

    printf("[SUCCESS] Copied %s to %s and removed non relevant lines\n", fileFromPath, fileToPath);

    return 0;
}

int main(int argc, char** argv)
{
    char* fileFromPath = NULL;
    char* fileToPath = NULL; 

    if (readInputFromargv(&fileFromPath, &fileToPath, argc, argv) == 0)
        readInputFromUser(&fileFromPath, &fileToPath);

    copyFile(fileFromPath, fileToPath);

    if (READ_INPUT_FROM_USER == 1)
    {
        free(fileFromPath);
        free(fileToPath);
    }

    return 0;
}

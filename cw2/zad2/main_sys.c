#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

struct Occurance
{
    int character;
    int line;
};

int processLine(int fileFD, char character, struct Occurance* occurances)
{
    char buffer[1];
    int characterOccured = 0;
    int readResult = 1;

    do
    {
        readResult = read(fileFD, buffer, 1);
        if (readResult == 0)
            break;
        if (buffer[0] == character)
        {
            occurances->character++;
            characterOccured = 1;
        }
    } while (buffer[0] != '\n');

    if (characterOccured == 1)
        occurances->line++;

    return readResult;
}

int countOccurances(char* filePath, char character, struct Occurance* occurances)
{
    int fileFD = open(filePath, O_RDONLY);

    if (fileFD == -1)
    {
        printf("[ERROR] Couldn't open the file\n");
        return -1;
    }

    while (processLine(fileFD, character, occurances) == 1) {}

    close(fileFD);

    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 3)
        return -1;

    char character = argv[1][0];
    char* filePath = argv[2];

    struct Occurance occurances = {0};
    if (countOccurances(filePath, character, &occurances) == 0)
        printf("In the file %s the character %c appears %d times in %d lines\n", 
               filePath, character, occurances.character, occurances.line);

    return 0;
}

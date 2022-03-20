#include <stdio.h>

struct Occurance
{
    int character;
    int line;
};

int processLine(int fileFD, char character, struct Occurance* occurances)
{
    char buffer[1];
    int characterOccured = 0;

    do
    {
        if (read(fileFD, buffer, 1) != 1)
            break;
        if (buffer[0] == character)
        {
            occurances->character++;
            characterOccured = 1;
        }
    } while (buffer[0] != '\n');

    return characterOccured;
}

int countOccurances(char* filePath, char character, struct Occurance* occurances)
{
    int fileFD = open(filePath, "r");

    if (file == -1)
    {
        printf("[ERROR] Couldn't open the file\n");
        return -1;
    }

    while (!feof(fileFD))
        if (processLine(fileFD, character, occurances) == 1)
            occurances->line++;

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

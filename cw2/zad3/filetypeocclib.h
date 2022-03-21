#ifndef FILETYPEOCCLIB
#define FILETYPEOCCLIB

struct FileTypeOccurance
{
    int sock;
    int slink;
    int file;
    int blockDev;
    int dir;
    int charDev;
    int fifo;
};

void printFileTypeOccuranceField(char* fileType, int occurances);
void printFileTypeOccurance(struct FileTypeOccurance fto);
void printHorizontalLine(char repr, int width);

#endif

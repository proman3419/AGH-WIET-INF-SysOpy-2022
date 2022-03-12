#include "library.h"

int main(int argc, char** argv)
{
    createResultsBlocks(512);

    char* filePaths[2];
    filePaths[0] = "playground/a.txt";
    filePaths[1] = "playground/b.txt";

    gatherStats(filePaths, 2, "playground/temp.txt");

    return 0;
}

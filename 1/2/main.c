int main(int argc, char** argv)
{
    createBlocks(512);

    char* filePaths[2];
    filePaths[0] = "playground/a.txt";
    filePaths[1] = "playground/b.txt";

    gatherStats(filePaths, 2, "playground/temp.tmp");
    printBlock(0);
    printBlock(1);

    freeBlock(0);
    printBlock(0);

    gatherStats(filePaths, 2, "playground/temp.tmp");
    printBlock(0);
    printBlock(1);
    printBlock(2);

    freeAll();

    return 0;
}

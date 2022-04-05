#define SIGNAL SIGUSR1

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        printf("[ERROR] Required parameters: catcherPID, signalsCount, sendingMode\n");
        return -1;
    }

    

    return 0;
}

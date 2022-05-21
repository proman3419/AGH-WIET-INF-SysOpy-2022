#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define MAX_LINE_LEN 128
#define LOG_BUFF_SIZE 72
#define LOG_FILE_PATH "Times.txt"

int threadsCnt;
pthread_t* threads;
int* threadsIds;

int** img;
int** imgNegative;
int imgWidth, imgHeight;

enum DivisionMethod
{
    NUMBERS,
    BLOCK
};

enum DivisionMethod strToDivisionMethod(char* str)
{
    if (strcmp(str, "numbers") == 0) return NUMBERS;
    if (strcmp(str, "block") == 0) return BLOCK;
    return -1;
}

int** callocArray2DInt(int r, int c)
{
    int **A = (int **)malloc((unsigned int)r*sizeof(int *));
    for (int i = 0; i < r; i++)
        A[i] = (int *)calloc((unsigned int)c, sizeof(int));

    return A;
}

void freeArray2DInt(int **A, int n)
{
    for (int i = 0; i < n; i++)
        free(A[i]);
    free(A);
}

FILE* openFile(char* fileName, char* mode)
{
    FILE* fp = fopen(fileName, mode);
    if (fp == NULL)
    {
        printf("[ERROR] Couldn't open %s\n", fileName);
        exit(-1);
    }

    return fp;
}

void loadImage(char* imgPath)
{
    FILE* fp = openFile(imgPath, "r");
    char* buff = calloc(MAX_LINE_LEN, sizeof(char));

    // skip the "magic number"
    fgets(buff, MAX_LINE_LEN, fp);

    // load image's width, height
    fgets(buff, MAX_LINE_LEN, fp);
    sscanf(buff, "%d %d\n", &imgWidth, &imgHeight);

    img = callocArray2DInt(imgHeight, imgWidth);
    int px;
    for (int r = 0; r < imgHeight; ++r)
    {
        for (int c = 0; c < imgWidth; ++c)
        {
            fscanf(fp, "%d", &px);
            if (px < 0 || px > 255)
            {
                printf("[ERROR] Invalid pixel value: %d, the pixel is r: %d c: %d\n", px, r, c);
                exit(-1);
            }
            img[r][c] = px;
        }
    }

    fclose(fp);
}

void* numbersDivisionMethod(void* arg)
{
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);

    int id = *((int*)arg);

    int minPxVal = 256 / threadsCnt * id;
    int maxPxVal = (id != threadsCnt - 1) ? (256 / threadsCnt * (id + 1) ) : 256;

    int px;
    for (int r = 0; r < imgHeight; ++r)
    {
        for (int c = 0; c < imgWidth; ++c)
        {
            px = img[r][c];
            if (px >= minPxVal && px < maxPxVal)
                imgNegative[r][c] = 255 - px;
        }
    }

    gettimeofday(&endTime, NULL);
    size_t* t = malloc(sizeof(size_t));
    *t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + endTime.tv_usec - startTime.tv_usec;
    pthread_exit(t);
}

void* blockDivisionMethod(void* arg)
{
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);

    int id = *((int*)arg);

    int xMin = (id) * ceil(imgWidth/threadsCnt);
    int xMax = (id != threadsCnt - 1) ? 
               ((id+1) * ceil(imgWidth/threadsCnt)-1) : imgWidth - 1;

    int px;
    for (int r = 0; r < imgHeight; ++r)
    {
        for (int c = xMin; c <= xMax; ++c)
        {
            px = img[r][c];
            imgNegative[r][c] = 255 - px;
        }
    }

    gettimeofday(&endTime, NULL);
    size_t* t = malloc(sizeof(size_t));
    *t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + endTime.tv_usec - startTime.tv_usec;
    pthread_exit(t);
}

void runThreads(enum DivisionMethod divisionMethod)
{
    for(int i = 0; i < threadsCnt; ++i)
    {
        threadsIds[i] = i;
        if (divisionMethod == NUMBERS)
            pthread_create(&threads[i], NULL, &numbersDivisionMethod, &threadsIds[i]);
        else if (divisionMethod == BLOCK)
            pthread_create(&threads[i], NULL, &blockDivisionMethod, &threadsIds[i]);
    }
}

void printAndSaveToFile(char* msg, char* savePath, int fprintfNewline)
{
    printf("%s", msg);
    FILE* fp = openFile(savePath, "a");
    if (fprintfNewline == 0)
        fprintf(fp, "%s", msg);
    else
        fprintf(fp, "%s\n", msg);
    fclose(fp);
}

void saveImg(int** img, char* path)
{
    FILE *fp = openFile(path, "w");

    fprintf(fp, "P2\n");
    fprintf(fp, "%d %d\n", imgWidth, imgHeight);
    fprintf(fp, "255\n");

    for (int r = 0; r < imgHeight; ++r)
    {
        for (int c = 0; c < imgWidth; ++c)
            fprintf(fp, "%d ", imgNegative[r][c]);
        fprintf(fp, "\n");
    }

    fclose(fp);
}

void cleanup()
{
    freeArray2DInt(img, imgHeight);
    freeArray2DInt(imgNegative, imgHeight);
    free(threads);
    free(threadsIds);
}

int main(int argc, char** argv)
{
    if (argc < 5)
    {
        printf("[ERROR] Required parameters: threadsCnt, divisionMethod, imgPath, outPath\n");
        exit(-1);
    }

    threadsCnt = atoi(argv[1]);
    enum DivisionMethod divisionMethod = strToDivisionMethod(argv[2]);
    char* divisionMethodStr = argv[2];
    char* imgPath = argv[3];
    char* outPath = argv[4];

    if (divisionMethod == -1)
    {
        printf("[ERROR] Incorrect value for divisionMethod, allowed values: numbers/block\n");
        exit(-1);
    }

    loadImage(imgPath);
    imgNegative = callocArray2DInt(imgHeight, imgWidth);
    threads = calloc(threadsCnt, sizeof(pthread_t));
    threadsIds = calloc(threadsCnt, sizeof(int));

    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);

    runThreads(divisionMethod);

    char buff[LOG_BUFF_SIZE];
    snprintf(buff, LOG_BUFF_SIZE, "+======================================+\n");
    printAndSaveToFile(buff, LOG_FILE_PATH, 0);
    snprintf(buff, LOG_BUFF_SIZE, "| Image path:       %s\n", imgPath);
    printAndSaveToFile(buff, LOG_FILE_PATH, 0);
    snprintf(buff, LOG_BUFF_SIZE, "| Image width:      %d\n", imgWidth);
    printAndSaveToFile(buff, LOG_FILE_PATH, 0);
    snprintf(buff, LOG_BUFF_SIZE, "| Image height:     %d\n", imgHeight);
    printAndSaveToFile(buff, LOG_FILE_PATH, 0);
    snprintf(buff, LOG_BUFF_SIZE, "| Threads count:    %d\n", threadsCnt);
    printAndSaveToFile(buff, LOG_FILE_PATH, 0);
    snprintf(buff, LOG_BUFF_SIZE, "| Division method:  %s\n", divisionMethodStr);
    printAndSaveToFile(buff, LOG_FILE_PATH, 0);
    snprintf(buff, LOG_BUFF_SIZE, "+======================================+\n");
    printAndSaveToFile(buff, LOG_FILE_PATH, 0);

    size_t* t;
    for (int i = 0; i < threadsCnt; ++i)
    {
        pthread_join(threads[i], (void **)&t);
        snprintf(buff, LOG_BUFF_SIZE, "> Thread: %3d\ttime: %4lu[μs]\n", i, *t);
        printAndSaveToFile(buff, LOG_FILE_PATH, 0);
    }

    gettimeofday(&endTime, NULL);
    t = malloc(sizeof(size_t));
    *t = (endTime.tv_sec - startTime.tv_sec) * 1000000 + endTime.tv_usec - startTime.tv_usec;
    snprintf(buff, LOG_BUFF_SIZE, "+======================================+\n");
    printAndSaveToFile(buff, LOG_FILE_PATH, 0);
    snprintf(buff, LOG_BUFF_SIZE, "| Total time: %4lu[μs]\n", *t);
    printAndSaveToFile(buff, LOG_FILE_PATH, 0);
    snprintf(buff, LOG_BUFF_SIZE, "+======================================+\n");
    printAndSaveToFile(buff, LOG_FILE_PATH, 1);

    saveImg(imgNegative, outPath);

    cleanup();

    return 0;
}

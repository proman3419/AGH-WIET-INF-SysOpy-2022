#ifndef CLIENT
#define CLIENT

void clean();
void sigintHandler(int sigNum);
void sigintHandlerSetup();
void setup();
void init();
void stopHandler();
void listHandler();
void tallHandler(char* msg);
void toneHandler(size_t cidTo, char* msg);
void sender();
void listener();
int main(int argc, char** argv);

#endif

#ifndef CLIENT
#define CLIENT

void clean();
void setup();
void init();
// void stopHandler();
void listHandler();
void tallHandler(char* msg);
void toneHandler(int cidTo, char* msg);
void sender();
void listener();
int main(int argc, char** argv);

#endif

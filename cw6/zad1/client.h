#ifndef CLIENT
#define CLIENT

void clean();
void setup();
void stopHandler();
void listHandler();
void tallHandler(char* input);
void toneHandler(int cidTo, char* msg);
void init();
int main(int argc, char** argv);

#endif

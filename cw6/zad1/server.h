#ifndef SERVER
#define SERVER

void clean();
void sigintHandler(int sigNum);
void sigintHandlerSetup();
void setup();
void stopHandler(struct MsgBuf received);
void listHandler(struct MsgBuf received);
void tallHandler(struct MsgBuf received);
void toneHandler(struct MsgBuf received);
void initHandler(struct MsgBuf received);
void logToFile();
int main(int argc, char** argv);

#endif

#ifndef SERVER
#define SERVER

void clean();
void setup();
void stopHandler(struct MsgBuf received);
void listHandler(struct MsgBuf received);
void tallHandler(struct MsgBuf received);
void toneHandler(struct MsgBuf received);
void initHandler(struct MsgBuf received);
int main(int argc, char** argv);

#endif

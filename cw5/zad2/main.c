#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define MAX_COMMAND_LENGTH 1024

void listMails(char* orderBy)
{
    FILE* mail;
    if (strcmp(orderBy, "author") == 0)
        mail = popen("mail | tail -n +3 | head -n -1 | sort -k3", "w");
    else if (strcmp(orderBy, "date") == 0)
        mail = popen("mail | tail -n +3 | head -n -1 | sort -dr", "w");
    else
    {
        printf("[ERROR] Ordering type not recognized; supported orderings: author, date\n");
        exit(1);
    }
    pclose(mail);
}

void sendMail(char* email, char* subject, char* content)
{
    char command[MAX_COMMAND_LENGTH];
    int commandLength = snprintf(command, MAX_COMMAND_LENGTH,
                                 "echo \"%s\" | mail -s \"%s\" %s", 
                                 content, subject, email);

    if (commandLength < 0)
        printf("[ERROR] Encoding error occurred for the command\n");
    else if (commandLength > MAX_COMMAND_LENGTH)
        printf("[ERROR] The command is too long\n");
    else
    {
        FILE* mail = popen(command, "r");
        if (mail == NULL)
            printf("[ERROR] Couldn't send the mail\n");
        else
            pclose(mail);
    }
}

int main(int argc, char** argv)
{
    if (!(argc == 2 || argc == 4))
    {
        printf("[ERROR] Required parameters: author/date OR email, subject, content\n");
        return -1;
    }

    if (argc == 2)
        listMails(argv[1]);
    if (argc == 4)
        sendMail(argv[1], argv[2], argv[3]);

    return 0;
}

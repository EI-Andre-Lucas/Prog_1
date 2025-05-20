#ifndef LOGS_H
#define LOGS_H
typedef struct logs
{
    int id;
    char mensagem[300];
}LOGS;

void logmessage(char mensagem[300], USERS user);

int getLastLogNumber();

#endif


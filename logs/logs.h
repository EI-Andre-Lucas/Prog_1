#ifndef LOGS_H
#define LOGS_H
typedef struct logs
{
    int id;
    char mensagem[300];
    char username[50];
    char data[100];
}LOGS;

void logmessage(char mensagem[300], USERS user);

int getLastLogNumber();

void mostrarTodosLogs();
void mostrarLogsPorPeriodo(time_t inicio, time_t fim);
void mostrarLogsPorUser(const char* username);
void mostrarLogsPorTipoAcao(int tipo_acao);

#endif


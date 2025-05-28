#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../users/users.h"
#include "logs.h"
#include "../utils/utils.h"

#define MAX_LOGS 1000

static Log logs[MAX_LOGS];
static int num_logs = 0;

void registrarLog(const char* username, const char* acao) {
    if (num_logs >= MAX_LOGS) return;

    Log* novo_log = &logs[num_logs];
    strncpy(novo_log->username, username, sizeof(novo_log->username) - 1);
    novo_log->username[sizeof(novo_log->username) - 1] = '\0';
    
    strncpy(novo_log->acao, acao, sizeof(novo_log->acao) - 1);
    novo_log->acao[sizeof(novo_log->acao) - 1] = '\0';
    
    novo_log->data_hora = time(NULL);
    num_logs++;
}

void mostrarLogs() {
    printf("\n=== Logs do Sistema ===\n");
    for (int i = 0; i < num_logs; i++) {
        printf("Data/Hora: %s", ctime(&logs[i].data_hora));
        printf("Usuário: %s\n", logs[i].username);
        printf("Ação: %s\n\n", logs[i].acao);
    }
}

void mostrarLogsPorUsuario(const char* username) {
    printf("\n=== Logs do Usuário %s ===\n", username);
    for (int i = 0; i < num_logs; i++) {
        if (strcmp(logs[i].username, username) == 0) {
            printf("Data/Hora: %s", ctime(&logs[i].data_hora));
            printf("Ação: %s\n\n", logs[i].acao);
        }
    }
}

void mostrarLogsPorPeriodo(time_t inicio, time_t fim) {
    printf("\n=== Logs por Período ===\n");
    for (int i = 0; i < num_logs; i++) {
        if (logs[i].data_hora >= inicio && logs[i].data_hora <= fim) {
            printf("Data/Hora: %s", ctime(&logs[i].data_hora));
            printf("Usuário: %s\n", logs[i].username);
            printf("Ação: %s\n\n", logs[i].acao);
        }
    }
}

void salvarLogs(const char* arquivo) {
    FILE* file = fopen(arquivo, "wb");
    if (file == NULL) return;

    fwrite(&num_logs, sizeof(int), 1, file);
    fwrite(logs, sizeof(Log), num_logs, file);

    fclose(file);
}

void carregarLogs(const char* arquivo) {
    FILE* file = fopen(arquivo, "rb");
    if (file == NULL) return;

    fread(&num_logs, sizeof(int), 1, file);
    fread(logs, sizeof(Log), num_logs, file);

    fclose(file);
}
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
    if (num_logs >= MAX_LOGS) {
        printf("Aviso: Número máximo de logs atingido!\n");
        return;
    }

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
        printf("Utilizador: %s\n", logs[i].username);
        printf("Ação: %s\n\n", logs[i].acao);
    }

    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        registrarLog(current_user->username, "Visualizou todos os logs do sistema");
    }
}

void mostrarLogsPorUtilizador(const char* username) {
    printf("\n=== Logs do Utilizador %s ===\n", username);
    bool encontrou = false;
    for (int i = 0; i < num_logs; i++) {
        if (strcmp(logs[i].username, username) == 0) {
            printf("Data/Hora: %s", ctime(&logs[i].data_hora));
            printf("Ação: %s\n\n", logs[i].acao);
            encontrou = true;
        }
    }
    if (!encontrou) {
        printf("Nenhum log encontrado para este utilizador.\n");
    }

    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        char log_message[150];
        snprintf(log_message, sizeof(log_message), "Visualizou os logs do utilizador %s", username);
        registrarLog(current_user->username, log_message);
    }
}

void mostrarLogsPorPeriodo(time_t inicio, time_t fim) {
    printf("\n=== Logs por Período ===\n");
    bool encontrou = false;
    for (int i = 0; i < num_logs; i++) {
        if (logs[i].data_hora >= inicio && logs[i].data_hora <= fim) {
            printf("Data/Hora: %s", ctime(&logs[i].data_hora));
            printf("Utilizador: %s\n", logs[i].username);
            printf("Ação: %s\n\n", logs[i].acao);
            encontrou = true;
        }
    }
    if (!encontrou) {
        printf("Nenhum log encontrado neste período.\n");
    }

    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        char log_message[150];
        snprintf(log_message, sizeof(log_message), "Visualizou logs do período %s a %s", 
                ctime(&inicio), ctime(&fim));
        registrarLog(current_user->username, log_message);
    }
}

void salvarLogs(const char* arquivo) {
    FILE* file = fopen(arquivo, "w");
    if (file == NULL) {
        printf("Erro ao abrir arquivo para salvar logs!\n");
        registrarLog("SISTEMA", "Erro ao salvar logs - arquivo não encontrado");
        return;
    }

    fprintf(file, "%d\n", num_logs);
    for (int i = 0; i < num_logs; i++) {
        fprintf(file, "%s|%s|%ld\n", 
                logs[i].username,
                logs[i].acao,
                logs[i].data_hora);
    }

    fclose(file);
    printf("Logs salvos com sucesso no arquivo %s!\n", arquivo);

    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        char log_message[150];
        snprintf(log_message, sizeof(log_message), "Salvou os logs no arquivo %s", arquivo);
        registrarLog(current_user->username, log_message);
    }
}

void carregarLogs(const char* arquivo) {
    FILE* file = fopen(arquivo, "r");
    if (file == NULL) {
        registrarLog("SISTEMA", "Erro ao carregar logs - arquivo não encontrado");
        return;
    }

    char linha[1024];
    if (fgets(linha, sizeof(linha), file) != NULL) {
        num_logs = atoi(linha);
    }

    for (int i = 0; i < num_logs && fgets(linha, sizeof(linha), file) != NULL; i++) {
        char* username = strtok(linha, "|");
        char* acao = strtok(NULL, "|");
        char* data_str = strtok(NULL, "\n");

        if (username && acao && data_str) {
            strncpy(logs[i].username, username, sizeof(logs[i].username) - 1);
            logs[i].username[sizeof(logs[i].username) - 1] = '\0';
            
            strncpy(logs[i].acao, acao, sizeof(logs[i].acao) - 1);
            logs[i].acao[sizeof(logs[i].acao) - 1] = '\0';
            
            logs[i].data_hora = atol(data_str);
        }
    }

    fclose(file);
    registrarLog("SISTEMA", "Logs carregados com sucesso");
}
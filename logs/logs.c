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

void registarLog(const char* username, const char* acao) {
    if (num_logs >= MAX_LOGS) {
        printf("Número máximo de logs atingido!\n");
        return;
    }

    Log novo_log;
    novo_log.id = num_logs + 1;  // Atribuir ID sequencial
    strncpy(novo_log.username, username, sizeof(novo_log.username) - 1);
    novo_log.username[sizeof(novo_log.username) - 1] = '\0';
    
    strncpy(novo_log.acao, acao, sizeof(novo_log.acao) - 1);
    novo_log.acao[sizeof(novo_log.acao) - 1] = '\0';
    
    novo_log.data_hora = time(NULL);

    logs[num_logs++] = novo_log;
}

void mostrarLogs() {
    printf("\n=== Logs do Sistema ===\n");
    for (int i = 0; i < num_logs; i++) {
        printf("Data/Hora: %s", ctime(&logs[i].data_hora));
        printf("User: %s\n", logs[i].username);
        printf("Ação: %s\n\n", logs[i].acao);
    }

    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        registarLog(current_user->username, "Visualizou todos os logs do sistema");
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
        registarLog(current_user->username, log_message);
    }
}

void mostrarLogsPorPeriodo(time_t inicio, time_t fim) {
    printf("\n=== Logs por Período ===\n");
    bool encontrou = false;
    for (int i = 0; i < num_logs; i++) {
        if (logs[i].data_hora >= inicio && logs[i].data_hora <= fim) {
            printf("Data/Hora: %s", ctime(&logs[i].data_hora));
            printf("User: %s\n", logs[i].username);
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
        snprintf(log_message, sizeof(log_message), "Visualizou os logs do período %s a %s", 
                ctime(&inicio), ctime(&fim));
        registarLog(current_user->username, log_message);
    }
}

void guardarLogs(const char* ficheiro) {
    FILE* file = fopen(ficheiro, "w");
    if (file == NULL) {
        printf("Erro ao abrir ficheiro de logs!\n");
        registarLog("SISTEMA", "Falha ao abrir ficheiro de logs - erro de permissão");
        return;
    }

    for (int i = 0; i < num_logs; i++) {
        Log* log = &logs[i];
        char data_str[32];
        struct tm* tm = localtime(&log->data_hora);
        strftime(data_str, sizeof(data_str), "%Y-%m-%d %H:%M:%S", tm);
        
        fprintf(file, "%d|%s|%s|%s\n",
            log->id,
            log->username,
            log->acao,
            data_str);
    }

    fclose(file);
    registarLog("SISTEMA", "Logs guardados com sucesso");
}

void carregarLogs(const char* ficheiro) {
    FILE* file = fopen(ficheiro, "r");
    if (file == NULL) {
        printf("Erro ao abrir ficheiro de logs!\n");
        registarLog("SISTEMA", "Falha ao abrir ficheiro de logs - erro de permissão");
        return;
    }

    char linha[256];
    num_logs = 0;

    while (fgets(linha, sizeof(linha), file) && num_logs < MAX_LOGS) {
        Log* log = &logs[num_logs];
        char data_str[32];
        
        if (sscanf(linha, "%d|%[^|]|%[^|]|%[^\n]", 
            &log->id,
            log->username,
            log->acao,
            data_str) == 4) {
            
            struct tm tm = {0};  // Inicializar todos os campos com 0
            int ano, mes, dia, hora, min, seg;
            
            if (sscanf(data_str, "%d-%d-%d %d:%d:%d", 
                &ano, &mes, &dia, &hora, &min, &seg) == 6) {
                
                tm.tm_year = ano - 1900;  // Ano desde 1900
                tm.tm_mon = mes - 1;      // Mês 0-11
                tm.tm_mday = dia;
                tm.tm_hour = hora;
                tm.tm_min = min;
                tm.tm_sec = seg;
                
                log->data_hora = mktime(&tm);
                num_logs++;
            }
        }
    }

    fclose(file);
    registarLog("SISTEMA", "Logs carregados com sucesso");
}
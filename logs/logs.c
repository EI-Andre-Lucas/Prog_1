#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../users/users.h"
#include "logs.h"

void getCurrentDateTime(char *datetime) {
    time_t t;
    struct tm *tm_info;
    time(&t);
    tm_info = localtime(&t);
    strftime(datetime, 100, "%Y-%m-%d %H:%M:%S", tm_info);
}

int getLastLogNumber() {
    FILE *file = fopen("logs.txt", "r");
    if (!file) return 0; // se não existir, começa em 0

    char line[300];
    int lastNumber = 0;
    char username[100], message[300], date[100];
    int number;

    while (fgets(line, sizeof(line), file)) {
        // Tenta ler uma linha no formato "Log %i | %s | - Username: %s | Data: %s"
        if (sscanf(line, "Log %i | %[^|] | - Username: %[^|] | Data: %s", 
                  &number, message, username, date) == 4) {
            if (number > lastNumber) {
                lastNumber = number;
            }
        }
    }

    fclose(file);
    return lastNumber;
}

void logmessage(char mensagem[300], USERS user) {
    LOGS log;
    FILE *fp = fopen("logs.txt", "a+");
    if (fp == NULL) {
        printf("\nErro ao abrir o ficheiro!");
        return;
    }
    
    strcpy(log.mensagem, mensagem);
    char date[100];
    getCurrentDateTime(date);
    log.id = getLastLogNumber() + 1;
    
    fprintf(fp, "Log %i | %s | - Username: %s | Data: %s\n", 
            log.id, log.mensagem, user.username, date);
    
    fclose(fp);
}

void mostrarTodosLogs() {
    FILE *file = fopen("logs.txt", "r");
    if (!file) {
        printf("Erro ao abrir o arquivo de logs!\n");
        return;
    }

    printf("\n=== Todos os Logs do Sistema ===\n");
    char line[500];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

void mostrarLogsPorPeriodo(time_t inicio, time_t fim) {
    FILE *file = fopen("logs.txt", "r");
    if (!file) {
        printf("Erro ao abrir o arquivo de logs!\n");
        return;
    }

    printf("\n=== Logs do Período ===\n");
    char line[500];
    char date_str[100];
    struct tm tm_date;
    time_t log_time;

    while (fgets(line, sizeof(line), file)) {
        // Extrair a data do log
        char *date_start = strstr(line, "Data: ");
        if (date_start) {
            date_start += 6; // Pular "Data: "
            strncpy(date_str, date_start, 19); // Pegar YYYY-MM-DD HH:MM:SS
            date_str[19] = '\0';
            
            // Converter string para time_t
            strptime(date_str, "%Y-%m-%d %H:%M:%S", &tm_date);
            log_time = mktime(&tm_date);
            
            // Verificar se está no período
            if (log_time >= inicio && log_time <= fim) {
                printf("%s", line);
            }
        }
    }
    fclose(file);
}

void mostrarLogsPorUser(const char* username) {
    FILE *file = fopen("logs.txt", "r");
    if (!file) {
        printf("Erro ao abrir o arquivo de logs!\n");
        return;
    }

    printf("\n=== Logs do Usuário %s ===\n", username);
    char line[500];
    char log_username[50];

    while (fgets(line, sizeof(line), file)) {
        // Extrair o username do log
        char *user_start = strstr(line, "Username: ");
        if (user_start) {
            user_start += 10; // Pular "Username: "
            sscanf(user_start, "%[^|]", log_username);
            
            // Verificar se é o usuário procurado
            if (strcmp(log_username, username) == 0) {
                printf("%s", line);
            }
        }
    }
    fclose(file);
}

void mostrarLogsPorTipoAcao(int tipo_acao) {
    FILE *file = fopen("logs.txt", "r");
    if (!file) {
        printf("Erro ao abrir o arquivo de logs!\n");
        return;
    }

    const char *tipos[] = {
        "Login",
        "Logout",
        "Criação",
        "Modificação",
        "Remoção"
    };

    if (tipo_acao < 1 || tipo_acao > 5) {
        printf("Tipo de ação inválido!\n");
        fclose(file);
        return;
    }

    printf("\n=== Logs de %s ===\n", tipos[tipo_acao - 1]);
    char line[500];
    char mensagem[300];

    while (fgets(line, sizeof(line), file)) {
        // Extrair a mensagem do log
        char *msg_start = strstr(line, "| ");
        if (msg_start) {
            msg_start += 2; // Pular "| "
            sscanf(msg_start, "%[^|]", mensagem);
            
            // Verificar se a mensagem contém o tipo de ação
            if (strstr(mensagem, tipos[tipo_acao - 1])) {
                printf("%s", line);
            }
        }
    }
    fclose(file);
}
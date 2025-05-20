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
#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../utils/utils.h"

typedef struct {
    int id;                 // ID único do log
    char username[50];      // Nome do utilizador
    char acao[100];         // Ação realizada
    time_t data_hora;       // Data e hora da ação
} Log;


void registarLog(const char* username, const char* acao);
void mostrarLogs();
void mostrarLogsPorUtilizador(const char* username);
void mostrarLogsPorPeriodo(time_t inicio, time_t fim);
void guardarLogs(const char* ficheiro);
void carregarLogs(const char* ficheiro);

#endif


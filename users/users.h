#ifndef USERS_H
#define USERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../utils/utils.h"

typedef enum {
    TECNICO = 0,
    ADMINISTRADOR = 1
} TipoUser;

typedef struct users {
    int id;
    char username[50];
    char password[50];
    char primeiroNome[50];
    char ultimoNome[50];
    TipoUser tipoUser;
} USERS;

void FirstUserCreator();
void login();
void registo();
void logout();
void menu_login_reg(int opcao);
bool guardarSessao(USERS *user);
USERS* verificarSessaoAtiva();

#endif

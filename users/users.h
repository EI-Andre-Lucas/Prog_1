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
bool login();
void registo(bool criar_admin);
void logout();
void menu_login_reg(int opcao);
bool guardarSessao(USERS *user);
USERS* verificarSessaoAtiva();

// Funções de gestão de utilizadores
void listarUtilizadores();
void removerUtilizador(const char* username);
void modificarTipoUtilizador(const char* username, int novo_tipo);
void alterarPasswordUtilizador(const char* username, const char* nova_senha);

#endif

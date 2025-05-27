#ifndef USERS_H
#define USERS_H

#include <stdbool.h>

typedef enum {
    ADMINISTRADOR,
    TECNICO
} UserType;

typedef struct users{
    UserType tipoUser;
    char email[100];
    char username[50];
    char PrimeiroNome[100];
    char UltimoNome[100];
    char password[100];
}USERS;

// Funções de autenticação
unsigned long hash_string(const char *str);
void logout();
bool guardarSessao(USERS *user);
void FirstUserCreator();
void registo();
void login();
USERS getsessao();

// Funções de gestão de users
void fornecerTodosUsers();
bool adicionarUser(const char* username, const char* password, int tipo);
bool removerUser(const char* username);
bool modificarUser(const char* username, const char* nova_password);
bool alterarRoleUser(const char* username, int novo_tipo);

#endif

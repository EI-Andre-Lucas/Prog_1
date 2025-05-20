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

unsigned long hash_string(const char *str);
void logout();
bool guardarSessao(USERS *user);
void FirstUserCreator();
void registo();
void login();
USERS getsessao();

#endif

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <users/users.h>
#include <logs/logs.h>


typedef enum {
    ADMINISTRADOR,
    TECNICO
} UserType;

typedef struct users{
    UserType tipoUser;
    char nome[100];
    char password[100];

}USERS;

typedef struct elem{

};

int main(){
    setlocale(LC_ALL, "portuguese");
    printf("============================================================================================\n");
    printf("=========Bem vindo ao Sistema de Gestão de Ciberincidentes e Respostas de Segurança=========\n");
    printf("==Nesta aplicação irá usar um menu do qual tera que usar numeros para navegar entre menus.==\n");
    printf("============================================================================================\n");
    
}
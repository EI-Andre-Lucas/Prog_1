#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "users/users.h"
#include "logs/logs.h"
#include "menus/menus.h"

typedef struct elem{
    USERS users;
    struct Elem *proximo;
    struct Elem *anterior;
}ELEM;


int main(){
    setlocale(LC_ALL, "portuguese");
    FirstUserCreator();
    logout();
    printf("============================================================================================\n");
    printf("=========Bem vindo ao Sistema de Gestão de Ciberincidentes e Respostas de Segurança=========\n");
    printf("==Nesta aplicação irá usar um menu do qual tera que usar numeros para navegar entre menus.==\n");
    printf("============================================================================================\n");
    clickEnter();
    menu_login_reg();
}
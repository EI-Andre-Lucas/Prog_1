#include <stdio.h>
#include <stdlib.h>
#include "../users/users.h"

void clrscr() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


//Função para criar o buffer
void clickEnter() {
    printf("Clica ENTER para continuar...\n");
    while (getchar() != '\n');
    getchar();
    clrscr();
}



void menu_login_reg(){
    int escolha = 0;
    printf("\n1 - Fazer Login.");
    printf("\n2 - Registar-se no software");
    printf("\nEscolha: ");
    scanf("%i",&escolha);
    switch (escolha)
    {
    case 1:
        login();
    break;
    case 2:
        registo();
    break;
    
    default:
        printf("\nOpção invalida");
    break;
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "users/users.h"
#include "logs/logs.h"
#include "menus/menus.h"
#include "incidentes/incidentes.h"

typedef struct elem{
    USERS users;
    struct Elem *proximo;
    struct Elem *anterior;
}ELEM;

// Variável global para a lista de incidentes
Incidente* lista_incidentes = NULL;

int main(){
    setlocale(LC_ALL, "Portuguese");
    FirstUserCreator();
    logout();
    printf("============================================================================================\n");
    printf("=========Bem vindo ao Sistema de Gestão de Ciberincidentes e Respostas de Segurança=========\n");
    printf("==Nesta aplicação irá usar um menu do qual tera que usar numeros para navegar entre menus.==\n");
    printf("============================================================================================\n");
    clickEnter();

    // Carregar incidentes do arquivo
    lista_incidentes = carregarIncidentes("incidentes.bin");

    // Iniciar o menu de login/registro
    menuLoginRegistro();

    // Salvar incidentes antes de sair
    guardarIncidentes(lista_incidentes, "incidentes.bin");
    liberarListaIncidentes(lista_incidentes);

    return 0;
}
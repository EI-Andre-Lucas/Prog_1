#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "users/users.h"
#include "logs/logs.h"
#include "menus/menus.h"
#include "incidentes/incidentes.h"
#include "utils/utils.h"

typedef struct elem{
    Incidente incidentes;
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
    printf("==Nesta aplicação irá usar um menu do qual terá que usar números para navegar entre menus.==\n");
    printf("============================================================================================\n");
    clickEnter();

    // Carregar incidentes do ficheiro
    lista_incidentes = carregarIncidentes("incidentes.bin");
    
    // Carregar logs do ficheiro
    carregarLogs("logs.bin");

    // Iniciar o menu de login/registo
    menuLoginRegisto();

    // Salvar incidentes antes de sair
    guardarIncidentes(lista_incidentes, "incidentes.bin");
    
    // Salvar logs antes de sair
    salvarLogs("logs.bin");
    
    fornecerListaIncidentes(lista_incidentes);

    return 0;
}
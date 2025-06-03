#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>
#include "users/users.h"
#include "logs/logs.h"
#include "menus/menus.h"
#include "incidentes/incidentes.h"
#include "utils/utils.h"

// Variáveis globais
USERS* current_user = NULL;
ELEM* lista_incidentes = NULL;

int main(){
    setlocale(LC_ALL, "Portuguese");
    FirstUserCreator();
    printf("============================================================================================\n");
    printf("=========Bem vindo ao Sistema de Gestão de Ciberincidentes e Respostas de Segurança=========\n");
    printf("==Nesta aplicação irá usar um menu do qual terá que usar números para navegar entre menus.==\n");
    printf("============================================================================================\n");
    clickEnter();

    // Carregar incidentes do ficheiro
    lista_incidentes = carregarIncidentes("incidentes.bin");
    
    // Carregar logs do ficheiro
    carregarLogs("logs.bin");

    // Verificar se existe uma sessão ativa
    USERS* user = verificarSessaoAtiva();
    if (user != NULL) {
        // Se existe sessão, vai direto para o menu apropriado
        if (user->tipoUser == ADMINISTRADOR) {
            menuAdministrador();
        } else {
            menuTecnico();
        }
    } else {
        // Se não existe sessão, vai para o menu principal
        menuPrincipal();
    }

    // Salvar incidentes antes de sair
    guardarIncidentes(lista_incidentes, "incidentes.bin");
    
    // Salvar logs antes de sair
    guardarLogs("logs.bin");
    
    fornecerListaIncidentes(lista_incidentes);

    return 0;
}
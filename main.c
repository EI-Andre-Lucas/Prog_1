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
    printf("\nCarregando incidentes do arquivo...\n");
    lista_incidentes = carregarIncidentes("incidentes.bin");
    if (lista_incidentes) {
        printf("Incidentes carregados com sucesso!\n");
        ELEM* atual = lista_incidentes;
        int count = 0;
        while (atual) {
            count++;
            atual = atual->proximo;
        }
        printf("Total de incidentes carregados: %d\n", count);
    } else {
        printf("Nenhum incidente encontrado ou erro ao carregar.\n");
    }
    
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
    printf("\nSalvando incidentes...\n");
    guardarIncidentes(lista_incidentes, "incidentes.bin");
    printf("Incidentes salvos com sucesso!\n");
    
    // Salvar logs antes de sair
    guardarLogs("logs.bin");
    
    fornecerListaIncidentes(lista_incidentes);

    return 0;
}
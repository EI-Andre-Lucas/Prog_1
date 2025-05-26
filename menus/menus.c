#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "../users/users.h"
#include "menus.h"

extern Incidente* lista_incidentes;

void limparEcra() {
    system("cls");
}

void clicarEnter() {
    printf("\nClique ENTER para continuar...");
    getchar();
}

void menuLoginRegisto(){
    int opcao;
    do {
        limparEcra();
        printf("\n=== Menu de Login/Registo ===\n");
        printf("1. Login\n");
        printf("2. Registo\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1: {
                USERS user = login();
                if (user.id != -1) {
                    menuPrincipal(&user);
                }
                break;
            }
            case 2: {
                USERS user = registo();
                if (user.id != -1) {
                    menuPrincipal(&user);
                }
                break;
            }
        }
    } while (opcao != 0);
}

void menuPrincipal(USERS* user) {
    if (user->tipo == 1) { // Admin
        menuAdministrador(user);
    } else { // Técnico
        menuTecnico(user);
    }
}

void menuAdministrador(USERS* user) {
    int opcao;
    do {
        limparEcra();
        printf("\n=== Menu do Administrador ===\n");
        printf("1. Gestão de Incidentes\n");
        printf("2. Gestão de Users\n");
        printf("3. Logs do Sistema\n");
        printf("0. Logout\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                menuIncidentesAdmin(&lista_incidentes);
                break;
            case 2:
                // Implementar menu de gestão de usuários
                break;
            case 3:
                // Implementar visualização de logs
                break;
            case 0:
                logout();
                return;
        }
    } while (opcao != 0);
}

void menuTecnico(USERS* user) {
    int opcao;
    do {
        limparEcra();
        printf("\n=== Menu do Técnico ===\n");
        printf("1. Gestão de Incidentes\n");
        printf("2. Meus Incidentes\n");
        printf("3. Histórico de Resoluções\n");
        printf("0. Logout\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                menuIncidentesTecnico(&lista_incidentes, user->username);
                break;
            case 2:
                fornecerIncidentesPorTecnico(lista_incidentes, user->username);
                clicarEnter();
                break;
            case 3: {
                Incidente* atual = lista_incidentes;
                printf("\n=== Histórico de Incidentes Resolvidos ===\n");
                while (atual != NULL) {
                    if (strcmp(atual->tecnico_responsavel, user->username) == 0 && 
                        atual->estado == RESOLVIDO) {
                        mostrarDetalhesIncidente(atual);
                        printf("\n");
                    }
                    atual = atual->proximo;
                }
                clicarEnter();
                break;
            }
            case 0:
                logout();
                return;
        }
    } while (opcao != 0);
}

void menuIncidentesAdmin(Incidente** lista_incidentes) {
    int opcao;
    do {
        printf("\n=== Menu de Gestão de Incidentes (Admin) ===\n");
        printf("1. Adicionar novo incidente\n");
        printf("2. Remover incidente\n");
        printf("3. Fornecer todos os incidentes\n");
        printf("4. Filtrar por estado\n");
        printf("5. Filtrar por severidade\n");
        printf("6. Filtrar por tipo\n");
        printf("7. Filtrar por técnico\n");
        printf("8. Filtrar por período\n");
        printf("9. Criar relatório\n");
        printf("10. Ver tempo médio de resolução por técnico\n");
        printf("0. Voltar\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1: {
                int id, tipo, severidade;
                char descricao[500];
                
                printf("ID do incidente: ");
                scanf("%d", &id);
                getchar();
                
                printf("Tipo (0-Phishing, 1-Malware, 2-Acesso não autorizado, 3-Outro): ");
                scanf("%d", &tipo);
                getchar();
                
                printf("Descrição: ");
                fgets(descricao, sizeof(descricao), stdin);
                descricao[strcspn(descricao, "\n")] = 0;
                
                printf("Severidade (0-Baixa, 1-Média, 2-Alta, 3-Crítica): ");
                scanf("%d", &severidade);
                getchar();
                
                Incidente* novo = criarIncidente(id, tipo, descricao, severidade);
                if (novo) {
                    adicionarIncidente(lista_incidentes, novo);
                    printf("Incidente adicionado com sucesso!\n");
                }
                break;
            }
            case 2: {
                int id;
                printf("ID do incidente a remover: ");
                scanf("%d", &id);
                removerIncidente(lista_incidentes, id);
                printf("Incidente removido com sucesso!\n");
                break;
            }
            case 3:
                fornecerIncidentes(*lista_incidentes);
                break;
            case 4: {
                int estado;
                printf("Estado (0-Pendente, 1-Em análise, 2-Resolvido): ");
                scanf("%d", &estado);
                fornecerIncidentesPorEstado(*lista_incidentes, estado);
                break;
            }
            case 5: {
                int severidade;
                printf("Severidade (0-Baixa, 1-Média, 2-Alta, 3-Crítica): ");
                scanf("%d", &severidade);
                fornecerIncidentesPorSeveridade(*lista_incidentes, severidade);
                break;
            }
            case 6: {
                int tipo;
                printf("Tipo (0-Phishing, 1-Malware, 2-Acesso não autorizado, 3-Outro): ");
                scanf("%d", &tipo);
                fornecerIncidentesPorTipo(*lista_incidentes, tipo);
                break;
            }
            case 7: {
                char tecnico[50];
                printf("Nome do técnico: ");
                fgets(tecnico, sizeof(tecnico), stdin);
                tecnico[strcspn(tecnico, "\n")] = 0;
                fornecerIncidentesPorTecnico(*lista_incidentes, tecnico);
                break;
            }
            case 8: {
                time_t inicio, fim;
                struct tm tm_inicio, tm_fim;
                char data_inicio[20], data_fim[20];
                
                printf("Data inicial (DD/MM/AAAA): ");
                fgets(data_inicio, sizeof(data_inicio), stdin);
                data_inicio[strcspn(data_inicio, "\n")] = 0;
                
                printf("Data final (DD/MM/AAAA): ");
                fgets(data_fim, sizeof(data_fim), stdin);
                data_fim[strcspn(data_fim, "\n")] = 0;
                
                strptime(data_inicio, "%d/%m/%Y", &tm_inicio);
                strptime(data_fim, "%d/%m/%Y", &tm_fim);
                
                inicio = mktime(&tm_inicio);
                fim = mktime(&tm_fim);
                
                fornecerIncidentesPorPeriodo(*lista_incidentes, inicio, fim);
                break;
            }
            case 9: {
                char arquivo[100];
                time_t inicio, fim;
                struct tm tm_inicio, tm_fim;
                char data_inicio[20], data_fim[20];
                
                printf("Nome do arquivo de relatório: ");
                fgets(arquivo, sizeof(arquivo), stdin);
                arquivo[strcspn(arquivo, "\n")] = 0;
                
                printf("Data inicial (DD/MM/AAAA): ");
                fgets(data_inicio, sizeof(data_inicio), stdin);
                data_inicio[strcspn(data_inicio, "\n")] = 0;
                
                printf("Data final (DD/MM/AAAA): ");
                fgets(data_fim, sizeof(data_fim), stdin);
                data_fim[strcspn(data_fim, "\n")] = 0;
                
                strptime(data_inicio, "%d/%m/%Y", &tm_inicio);
                strptime(data_fim, "%d/%m/%Y", &tm_fim);
                
                inicio = mktime(&tm_inicio);
                fim = mktime(&tm_fim);
                
                criarRelatorio(*lista_incidentes, arquivo, inicio, fim);
                printf("Relatório criado com sucesso!\n");
                break;
            }
            case 10: {
                char tecnico[50];
                printf("Nome do técnico: ");
                fgets(tecnico, sizeof(tecnico), stdin);
                tecnico[strcspn(tecnico, "\n")] = 0;
                calcularTempoMedioResolucao(*lista_incidentes, tecnico);
                break;
            }
        }
    } while (opcao != 0);
}

void menuIncidentesTecnico(Incidente** lista_incidentes, const char* tecnico) {
    int opcao;
    do {
        printf("\n=== Menu de Gestão de Incidentes (Técnico) ===\n");
        printf("1. Ver incidentes atribuídos\n");
        printf("2. Atualizar estado de incidente\n");
        printf("3. Adicionar comentário\n");
        printf("4. Registar uso de ferramenta\n");
        printf("5. Designar incidente\n");
        printf("6. Ver histórico de incidentes resolvidos\n");
        printf("0. Voltar\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                FornecerIncidentesPorTecnico(*lista_incidentes, tecnico);
                break;
            case 2: {
                int id, estado;
                printf("ID do incidente: ");
                scanf("%d", &id);
                printf("Novo estado (0-Pendente, 1-Em análise, 2-Resolvido): ");
                scanf("%d", &estado);
                
                Incidente* incidente = procurarIncidente(*lista_incidentes, id);
                if (incidente && strcmp(incidente->tecnico_responsavel, tecnico) == 0) {
                    atualizarEstadoIncidente(incidente, estado);
                    printf("Estado atualizado com sucesso!\n");
                } else {
                    printf("Incidente não encontrado ou não está atribuído a este técnico.\n");
                }
                break;
            }
            case 3: {
                int id;
                char comentario[500];
                printf("ID do incidente: ");
                scanf("%d", &id);
                getchar();
                printf("Comentário: ");
                fgets(comentario, sizeof(comentario), stdin);
                comentario[strcspn(comentario, "\n")] = 0;
                
                Incidente* incidente = buscarIncidente(*lista_incidentes, id);
                if (incidente && strcmp(incidente->tecnico_responsavel, tecnico) == 0) {
                    adicionarAcaoHistorico(incidente, comentario, tecnico);
                    printf("Comentário adicionado com sucesso!\n");
                } else {
                    printf("Incidente não encontrado ou não está atribuído a este técnico.\n");
                }
                break;
            }
            case 4: {
                int id;
                char ferramenta[100];
                printf("ID do incidente: ");
                scanf("%d", &id);
                getchar();
                printf("Nome da ferramenta: ");
                fgets(ferramenta, sizeof(ferramenta), stdin);
                ferramenta[strcspn(ferramenta, "\n")] = 0;
                
                Incidente* incidente = procurarIncidente(*lista_incidentes, id);
                if (incidente && strcmp(incidente->tecnico_responsavel, tecnico) == 0) {
                    adicionarFerramenta(incidente, ferramenta);
                    printf("Ferramenta registada com sucesso!\n");
                } else {
                    printf("Incidente não encontrado ou não está atribuído a este técnico.\n");
                }
                break;
            }
            case 5: {
                int id;
                char novo_tecnico[50], motivo[500];
                printf("ID do incidente: ");
                scanf("%d", &id);
                getchar();
                printf("Novo técnico: ");
                fgets(novo_tecnico, sizeof(novo_tecnico), stdin);
                novo_tecnico[strcspn(novo_tecnico, "\n")] = 0;
                printf("Motivo da designação: ");
                fgets(motivo, sizeof(motivo), stdin);
                motivo[strcspn(motivo, "\n")] = 0;
                
                Incidente* incidente = procurarIncidente(*lista_incidentes, id);
                if (incidente && strcmp(incidente->tecnico_responsavel, tecnico) == 0) {
                    designarIncidente(incidente, novo_tecnico, motivo);
                    printf("Incidente designado com sucesso!\n");
                } else {
                    printf("Incidente não encontrado ou não está atribuído a este técnico.\n");
                }
                break;
            }
            case 6: {
                Incidente* atual = *lista_incidentes;
                printf("\n=== Histórico de Incidentes Resolvidos ===\n");
                while (atual != NULL) {
                    if (strcmp(atual->tecnico_responsavel, tecnico) == 0 && 
                        atual->estado == RESOLVIDO) {
                        mostrarDetalhesIncidente(atual);
                        printf("\n");
                    }
                    atual = atual->proximo;
                }
                break;
            }
        }
    } while (opcao != 0);
}

void fornecerIncidentes(Incidente* lista) {
    printf("\n=== Lista de Incidentes ===\n");
    Incidente* atual = lista;
    while (atual != NULL) {
        mostrarDetalhesIncidente(atual);
        printf("\n");
        atual = atual->proximo;
    }
}

void fornecerIncidentesPorEstado(Incidente* lista, EstadoIncidente estado) {
    printf("\n=== Incidentes por Estado ===\n");
    Incidente* atual = lista;
    while (atual != NULL) {
        if (atual->estado == estado) {
            mostrarDetalhesIncidente(atual);
            printf("\n");
        }
        atual = atual->proximo;
    }
}

void fornecerIncidentesPorSeveridade(Incidente* lista, Severidade severidade) {
    printf("\n=== Incidentes por Severidade ===\n");
    Incidente* atual = lista;
    while (atual != NULL) {
        if (atual->severidade == severidade) {
            mostrarDetalhesIncidente(atual);
            printf("\n");
        }
        atual = atual->proximo;
    }
}

void fornecerIncidentesPorTipo(Incidente* lista, TipoIncidente tipo) {
    printf("\n=== Incidentes por Tipo ===\n");
    Incidente* atual = lista;
    while (atual != NULL) {
        if (atual->tipo == tipo) {
            mostrarDetalhesIncidente(atual);
            printf("\n");
        }
        atual = atual->proximo;
    }
}

void fornecerIncidentesPorTecnico(Incidente* lista, const char* tecnico) {
    printf("\n=== Incidentes por Técnico ===\n");
    Incidente* atual = lista;
    while (atual != NULL) {
        if (strcmp(atual->tecnico_responsavel, tecnico) == 0) {
            mostrarDetalhesIncidente(atual);
            printf("\n");
        }
        atual = atual->proximo;
    }
}

void fornecerIncidentesPorPeriodo(Incidente* lista, time_t inicio, time_t fim) {
    printf("\n=== Incidentes por Período ===\n");
    Incidente* atual = lista;
    while (atual != NULL) {
        if (atual->data_hora >= inicio && atual->data_hora <= fim) {
            mostrarDetalhesIncidente(atual);
            printf("\n");
        }
        atual = atual->proximo;
    }
}

void mostrarDetalhesIncidente(Incidente* incidente) {
    printf("ID: %d\n", incidente->id);
    printf("Tipo: %d\n", incidente->tipo);
    printf("Data/Hora: %s", ctime(&incidente->data_hora));
    printf("Descrição: %s\n", incidente->descricao);
    printf("Severidade: %d\n", incidente->severidade);
    printf("Estado: %d\n", incidente->estado);
    printf("Técnico: %s\n", incidente->tecnico_responsavel);
    if (incidente->estado == RESOLVIDO) {
        printf("Tempo de Resolução: %ld segundos\n", incidente->tempo_real);
    }
}

void mostrarHistoricoIncidente(Incidente* incidente) {
    printf("\n=== Histórico do Incidente %d ===\n", incidente->id);
    for (int i = 0; i < incidente->num_acoes; i++) {
        printf("Data/Hora: %s", ctime(&incidente->historico[i].data_hora));
        printf("Técnico: %s\n", incidente->historico[i].tecnico);
        printf("Ação: %s\n\n", incidente->historico[i].descricao);
    }
}

void mostrarFerramentasUtilizadas(Incidente* incidente) {
    printf("\n=== Ferramentas Utilizadas no Incidente %d ===\n", incidente->id);
    for (int i = 0; i < incidente->num_ferramentas; i++) {
        printf("Ferramenta: %s\n", incidente->ferramentas[i].nome);
        printf("Data de Uso: %s", ctime(&incidente->ferramentas[i].data_uso));
    }
}

void calcularTempoMedioResolucao(Incidente* lista, const char* tecnico) {
    long total_tempo = 0;
    int num_incidentes = 0;
    
    Incidente* atual = lista;
    while (atual != NULL) {
        if (strcmp(atual->tecnico_responsavel, tecnico) == 0 && 
            atual->estado == RESOLVIDO) {
            total_tempo += atual->tempo_real;
            num_incidentes++;
        }
        atual = atual->proximo;
    }
    
    if (num_incidentes > 0) {
        double tempo_medio = (double)total_tempo / num_incidentes;
        printf("\nTempo médio de resolução para %s: %.2f segundos\n", 
               tecnico, tempo_medio);
    } else {
        printf("\nNenhum incidente resolvido encontrado para %s\n", tecnico);
    }
}

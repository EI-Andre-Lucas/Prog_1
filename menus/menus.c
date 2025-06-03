#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "../users/users.h"
#include "menus.h"
#include "../utils/utils.h"
#include "../logs/logs.h"

extern USERS* current_user;
extern ELEM* lista_incidentes;

// Função auxiliar para converter string de data para struct tm
void converterDataStringParaTM(const char* data_str, struct tm* tm_data) {
    int dia, mes, ano;
    sscanf(data_str, "%d/%d/%d", &dia, &mes, &ano);
    
    tm_data->tm_mday = dia;
    tm_data->tm_mon = mes - 1;  // Mês em C vai de 0 a 11
    tm_data->tm_year = ano - 1900;  // Ano em C é contado a partir de 1900
    tm_data->tm_hour = 0;
    tm_data->tm_min = 0;
    tm_data->tm_sec = 0;
    tm_data->tm_isdst = -1;  // Deixa o sistema determinar o DST
}

void menuIncidentesTecnico(ELEM** lista_incidentes, const char* tecnico) {
    int opcao;
    do {
        printf("\n=== Menu de Gestão de Incidentes (Técnico) ===\n");
        printf("1. Ver incidentes atribuídos\n");
        printf("2. Atualizar estado de incidente\n");
        printf("3. Adicionar comentário\n");
        printf("4. Registrar uso de ferramenta\n");
        printf("5. Delegar incidente\n");
        printf("6. Ver histórico de incidentes resolvidos\n");
        printf("7. Ordenar incidentes\n");
        printf("0. Voltar\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                fornecerIncidentesPorTecnico(*lista_incidentes, tecnico);
                break;
            case 2: {
                int id, estado;
                printf("ID do incidente: ");
                scanf("%d", &id);
                printf("Novo estado (0-Por tratar, 1-Em análise, 2-Resolvido): ");
                scanf("%d", &estado);
                
                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
                if (incidente && strcmp(incidente->incidente.tecnico_responsavel, tecnico) == 0) {
                    atualizarEstadoIncidente(incidente, estado);
                    printf("Estado atualizado com sucesso!\n");
                } else {
                    printf("O incidente não foi encontrado ou não lhe está atribuído.\n");
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
                
                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
                if (incidente && strcmp(incidente->incidente.tecnico_responsavel, tecnico) == 0) {
                    adicionarAcaoHistorico(incidente, comentario, tecnico);
                    printf("Comentário adicionado com sucesso!\n");
                } else {
                    printf("O incidente não foi encontrado ou não lhe está atribuído.\n");
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
                
                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
                if (incidente && strcmp(incidente->incidente.tecnico_responsavel, tecnico) == 0) {
                    adicionarFerramenta(incidente, ferramenta);
                    printf("Ferramenta registada com sucesso!\n");
                } else {
                    printf("O incidente não foi encontrado ou não lhe está atribuído..\n");
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
                printf("Motivo da delegação: ");
                fgets(motivo, sizeof(motivo), stdin);
                motivo[strcspn(motivo, "\n")] = 0;
                
                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
                if (incidente && strcmp(incidente->incidente.tecnico_responsavel, tecnico) == 0) {
                    designarIncidente(incidente, novo_tecnico, motivo);
                    printf("Incidente delegado com sucesso!\n");
                } else {
                    printf("O incidente não foi encontrado ou não lhe está atribuído.\n");
                }
                break;
            }
            case 6: {
                ELEM* atual = *lista_incidentes;
                printf("\n=== Histórico de Incidentes Resolvidos ===\n");
                while (atual != NULL) {
                    if (strcmp(atual->incidente.tecnico_responsavel, tecnico) == 0 && 
                        atual->incidente.estado == RESOLVIDO) {
                        mostrarDetalhesIncidente(atual);
                        printf("\n");
                    }
                    atual = atual->proximo;
                }
                break;
            }
            case 7:
                ordenarIncidentes(lista_incidentes);
                clickEnter();
                break;
            case 0:
                break;
            default:
                printf("\nOpção inválida!\n");
                clickEnter();
        }
    } while (opcao != 0);
}

void menuPrincipal() {
    int opcao;
    do {
        system("cls");
        printf("\n=== Menu Principal ===\n");
        printf("1. Login\n");
        printf("2. Registo\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1:
                if (login()) {
                    USERS* user = verificarSessaoAtiva();
                    if (user != NULL) {
                        if (user->tipoUser == ADMINISTRADOR) {
                            menuAdministrador();
                        } else {
                            menuTecnico();
                        }
                    }
                }
                break;
            case 2:
                registo(false); // Cria um novo técnico
                break;
            case 0:
                printf("A sair...\n");
                break;
            default:
                printf("Opção inválida!\n");
                clickEnter();
        }
    } while (opcao != 0);
}

void menuAdministrador() {
    int opcao;
    do {
        system("cls");
        printf("\n=== Menu do Administrador ===\n");
        printf("1. Gestão de Incidentes\n");
        printf("2. Gestão de Utilizadores\n");
        printf("3. Logs do Sistema\n");
        printf("0. Logout\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1:
                if (verificarIncidentesExistentes(lista_incidentes)) {
                    menuIncidentesAdmin();
                }
                break;
            case 2:
                menuGestaoUsers();
                break;
            case 3:
                menuLogs();
                break;
            case 0:
                logout();
                return;
        }
    } while (opcao != 0);
}

void menuTecnico() {
    USERS* user = verificarSessaoAtiva();
    if (user == NULL) return;

    int opcao;
    do {
        system("cls");
        printf("\n=== Menu do Técnico ===\n");
        printf("1. Gestão de Incidentes\n");
        printf("2. Meus Incidentes\n");
        printf("3. Histórico de Resoluções\n");
        printf("0. Logout\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1:
                if (verificarIncidentesExistentes(lista_incidentes)) {
                    menuIncidentesTecnico(&lista_incidentes, user->username);
                }
                break;
            case 2:
                if (verificarIncidentesExistentes(lista_incidentes)) {
                    fornecerIncidentesPorTecnico(lista_incidentes, user->username);
                    clickEnter();
                }
                break;
            case 3:
                if (verificarIncidentesExistentes(lista_incidentes)) {
                    ELEM* atual = lista_incidentes;
                    printf("\n=== Histórico de Incidentes Resolvidos ===\n");
                    bool encontrou = false;
                    while (atual != NULL) {
                        if (strcmp(atual->incidente.tecnico_responsavel, user->username) == 0 && 
                            atual->incidente.estado == RESOLVIDO) {
                            mostrarDetalhesIncidente(atual);
                            printf("\n");
                            encontrou = true;
                        }
                        atual = atual->proximo;
                    }
                    if (!encontrou) {
                        printf("Não existem incidentes resolvidos por si.\n");
                    }
                    clickEnter();
                }
                break;
            case 0:
                logout();
                return;
        }
    } while (opcao != 0);
}

void menuIncidentesAdmin() {
    int opcao;
    do {
        system("cls");
        printf("\n=== Menu de Gestão de Incidentes ===\n");
        printf("1. Criar novo incidente\n");
        printf("2. Listar incidentes\n");
        printf("3. Remover incidente\n");
        printf("4. Atualizar estado de incidente\n");
        printf("5. Designar incidente\n");
        printf("6. Adicionar ferramenta\n");
        printf("7. Adicionar comentário\n");
        printf("8. Gerar relatório\n");
        printf("9. Ordenar incidentes\n");
        printf("0. Voltar\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1: {
                TipoIncidente tipo;
                char descricao[500];
                Severidade severidade;

                printf("\nTipo de incidente (1-Hardware, 2-Software, 3-Rede): ");
                scanf("%d", &tipo);
                limparBuffer();

                printf("Descrição: ");
                fgets(descricao, sizeof(descricao), stdin);
                descricao[strcspn(descricao, "\n")] = 0;

                printf("Severidade (1-Baixa, 2-Média, 3-Alta): ");
                scanf("%d", &severidade);
                limparBuffer();

                ELEM* novo = criarIncidente(tipo, descricao, severidade);
                if (novo != NULL) {
                    adicionarIncidente(&lista_incidentes, novo);
                    printf("\nIncidente criado com sucesso!\n");
                } else {
                    printf("\nErro ao criar incidente!\n");
                }
                clickEnter();
                break;
            }
            case 2: {
                if (lista_incidentes == NULL) {
                    printf("\nNão há incidentes registados.\n");
                } else {
                    printf("\n=== Lista de Incidentes ===\n");
                    ELEM* atual = lista_incidentes;
                    while (atual != NULL) {
                        printf("\nID: %d\n", atual->incidente.id);
                        printf("Tipo: %d\n", atual->incidente.tipo);
                        printf("Data/Hora: %s", ctime(&atual->incidente.data_hora));
                        printf("Descrição: %s\n", atual->incidente.descricao);
                        printf("Severidade: %d\n", atual->incidente.severidade);
                        printf("Estado: %d\n", atual->incidente.estado);
                        printf("Técnico: %s\n", atual->incidente.tecnico_responsavel);
                        printf("Tempo de Resolução: %ld segundos\n", atual->incidente.tempo_real);
                        atual = atual->proximo;
                    }
                }
                clickEnter();
                break;
            }
            case 3: {
                int id;
                printf("\nID do incidente a remover: ");
                scanf("%d", &id);
                limparBuffer();

                removerIncidente(&lista_incidentes, id);
                printf("\nIncidente removido com sucesso!\n");
                clickEnter();
                break;
            }
            case 4: {
                int id;
                EstadoIncidente novo_estado;
                printf("\nID do incidente: ");
                scanf("%d", &id);
                limparBuffer();

                printf("Novo estado (1-Por Tratar, 2-Em Análise, 3-Resolvido): ");
                scanf("%d", &novo_estado);
                limparBuffer();

                ELEM* incidente = procurarIncidente(lista_incidentes, id);
                if (incidente != NULL) {
                    atualizarEstadoIncidente(incidente, novo_estado);
                    printf("\nEstado atualizado com sucesso!\n");
                } else {
                    printf("\nIncidente não encontrado!\n");
                }
                clickEnter();
                break;
            }
            case 5: {
                int id;
                char novo_tecnico[50];
                char motivo[500];
                printf("\nID do incidente: ");
                scanf("%d", &id);
                limparBuffer();

                printf("Nome do novo técnico: ");
                fgets(novo_tecnico, sizeof(novo_tecnico), stdin);
                novo_tecnico[strcspn(novo_tecnico, "\n")] = 0;

                printf("Motivo da designação: ");
                fgets(motivo, sizeof(motivo), stdin);
                motivo[strcspn(motivo, "\n")] = 0;

                ELEM* incidente = procurarIncidente(lista_incidentes, id);
                if (incidente != NULL) {
                    designarIncidente(incidente, novo_tecnico, motivo);
                    printf("\nIncidente designado com sucesso!\n");
                } else {
                    printf("\nIncidente não encontrado!\n");
                }
                clickEnter();
                break;
            }
            case 6: {
                int id;
                char nome[50];
                printf("\nID do incidente: ");
                scanf("%d", &id);
                limparBuffer();

                printf("Nome da ferramenta: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = 0;

                ELEM* incidente = procurarIncidente(lista_incidentes, id);
                if (incidente != NULL) {
                    adicionarFerramenta(incidente, nome);
                    printf("\nFerramenta adicionada com sucesso!\n");
                } else {
                    printf("\nIncidente não encontrado!\n");
                }
                clickEnter();
                break;
            }
            case 7: {
                int id;
                char comentario[500];
                printf("\nID do incidente: ");
                scanf("%d", &id);
                limparBuffer();

                printf("Comentário: ");
                fgets(comentario, sizeof(comentario), stdin);
                comentario[strcspn(comentario, "\n")] = 0;

                ELEM* incidente = procurarIncidente(lista_incidentes, id);
                if (incidente != NULL) {
                    adicionarAcaoHistorico(incidente, comentario, current_user->username);
                    printf("\nComentário adicionado com sucesso!\n");
                } else {
                    printf("\nIncidente não encontrado!\n");
                }
                clickEnter();
                break;
            }
            case 8: {
                time_t inicio, fim;
                char ficheiro[100];
                struct tm* data_inicio = malloc(sizeof(struct tm));
                struct tm* data_fim = malloc(sizeof(struct tm));

                printf("\nData inicial (DD/MM/AAAA): ");
                scanf("%d/%d/%d", &data_inicio->tm_mday, &data_inicio->tm_mon, &data_inicio->tm_year);
                data_inicio->tm_mon--;
                data_inicio->tm_year -= 1900;
                data_inicio->tm_hour = 0;
                data_inicio->tm_min = 0;
                data_inicio->tm_sec = 0;
                inicio = mktime(data_inicio);

                printf("Data final (DD/MM/AAAA): ");
                scanf("%d/%d/%d", &data_fim->tm_mday, &data_fim->tm_mon, &data_fim->tm_year);
                data_fim->tm_mon--;
                data_fim->tm_year -= 1900;
                data_fim->tm_hour = 23;
                data_fim->tm_min = 59;
                data_fim->tm_sec = 59;
                fim = mktime(data_fim);

                printf("Nome do ficheiro de relatório: ");
                scanf("%s", ficheiro);
                limparBuffer();

                criarRelatorio(lista_incidentes, ficheiro, inicio, fim);
                printf("\nRelatório criado com sucesso!\n");

                free(data_inicio);
                free(data_fim);
                clickEnter();
                break;
            }
            case 9:
                ordenarIncidentes(&lista_incidentes);
                clickEnter();
                break;
            case 0:
                break;
            default:
                printf("\nOpção inválida!\n");
                clickEnter();
        }
    } while (opcao != 0);
}

void fornecerIncidentes(ELEM* lista) {
    printf("\n=== Lista de Incidentes ===\n");
    ELEM* atual = lista;
    while (atual != NULL) {
        mostrarDetalhesIncidente(atual);
        printf("\n");
        atual = atual->proximo;
    }
}

void fornecerIncidentesPorEstado(ELEM* lista, EstadoIncidente estado) {
    printf("\n=== Incidentes por Estado ===\n");
    ELEM* atual = lista;
    while (atual != NULL) {
        if (atual->incidente.estado == estado) {
            mostrarDetalhesIncidente(atual);
            printf("\n");
        }
        atual = atual->proximo;
    }
}

void fornecerIncidentesPorSeveridade(ELEM* lista, Severidade severidade) {
    printf("\n=== Incidentes por Severidade ===\n");
    ELEM* atual = lista;
    while (atual != NULL) {
        if (atual->incidente.severidade == severidade) {
            mostrarDetalhesIncidente(atual);
            printf("\n");
        }
        atual = atual->proximo;
    }
}

void fornecerIncidentesPorTipo(ELEM* lista, TipoIncidente tipo) {
    printf("\n=== Incidentes por Tipo ===\n");
    ELEM* atual = lista;
    while (atual != NULL) {
        if (atual->incidente.tipo == tipo) {
            mostrarDetalhesIncidente(atual);
            printf("\n");
        }
        atual = atual->proximo;
    }
}

void fornecerIncidentesPorTecnico(ELEM* lista, const char* tecnico) {
    printf("\n=== Incidentes por Técnico ===\n");
    ELEM* atual = lista;
    while (atual != NULL) {
        if (strcmp(atual->incidente.tecnico_responsavel, tecnico) == 0) {
            mostrarDetalhesIncidente(atual);
            printf("\n");
        }
        atual = atual->proximo;
    }
}

void fornecerIncidentesPorPeriodo(ELEM* lista, time_t inicio, time_t fim) {
    printf("\n=== Incidentes por Período ===\n");
    ELEM* atual = lista;
    while (atual != NULL) {
        if (atual->incidente.data_hora >= inicio && atual->incidente.data_hora <= fim) {
            mostrarDetalhesIncidente(atual);
            printf("\n");
        }
        atual = atual->proximo;
    }
}

void mostrarDetalhesIncidente(ELEM* elem) {
    printf("ID: %d\n", elem->incidente.id);
    printf("Tipo: %d\n", elem->incidente.tipo);
    printf("Data/Hora: %s", ctime(&elem->incidente.data_hora));
    printf("Descrição: %s\n", elem->incidente.descricao);
    printf("Severidade: %d\n", elem->incidente.severidade);
    printf("Estado: %d\n", elem->incidente.estado);
    printf("Técnico: %s\n", elem->incidente.tecnico_responsavel);
    if (elem->incidente.estado == RESOLVIDO) {
        printf("Tempo de Resolução: %ld segundos\n", elem->incidente.tempo_real);
    }
}

void mostrarHistoricoIncidente(ELEM* elem) {
    printf("\n=== Histórico do Incidente %d ===\n", elem->incidente.id);
    for (int i = 0; i < elem->incidente.num_acoes; i++) {
        printf("Data/Hora: %s", ctime(&elem->incidente.historico[i].data_hora));
        printf("Técnico: %s\n", elem->incidente.historico[i].tecnico);
        printf("Ação: %s\n\n", elem->incidente.historico[i].descricao);
    }
}

void mostrarFerramentasUtilizadas(ELEM* elem) {
    printf("\n=== Ferramentas Utilizadas no Incidente %d ===\n", elem->incidente.id);
    for (int i = 0; i < elem->incidente.num_ferramentas; i++) {
        printf("Ferramenta: %s\n", elem->incidente.ferramentas[i].nome);
        printf("Data de Uso: %s", ctime(&elem->incidente.ferramentas[i].data_uso));
    }
}

void calcularTempoMedioResolucao(ELEM* lista, const char* tecnico) {
    long total_tempo = 0;
    int num_incidentes = 0;
    
    ELEM* atual = lista;
    while (atual != NULL) {
        if (strcmp(atual->incidente.tecnico_responsavel, tecnico) == 0 && 
            atual->incidente.estado == RESOLVIDO) {
            total_tempo += atual->incidente.tempo_real;
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

void menuGestaoUsers() {
    int opcao;
    do {
        system("cls");
        printf("\n=== Gestão de Utilizadores ===\n");
        printf("1. Listar Utilizadores\n");
        printf("2. Adicionar Novo Utilizador\n");
        printf("3. Adicionar Novo Administrador\n");
        printf("4. Remover Utilizador\n");
        printf("5. Modificar Tipo de Utilizador\n");
        printf("6. Alterar Password\n");
        printf("0. Voltar\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1:
                listarUtilizadores();
                break;
            case 2:
                registo(false); // Cria um novo técnico
                break;
            case 3:
                registo(true); // Cria um novo administrador
                break;
            case 4: {
                char username[50];
                printf("Username do utilizador a remover: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0;
                removerUtilizador(username);
                break;
            }
            case 5: {
                char username[50];
                int novo_tipo;
                printf("Username do utilizador: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0;
                printf("Novo tipo (1-Técnico, 2-Administrador): ");
                scanf("%d", &novo_tipo);
                limparBuffer();
                modificarTipoUtilizador(username, novo_tipo);
                break;
            }
            case 6: {
                char username[50];
                char nova_senha[50];
                printf("Username do utilizador: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0;
                printf("Nova password: ");
                lerPassword(nova_senha, sizeof(nova_senha));
                alterarPasswordUtilizador(username, nova_senha);
                break;
            }
            case 0:
                return;
            default:
                printf("Opção inválida!\n");
                clickEnter();
        }
    } while (opcao != 0);
}

void menuLogs() {
    int opcao;
    do {
        limparEcra();
        printf("\n=== Menu de Logs do Sistema ===\n");
        printf("1. Ver todos os logs\n");
        printf("2. Filtrar logs por utilizador\n");
        printf("3. Filtrar logs por período\n");
        printf("4. Guardar logs em ficheiro\n");
        printf("0. Voltar\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                mostrarLogs();
                clickEnter();
                break;
            case 2: {
                char username[50];
                printf("Nome do utilizador: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0;
                mostrarLogsPorUtilizador(username);
                clickEnter();
                break;
            }
            case 3: {
                time_t inicio, fim;
                struct tm tm_inicio = {0}, tm_fim = {0};
                char data_inicio[20], data_fim[20];
                
                printf("Data inicial (DD/MM/2024): ");
                fgets(data_inicio, sizeof(data_inicio), stdin);
                data_inicio[strcspn(data_inicio, "\n")] = 0;
                
                printf("Data final (DD/MM/2024): ");
                fgets(data_fim, sizeof(data_fim), stdin);
                data_fim[strcspn(data_fim, "\n")] = 0;
                
                converterDataStringParaTM(data_inicio, &tm_inicio);
                converterDataStringParaTM(data_fim, &tm_fim);
                
                inicio = mktime(&tm_inicio);
                fim = mktime(&tm_fim);
                
                mostrarLogsPorPeriodo(inicio, fim);
                clickEnter();
                break;
            }
            case 4: {
                char ficheiro[100];
                printf("Nome do ficheiro para guardar os logs: ");
                fgets(ficheiro, sizeof(ficheiro), stdin);
                ficheiro[strcspn(ficheiro, "\n")] = 0;
                guardarLogs(ficheiro);
                clickEnter();
                break;
            }
            case 0:
                return;
            default:
                printf("Opção inválida!\n");
                clickEnter();
                break;
        }
    } while (opcao != 0);
}





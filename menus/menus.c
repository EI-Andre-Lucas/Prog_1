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
        printf("8. Gerir respostas\n");
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
                limparBuffer();

                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
                if (!incidente) {
                    printf("Incidente não encontrado!\n");
                    system("pause");
                    break;
                }

                printf("Comentário: ");
                if (fgets(comentario, sizeof(comentario), stdin) == NULL) {
                    printf("Erro ao ler comentário!\n");
                    system("pause");
                    break;
                }
                // Remover o \n do final, se existir
                comentario[strcspn(comentario, "\n")] = 0;

                if (strlen(comentario) == 0) {
                    printf("Comentário vazio!\n");
                    system("pause");
                    break;
                }

                USERS* utilizador_atual = verificarSessaoAtiva();
                if (!utilizador_atual) {
                    printf("Sessão inválida!\n");
                    break;
                }
                adicionarAcaoHistorico(incidente, comentario, utilizador_atual->username);
                printf("Comentário adicionado com sucesso!\n");
                guardarIncidentes(*lista_incidentes, "incidentes.bin");
                system("pause");
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
                char username_tecnico[50];
                printf("ID do incidente: ");
                scanf("%d", &id);
                limparBuffer();

                printf("Username do técnico: ");
                scanf("%s", username_tecnico);
                limparBuffer();

                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
                if (!incidente) {
                    printf("Incidente não encontrado!\n");
                    break;
                }
                char motivo[200] = "Designação manual pelo administrador";
                designarIncidente(incidente, username_tecnico, motivo);
                system("pause");
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
            case 8: {
                int id;
                printf("\nID do incidente: ");
                scanf("%d", &id);
                limparBuffer();
                
                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
                if (!incidente) {
                    printf("Incidente não encontrado!\n");
                    system("pause");
                    break;
                }

                USERS* utilizador_atual = verificarSessaoAtiva();
                if (!utilizador_atual) {
                    printf("Sessão inválida!\n");
                    system("pause");
                    break;
                }

                int subopcao;
                do {
                    printf("\n=== Gestão de Respostas ===\n");
                    printf("1. Ver respostas\n");
                    printf("2. Adicionar resposta\n");
                    printf("3. Marcar resposta como solução\n");
                    printf("0. Voltar\n");
                    printf("Escolha uma opção: ");
                    scanf("%d", &subopcao);
                    limparBuffer();

                    switch (subopcao) {
                        case 1:
                            listarRespostas(incidente);
                            system("pause");
                            break;
                        case 2: {
                            char resposta[1000];
                            bool solucao;
                            printf("Resposta: ");
                            limparBuffer();
                            fgets(resposta, sizeof(resposta), stdin);
                            resposta[strcspn(resposta, "\n")] = 0;
                            
                            printf("Esta resposta resolve o incidente? (1-Sim, 0-Não): ");
                            scanf("%d", &solucao);
                            limparBuffer();
                            
                            adicionarResposta(incidente, resposta, utilizador_atual->username, solucao);
                            guardarIncidentes(*lista_incidentes, "incidentes.bin");
                            printf("\nResposta adicionada com sucesso!\n");
                            system("pause");
                            break;
                        }
                        case 3: {
                            int indice;
                            listarRespostas(incidente);
                            printf("\nNúmero da resposta a marcar como solução: ");
                            scanf("%d", &indice);
                            limparBuffer();
                            
                            marcarRespostaComoSolucao(incidente, indice - 1);
                            guardarIncidentes(*lista_incidentes, "incidentes.bin");
                            printf("\nResposta marcada como solução!\n");
                            system("pause");
                            break;
                        }
                        case 0:
                            break;
                        default:
                            printf("\nOpção inválida!\n");
                            system("pause");
                    }
                } while (subopcao != 0);
                break;
            }
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
        printf("4. Relatórios\n");
        printf("0. Logout\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1:
                menuIncidentesAdmin(&lista_incidentes);
                break;
            case 2:
                menuGestaoUsers();
                break;
            case 3:
                menuLogs();
                break;
            case 4:
                if (verificarIncidentesExistentes(lista_incidentes)) {
                    gerarRelatorioPeriodico(lista_incidentes);
                    clickEnter();
                }
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

void menuIncidentesAdmin(ELEM** lista_incidentes) {
    if (!lista_incidentes) {
        printf("\nErro: Lista de incidentes inválida!\n");
        return;
    }

    // Garantir que a lista está carregada
    if (!*lista_incidentes) {
        *lista_incidentes = carregarIncidentes("incidentes.bin");
        if (!*lista_incidentes) {
            printf("\nErro ao carregar incidentes!\n");
            return;
        }
    }

    int opcao;
    do {
        printf("\n=== Gestão de Incidentes (Admin) ===\n");
        printf("1. Listar incidentes\n");
        printf("2. Criar incidente\n");
        printf("3. Atualizar estado\n");
        printf("4. Designar técnico\n");
        printf("5. Adicionar comentário\n");
        printf("6. Adicionar ferramenta\n");
        printf("7. Adicionar resposta\n");
        printf("8. Gerar relatório\n");
        printf("9. Remover incidente\n");
        printf("0. Voltar\n");
        printf("Opção: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1: {
                ELEM* atual = *lista_incidentes;
                printf("\n=== Lista de Incidentes ===\n");
                while (atual != NULL) {
                    mostrarDetalhesIncidente(atual);
                    atual = atual->proximo;
                }
                clickEnter();
                break;
            }
            case 2: {
                TipoIncidente tipo;
                char descricao[500];
                Severidade severidade;
                int tempo_estimado;

                printf("\nTipo de incidente:\n");
                printf("1 - Phishing\n");
                printf("2 - Malware\n");
                printf("3 - Acesso Nao Autorizado\n");
                printf("4 - Outro\n");
                printf("Escolha uma opcao: ");
                scanf("%d", &tipo);
                limparBuffer();

                printf("\nDescricao do incidente: ");
                fgets(descricao, sizeof(descricao), stdin);
                descricao[strcspn(descricao, "\n")] = 0;

                printf("Tempo estimado (em horas): ");
                scanf("%d", &tempo_estimado);
                limparBuffer();

                printf("\nSeveridade:\n");
                printf("1 - Baixa\n");
                printf("2 - Media\n");
                printf("3 - Alta\n");
                printf("4 - Critica\n");
                printf("Escolha uma opcao: ");
                scanf("%d", &severidade);
                limparBuffer();

                // Criar o incidente
                ELEM* novo = criarIncidente(tipo, descricao, tempo_estimado, severidade);
                if (novo) {
                    // Adiciona à lista
                    adicionarIncidente(lista_incidentes, novo);
                    
                    // Salva no arquivo
                    guardarIncidentes(*lista_incidentes, "incidentes.bin");
                    
                    printf("\nIncidente criado com sucesso!\n");
                    system("pause");
                } else {
                    printf("\nErro ao criar incidente!\n");
                }
                clickEnter();
                break;
            }
            case 3: {
                int id;
                EstadoIncidente novo_estado;
                printf("\nID do incidente: ");
                scanf("%d", &id);
                limparBuffer();

                printf("Novo estado (1-Por Tratar, 2-Em Análise, 3-Resolvido): ");
                scanf("%d", &novo_estado);
                limparBuffer();

                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
                if (incidente != NULL) {
                    atualizarEstadoIncidente(incidente, novo_estado);
                    printf("\nEstado atualizado com sucesso!\n");
                } else {
                    printf("\nIncidente não encontrado!\n");
                }
                clickEnter();
                break;
            }
            case 4: {
                int id;
                char username_tecnico[50];
                printf("\nID do incidente: ");
                scanf("%d", &id);
                limparBuffer();

                printf("Username do técnico: ");
                scanf("%s", username_tecnico);
                limparBuffer();

                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
                if (!incidente) {
                    printf("Incidente não encontrado!\n");
                    break;
                }
                char motivo[200] = "Designação manual pelo administrador";
                designarIncidente(incidente, username_tecnico, motivo);
                system("pause");
                break;
            }
            case 5: {
                int id;
                char comentario[500] = {0};  // Inicializar com zeros
                printf("\nID do incidente: ");
                scanf("%d", &id);
                limparBuffer();

                printf("Comentario: ");
                scanf(" %[^\n]s", comentario);  // Ler até encontrar uma nova linha
                
                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
                if (incidente != NULL) {
                    USERS* utilizador_atual = verificarSessaoAtiva();
                    if (!utilizador_atual) {
                        printf("Sessão inválida!\n");
                        break;
                    }
                    adicionarAcaoHistorico(incidente, comentario, utilizador_atual->username);
                    printf("Comentário adicionado com sucesso!\n");
                    guardarIncidentes(*lista_incidentes, "incidentes.bin");
                } else {
                    printf("\nIncidente nao encontrado!\n");
                }
                system("pause");
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

                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
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
                printf("\nID do incidente: ");
                scanf("%d", &id);
                limparBuffer();

                ELEM* incidente = procurarIncidente(*lista_incidentes, id);
                if (!incidente) {
                    printf("Incidente não encontrado!\n");
                    system("pause");
                    break;
                }

                USERS* utilizador_atual = verificarSessaoAtiva();
                if (!utilizador_atual) {
                    printf("Sessão inválida!\n");
                    system("pause");
                    break;
                }

                int subopcao;
                do {
                    printf("\n=== Gestão de Respostas ===\n");
                    printf("1. Ver respostas\n");
                    printf("2. Adicionar resposta\n");
                    printf("3. Marcar resposta como solução\n");
                    printf("0. Voltar\n");
                    printf("Escolha uma opção: ");
                    scanf("%d", &subopcao);
                    limparBuffer();

                    switch (subopcao) {
                        case 1:
                            listarRespostas(incidente);
                            system("pause");
                            break;
                        case 2: {
                            char resposta[1000];
                            bool solucao;
                            printf("Resposta: ");
                            limparBuffer();
                            fgets(resposta, sizeof(resposta), stdin);
                            resposta[strcspn(resposta, "\n")] = 0;
                            
                            printf("Esta resposta resolve o incidente? (1-Sim, 0-Não): ");
                            scanf("%d", &solucao);
                            limparBuffer();
                            
                            adicionarResposta(incidente, resposta, utilizador_atual->username, solucao);
                            guardarIncidentes(*lista_incidentes, "incidentes.bin");
                            printf("\nResposta adicionada com sucesso!\n");
                            system("pause");
                            break;
                        }
                        case 3: {
                            int indice;
                            listarRespostas(incidente);
                            printf("\nNúmero da resposta a marcar como solução: ");
                            scanf("%d", &indice);
                            limparBuffer();
                            
                            marcarRespostaComoSolucao(incidente, indice - 1);
                            guardarIncidentes(*lista_incidentes, "incidentes.bin");
                            printf("\nResposta marcada como solução!\n");
                            system("pause");
                            break;
                        }
                        case 0:
                            break;
                        default:
                            printf("\nOpção inválida!\n");
                            system("pause");
                    }
                } while (subopcao != 0);
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

                criarRelatorio(*lista_incidentes, ficheiro, inicio, fim);
                printf("\nRelatório criado com sucesso!\n");

                free(data_inicio);
                free(data_fim);
                clickEnter();
                break;
            }
            case 9: {
                int id;
                printf("\nID do incidente a remover: ");
                scanf("%d", &id);
                limparBuffer();

                printf("\nTem certeza que deseja remover o incidente #%d? (1-Sim, 0-Não): ", id);
                int confirmacao;
                scanf("%d", &confirmacao);
                limparBuffer();

                if (confirmacao == 1) {
                    removerIncidente(lista_incidentes, id);
                } else {
                    printf("\nOperação cancelada.\n");
                }
                system("pause");
                break;
            }
            case 0:
                return;
            default:
                printf("\nOpção inválida!\n");
                system("pause");
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
    if (!elem) {
        printf("\nIncidente nao encontrado.\n");
        return;
    }

    char data_str[20];
    strftime(data_str, sizeof(data_str), "%d/%m/%Y %H:%M", localtime(&elem->incidente.data_hora));

    const char* estado_str;
    switch(elem->incidente.estado) {
        case POR_TRATAR: estado_str = "Por tratar"; break;
        case EM_ANALISE: estado_str = "Em analise"; break;
        case RESOLVIDO: estado_str = "Resolvido"; break;
        default: estado_str = "Desconhecido";
    }

    const char* tipo_str;
    switch(elem->incidente.tipo) {
        case PHISHING: tipo_str = "Phishing"; break;
        case MALWARE: tipo_str = "Malware"; break;
        case ACESSO_NAO_AUTORIZADO: tipo_str = "Acesso Nao Autorizado"; break;
        default: tipo_str = "Outro";
    }

    const char* severidade_str;
    switch(elem->incidente.severidade) {
        case BAIXA: severidade_str = "Baixa"; break;
        case MEDIA: severidade_str = "Media"; break;
        case ALTA: severidade_str = "Alta"; break;
        default: severidade_str = "Desconhecida";
    }

    printf("\n=== DETALHES DO INCIDENTE ===\n");
    printf("ID: %d\n", elem->incidente.id);
    printf("Tipo: %s\n", tipo_str);
    printf("Data/Hora: %s\n", data_str);
    printf("Descricao: %s\n", elem->incidente.descricao);
    printf("Severidade: %s\n", severidade_str);
    printf("Estado: %s\n", estado_str);
    printf("Tecnico: %s\n", elem->incidente.tecnico_responsavel);
    
    if (elem->incidente.estado == RESOLVIDO) {
        printf("Tempo de Resolucao: %ld segundos\n", elem->incidente.tempo_real);
    }

    // Mostrar histórico de ações
    printf("\n=== HISTORICO DE ACOES ===\n");
    for (int i = 0; i < elem->incidente.num_acoes; i++) {
        char acao_data[20];
        strftime(acao_data, sizeof(acao_data), "%d/%m/%Y %H:%M", 
                localtime(&elem->incidente.historico[i].data_hora));
        printf("%d. [%s] %s - %s\n", 
               i + 1,
               acao_data,
               elem->incidente.historico[i].tecnico,
               elem->incidente.historico[i].descricao);
    }

    // Mostrar ferramentas utilizadas
    printf("\n=== FERRAMENTAS UTILIZADAS ===\n");
    for (int i = 0; i < elem->incidente.num_ferramentas; i++) {
        printf("%d. %s\n", i + 1, elem->incidente.ferramentas[i].nome);
    }

    // Mostrar respostas/comentários
    printf("\n=== RESPOSTAS/COMENTARIOS ===\n");
    for (int i = 0; i < elem->incidente.num_respostas; i++) {
        char resposta_data[20];
        strftime(resposta_data, sizeof(resposta_data), "%d/%m/%Y %H:%M", 
                localtime(&elem->incidente.respostas[i].data_hora));
        printf("%d. [%s] %s - %s\n", 
               i + 1,
               resposta_data,
               elem->incidente.respostas[i].autor,
               elem->incidente.respostas[i].resposta);
        if (elem->incidente.respostas[i].solucao) {
            printf("   [SOLUCAO]\n");
        }
    }
    printf("\n");
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





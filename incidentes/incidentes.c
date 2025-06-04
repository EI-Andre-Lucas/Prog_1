#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "incidentes.h"
#include "../utils/utils.h"
#include "../logs/logs.h"
#include "../users/users.h"

// Função para obter o próximo ID disponível
int obterProximoId(ELEM* lista) {
    int max_id = 0;
    ELEM* atual = lista;
    
    while (atual != NULL) {
        if (atual->incidente.id > max_id) {
            max_id = atual->incidente.id;
        }
        atual = atual->proximo;
    }
    
    return max_id + 1;
}

ELEM* criarIncidente(TipoIncidente tipo, const char* descricao, Severidade severidade) {
    ELEM* lista = carregarIncidentes("../incidentes/incidentes.bin");
    if (lista == NULL) {
        lista = NULL; // Cria lista vazia se não existir arquivo
    }
    ELEM* novo = (ELEM*)malloc(sizeof(ELEM));
    if (novo == NULL) {
        registarLog("SISTEMA", "Erro ao criar incidente - falha de memória");
        return NULL;
    }

    // Obtém o próximo ID disponível
    novo->incidente.id = obterProximoId(lista);
    novo->incidente.tipo = tipo;
    novo->incidente.data_hora = time(NULL);
    strncpy(novo->incidente.descricao, descricao, sizeof(novo->incidente.descricao) - 1);
    novo->incidente.descricao[sizeof(novo->incidente.descricao) - 1] = '\0';
    novo->incidente.severidade = severidade;
    novo->incidente.estado = POR_TRATAR;
    novo->incidente.tecnico_responsavel[0] = '\0';
    novo->incidente.tempo_estimado = 0;
    novo->incidente.tempo_real = 0;
    novo->incidente.historico = NULL;
    novo->incidente.num_acoes = 0;
    novo->incidente.ferramentas = NULL;
    novo->incidente.num_ferramentas = 0;
    novo->proximo = NULL;
    novo->anterior = NULL;

    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        char log_message[150];
        snprintf(log_message, sizeof(log_message), "Criou o incidente #%d (%s)", 
                novo->incidente.id, descricao);
        registarLog(current_user->username, log_message);
    }

    return novo;
}

void adicionarIncidente(ELEM** lista, ELEM* novo) {
    if (*lista == NULL) {
        *lista = novo;
        return;
    }

    ELEM* atual = *lista;
    while (atual->proximo != NULL) {
        atual = atual->proximo;
    }
    atual->proximo = novo;
    novo->anterior = atual;
}

void removerIncidente(ELEM** lista, int id) {
    if (*lista == NULL) {
        registarLog("SISTEMA", "Tentativa de remover incidente falhou - lista vazia");
        return;
    }

    ELEM* atual = *lista;
    while (atual != NULL && atual->incidente.id != id) {
        atual = atual->proximo;
    }

    if (atual == NULL) {
        registarLog("SISTEMA", "Tentativa de remover incidente falhou - incidente não encontrado");
        return;
    }

    if (atual->anterior == NULL) {
        *lista = atual->proximo;
        if (*lista != NULL) {
            (*lista)->anterior = NULL;
        }
    } else {
        atual->anterior->proximo = atual->proximo;
        if (atual->proximo != NULL) {
            atual->proximo->anterior = atual->anterior;
        }
    }

    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        char log_message[150];
        snprintf(log_message, sizeof(log_message), "Removeu o incidente #%d", id);
        registarLog(current_user->username, log_message);
    }

    free(atual->incidente.historico);
    free(atual->incidente.ferramentas);
    free(atual);
}

ELEM* procurarIncidente(ELEM* lista, int id) {
    ELEM* atual = lista;
    while (atual != NULL) {
        if (atual->incidente.id == id) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL;
}

void atualizarEstadoIncidente(ELEM* elem, EstadoIncidente novo_estado) {
    if (elem == NULL) {
        registarLog("SISTEMA", "Tentativa de atualizar estado falhou - incidente não encontrado");
        return;
    }
    
    EstadoIncidente estado_anterior = elem->incidente.estado;
    elem->incidente.estado = novo_estado;
    
    if (novo_estado == RESOLVIDO) {
        elem->incidente.tempo_real = time(NULL) - elem->incidente.data_hora;
    }

    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        char log_message[150];
        const char* estados[] = {"Por Tratar", "Em Análise", "Resolvido"};
        snprintf(log_message, sizeof(log_message), "Alterou o estado do incidente #%d de %s para %s", 
                elem->incidente.id, estados[estado_anterior], estados[novo_estado]);
        registarLog(current_user->username, log_message);
    }
}

void adicionarAcaoHistorico(ELEM* elem, const char* descricao, const char* tecnico) {
    if (elem == NULL) {
        registarLog("SISTEMA", "Tentativa de adicionar ação ao histórico falhou - incidente não encontrado");
        return;
    }

    HistoricoAcao* novo_historico = realloc(elem->incidente.historico, 
                                          (elem->incidente.num_acoes + 1) * sizeof(HistoricoAcao));
    
    if (novo_historico == NULL) {
        registarLog("SISTEMA", "Erro ao adicionar ação ao histórico - falha de memória");
        return;
    }

    elem->incidente.historico = novo_historico;
    HistoricoAcao* nova_acao = &elem->incidente.historico[elem->incidente.num_acoes];
    
    nova_acao->data_hora = time(NULL);
    strncpy(nova_acao->descricao, descricao, sizeof(nova_acao->descricao) - 1);
    nova_acao->descricao[sizeof(nova_acao->descricao) - 1] = '\0';
    strncpy(nova_acao->tecnico, tecnico, sizeof(nova_acao->tecnico) - 1);
    nova_acao->tecnico[sizeof(nova_acao->tecnico) - 1] = '\0';
    
    elem->incidente.num_acoes++;

    char log_message[150];
    snprintf(log_message, sizeof(log_message), "Adicionou comentário ao incidente #%d: %s", 
            elem->incidente.id, descricao);
    registarLog(tecnico, log_message);
}

void adicionarFerramenta(ELEM* elem, const char* nome) {
    if (elem == NULL) {
        registarLog("SISTEMA", "Tentativa de adicionar ferramenta falhou - incidente não encontrado");
        return;
    }

    Ferramenta* novas_ferramentas = realloc(elem->incidente.ferramentas,
                                          (elem->incidente.num_ferramentas + 1) * sizeof(Ferramenta));
    
    if (novas_ferramentas == NULL) {
        registarLog("SISTEMA", "Erro ao adicionar ferramenta - falha de memória");
        return;
    }

    elem->incidente.ferramentas = novas_ferramentas;
    Ferramenta* nova_ferramenta = &elem->incidente.ferramentas[elem->incidente.num_ferramentas];
    
    strncpy(nova_ferramenta->nome, nome, sizeof(nova_ferramenta->nome) - 1);
    nova_ferramenta->nome[sizeof(nova_ferramenta->nome) - 1] = '\0';
    nova_ferramenta->data_uso = time(NULL);
    
    elem->incidente.num_ferramentas++;

    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        char log_message[150];
        snprintf(log_message, sizeof(log_message), "Registou uso da ferramenta '%s' no incidente #%d", 
                nome, elem->incidente.id);
        registarLog(current_user->username, log_message);
    }
}

void designarIncidente(ELEM* elem, const char* novo_tecnico, const char* motivo) {
    if (elem == NULL) {
        registarLog("SISTEMA", "Tentativa de designar incidente falhou - incidente não encontrado");
        return;
    }

    // Verificar se o técnico existe
    USERS* tecnico = procurarTecnico(novo_tecnico);
    if (tecnico == NULL) {
        printf("\nErro: Técnico '%s' não encontrado no sistema.\n", novo_tecnico);
        printf("Por favor, selecione outro técnico.\n");
        return;
    }

    // Verificar se o user é um técnico
    if (tecnico->tipoUser != TECNICO) {
        printf("\nErro: O user '%s' não é um técnico.\n", novo_tecnico);
        printf("Por favor, selecione um técnico válido.\n");
        return;
    }else{
        char descricao[500];
        snprintf(descricao, sizeof(descricao), "Incidente delegado para %s. Motivo: %s", 
                 novo_tecnico, motivo);
        adicionarAcaoHistorico(elem, descricao, elem->incidente.tecnico_responsavel);
        strncpy(elem->incidente.tecnico_responsavel, novo_tecnico, sizeof(elem->incidente.tecnico_responsavel) - 1);
        elem->incidente.tecnico_responsavel[sizeof(elem->incidente.tecnico_responsavel) - 1] = '\0';
        USERS* current_user = verificarSessaoAtiva();
        if (current_user != NULL) {
            char log_message[150];
            snprintf(log_message, sizeof(log_message), "Atribuiu o incidente #%d para %s", 
                    elem->incidente.id, novo_tecnico);
            registarLog(current_user->username, log_message);
        }
        printf("\nIncidente #%d atribuído com sucesso para o técnico %s.\n", 
        elem->incidente.id, novo_tecnico);
    }
}

void guardarIncidentes(ELEM* lista, const char* ficheiro) {
    FILE* file = fopen(ficheiro, "wb");
    if (file == NULL) return;

    ELEM* atual = lista;
    while (atual != NULL) {
        fwrite(&atual->incidente, sizeof(Incidente), 1, file);
        
        // Guardar histórico
        fwrite(&atual->incidente.num_acoes, sizeof(int), 1, file);
        if (atual->incidente.num_acoes > 0) {
            fwrite(atual->incidente.historico, sizeof(HistoricoAcao), atual->incidente.num_acoes, file);
        }
        
        // Guardar ferramentas
        fwrite(&atual->incidente.num_ferramentas, sizeof(int), 1, file);
        if (atual->incidente.num_ferramentas > 0) {
            fwrite(atual->incidente.ferramentas, sizeof(Ferramenta), atual->incidente.num_ferramentas, file);
        }
        
        atual = atual->proximo;
    }

    fclose(file);
}

ELEM* carregarIncidentes(const char* ficheiro) {
    FILE* file = fopen(ficheiro, "rb");
    if (file == NULL) return NULL;

    ELEM* lista = NULL;
    ELEM* atual = NULL;

    while (!feof(file)) {
        ELEM* novo = (ELEM*)malloc(sizeof(ELEM));
        if (novo == NULL) break;

        if (fread(&novo->incidente, sizeof(Incidente), 1, file) != 1) {
            free(novo);
            break;
        }

        // Carregar histórico
        fread(&novo->incidente.num_acoes, sizeof(int), 1, file);
        if (novo->incidente.num_acoes > 0) {
            novo->incidente.historico = malloc(novo->incidente.num_acoes * sizeof(HistoricoAcao));
            fread(novo->incidente.historico, sizeof(HistoricoAcao), novo->incidente.num_acoes, file);
        }

        // Carregar ferramentas
        fread(&novo->incidente.num_ferramentas, sizeof(int), 1, file);
        if (novo->incidente.num_ferramentas > 0) {
            novo->incidente.ferramentas = malloc(novo->incidente.num_ferramentas * sizeof(Ferramenta));
            fread(novo->incidente.ferramentas, sizeof(Ferramenta), novo->incidente.num_ferramentas, file);
        }

        novo->proximo = NULL;
        novo->anterior = NULL;

        if (lista == NULL) {
            lista = novo;
            atual = novo;
        } else {
            atual->proximo = novo;
            novo->anterior = atual;
            atual = novo;
        }
    }

    fclose(file);
    return lista;
}

void criarRelatorio(ELEM* lista, const char* ficheiro, time_t inicio, time_t fim) {
    FILE* file = fopen(ficheiro, "w");
    if (file == NULL) return;

    fprintf(file, "Relatório de Incidentes\n");
    fprintf(file, "Período: %s a %s\n\n", ctime(&inicio), ctime(&fim));

    ELEM* atual = lista;
    while (atual != NULL) {
        if (atual->incidente.data_hora >= inicio && atual->incidente.data_hora <= fim) {
            fprintf(file, "ID: %d\n", atual->incidente.id);
            fprintf(file, "Tipo: %d\n", atual->incidente.tipo);
            fprintf(file, "Data/Hora: %s", ctime(&atual->incidente.data_hora));
            fprintf(file, "Descrição: %s\n", atual->incidente.descricao);
            fprintf(file, "Severidade: %d\n", atual->incidente.severidade);
            fprintf(file, "Estado: %d\n", atual->incidente.estado);
            fprintf(file, "Técnico: %s\n", atual->incidente.tecnico_responsavel);
            fprintf(file, "Tempo de Resolução: %ld segundos\n\n", atual->incidente.tempo_real);
        }
        atual = atual->proximo;
    }

    fclose(file);
}

void fornecerListaIncidentes(ELEM* lista) {
    while (lista != NULL) {
        ELEM* temp = lista;
        lista = lista->proximo;
        free(temp->incidente.historico);
        free(temp->incidente.ferramentas);
        free(temp);
    }
}

bool verificarIncidentesExistentes(ELEM* lista) {
    // Se a lista já estiver carregada, retorna true
    if (lista != NULL) {
        return true;
    }

    // Se não houver na lista, tenta carregar do ficheiro
    FILE *f = fopen("incidentes.bin", "rb");
    if (!f) {
        printf("\nNão existem incidentes registados no sistema.\n");
        clickEnter();
        return false;
    }
    fclose(f);
    return true;
}

void limparListaIncidentes(ELEM** lista) {
    // Primeiro guarda os incidentes no arquivo
    if (*lista != NULL) {
        guardarIncidentes(*lista, "incidentes.bin");
    }

    // Depois limpa a memória
    ELEM* atual = *lista;
    while (atual != NULL) {
        ELEM* proximo = atual->proximo;
        
        // Mostra o histórico
        if (atual->incidente.historico != NULL) {
            free(atual->incidente.historico);
        }
        
        // Mostra as ferramentas
        if (atual->incidente.ferramentas != NULL) {
            free(atual->incidente.ferramentas);
        }
        
        // Mostra o elemento
        free(atual);
        
        atual = proximo;
    }
    
    // Reposiciona o ponteiro da lista
    *lista = NULL;
}

// Função para trocar dois incidentes na lista
void trocarIncidentes(ELEM* a, ELEM* b) {
    Incidente temp = a->incidente;
    a->incidente = b->incidente;
    b->incidente = temp;
}

// Função para ordenar incidentes por ID
void ordenarPorID(ELEM* inicio) {
    ELEM* i, *j;
    for (i = inicio; i != NULL; i = i->proximo) {
        for (j = i->proximo; j != NULL; j = j->proximo) {
            if (i->incidente.id > j->incidente.id) {
                trocarIncidentes(i, j);
            }
        }
    }
}

// Função para ordenar incidentes por data de criação
void ordenarPorDataCriacao(ELEM* inicio) {
    ELEM* i, *j;
    for (i = inicio; i != NULL; i = i->proximo) {
        for (j = i->proximo; j != NULL; j = j->proximo) {
            if (i->incidente.data_hora > j->incidente.data_hora) {
                trocarIncidentes(i, j);
            }
        }
    }
}

// Função para ordenar incidentes por estado
void ordenarPorEstado(ELEM* inicio) {
    ELEM* i, *j;
    for (i = inicio; i != NULL; i = i->proximo) {
        for (j = i->proximo; j != NULL; j = j->proximo) {
            if (i->incidente.estado > j->incidente.estado) {
                trocarIncidentes(i, j);
            }
        }
    }
}

// Função para ordenar incidentes por técnico
void ordenarPorTecnico(ELEM* inicio) {
    ELEM* i, *j;
    for (i = inicio; i != NULL; i = i->proximo) {
        for (j = i->proximo; j != NULL; j = j->proximo) {
            if (strcmp(i->incidente.tecnico_responsavel, j->incidente.tecnico_responsavel) > 0) {
                trocarIncidentes(i, j);
            }
        }
    }
}

// Função para ordenar incidentes
void ordenarIncidentes(ELEM** lista) {
    if (*lista == NULL) {
        printf("\nNão existem incidentes para ordenar.\n");
        return;
    }

    int opcao;
    printf("\n=== Ordenar Incidentes ===\n");
    printf("1. Ordenar por ID\n");
    printf("2. Ordenar por Data de Criação\n");
    printf("3. Ordenar por Estado\n");
    printf("4. Ordenar por Técnico\n");
    printf("0. Voltar\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);
    limparBuffer();

    switch (opcao) {
        case 1:
            ordenarPorID(*lista);
            printf("\nIncidentes ordenados por ID.\n");
            break;
        case 2:
            ordenarPorDataCriacao(*lista);
            printf("\nIncidentes ordenados por Data de Criação.\n");
            break;
        case 3:
            ordenarPorEstado(*lista);
            printf("\nIncidentes ordenados por Estado.\n");
            break;
        case 4:
            ordenarPorTecnico(*lista);
            printf("\nIncidentes ordenados por Técnico.\n");
            break;
        case 0:
            return;
        default:
            printf("\nOpção inválida!\n");
            return;
    }

    // Guardar a lista ordenada no ficheiro
    guardarIncidentes(*lista, "incidentes.bin");
    printf("\nLista de incidentes atualizada e guardada.\n");
}

void adicionarResposta(ELEM* elem, const char* resposta, const char* autor, bool solucao) {
    if (elem == NULL) {
        printf("\nErro: Incidente não encontrado.\n");
        return;
    }

    RespostaIncidente* novas_respostas = realloc(elem->incidente.respostas,
                                               (elem->incidente.num_respostas + 1) * sizeof(RespostaIncidente));
    
    if (novas_respostas == NULL) {
        printf("\nErro: Não foi possível adicionar a resposta.\n");
        return;
    }

    elem->incidente.respostas = novas_respostas;
    RespostaIncidente* nova_resposta = &elem->incidente.respostas[elem->incidente.num_respostas];
    
    nova_resposta->data_hora = time(NULL);
    strncpy(nova_resposta->resposta, resposta, sizeof(nova_resposta->resposta) - 1);
    nova_resposta->resposta[sizeof(nova_resposta->resposta) - 1] = '\0';
    strncpy(nova_resposta->autor, autor, sizeof(nova_resposta->autor) - 1);
    nova_resposta->autor[sizeof(nova_resposta->autor) - 1] = '\0';
    nova_resposta->solucao = solucao;
    
    elem->incidente.num_respostas++;

    printf("\nResposta adicionada com sucesso.\n");
    registarLog(autor, "Adicionou resposta ao incidente");
}

void marcarRespostaComoSolucao(ELEM* elem, int indice_resposta) {
    if (elem == NULL || indice_resposta < 0 || indice_resposta >= elem->incidente.num_respostas) {
        printf("\nErro: Resposta inválida.\n");
        return;
    }

    // Verificar se o autor tem permissão para marcar como solução
    USERS* current_user = verificarSessaoAtiva();
    if (current_user == NULL) {
        printf("\nErro: Sessão inválida.\n");
        return;
    }

    bool tem_permissao = false;
    if (current_user->tipoUser == ADMINISTRADOR) {
        tem_permissao = true;
    } else if (strcmp(current_user->username, elem->incidente.tecnico_responsavel) == 0) {
        tem_permissao = true;
    }

    if (!tem_permissao) {
        printf("\nErro: Apenas o técnico que criou o incidente ou um administrador podem marcar uma resposta como solução.\n");
        registarLog(current_user->username, "Tentativa de marcar resposta como solução sem permissão");
        return;
    }

    for (int i = 0; i < elem->incidente.num_respostas; i++) {
        elem->incidente.respostas[i].solucao = false;
    }

    elem->incidente.respostas[indice_resposta].solucao = true;
    atualizarEstadoIncidente(elem, RESOLVIDO);

    printf("\nResposta marcada como solução e incidente atualizado.\n");
    registarLog(current_user->username, "Marcou resposta como solução");
}

void listarRespostas(ELEM* elem) {
    if (elem == NULL || elem->incidente.num_respostas == 0) {
        printf("\nNão existem respostas para este incidente.\n");
        return;
    }

    printf("\n=== Respostas ao Incidente #%d ===\n", elem->incidente.id);
    for (int i = 0; i < elem->incidente.num_respostas; i++) {
        RespostaIncidente* resp = &elem->incidente.respostas[i];
        printf("\nResposta #%d:\n", i + 1);
        printf("Data/Hora: %s", ctime(&resp->data_hora));
        printf("Autor: %s\n", resp->autor);
        printf("Resposta: %s\n", resp->resposta);
        if (resp->solucao) {
            printf("✓ Solução aceite\n");
        }
        printf("------------------------\n");
    }
}

void menuRespostasIncidente(ELEM* elem) {
    if (elem == NULL) {
        printf("\nErro: Incidente não encontrado.\n");
        return;
    }

    USERS* current_user = verificarSessaoAtiva();
    if (current_user == NULL) {
        printf("\nErro: Sessão inválida.\n");
        return;
    }

    int opcao;
    char resposta[1000];
    int indice_resposta;

    do {
        printf("\n=== Gestão de Respostas ===\n");
        printf("1. Ver respostas\n");
        printf("2. Adicionar resposta\n");
        if (current_user->tipoUser == ADMINISTRADOR || 
            strcmp(current_user->username, elem->incidente.tecnico_responsavel) == 0) {
            printf("3. Marcar resposta como solução\n");
        }
        printf("0. Voltar\n");
        printf("Opção: ");
        scanf("%d", &opcao);
        while (getchar() != '\n');

        switch (opcao) {
            case 1:
                listarRespostas(elem);
                break;
            case 2:
                printf("\nEscreva a sua resposta: ");
                fgets(resposta, sizeof(resposta), stdin);
                resposta[strcspn(resposta, "\n")] = 0;
                adicionarResposta(elem, resposta, current_user->username, false);
                break;
            case 3:
                if (current_user->tipoUser == ADMINISTRADOR || 
                    strcmp(current_user->username, elem->incidente.tecnico_responsavel) == 0) {
                    listarRespostas(elem);
                    printf("\nIndique o número da resposta a marcar como solução: ");
                    scanf("%d", &indice_resposta);
                    while (getchar() != '\n');
                    marcarRespostaComoSolucao(elem, indice_resposta - 1);
                }
                break;
            case 0:
                return;
            default:
                printf("\nOpção inválida!\n");
        }
    } while (opcao != 0);
}
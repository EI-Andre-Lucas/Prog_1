#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "incidentes.h"

Incidente* criarIncidente(int id, TipoIncidente tipo, const char* descricao, Severidade severidade) {
    Incidente* novo = (Incidente*)malloc(sizeof(Incidente));
    if (novo == NULL) return NULL;

    novo->id = id;
    novo->tipo = tipo;
    novo->data_hora = time(NULL);
    strncpy(novo->descricao, descricao, sizeof(novo->descricao) - 1);
    novo->descricao[sizeof(novo->descricao) - 1] = '\0';
    novo->severidade = severidade;
    novo->estado = POR_TRATAR;
    novo->tecnico_responsavel[0] = '\0';
    novo->tempo_estimado = 0;
    novo->tempo_real = 0;
    novo->historico = NULL;
    novo->num_acoes = 0;
    novo->ferramentas = NULL;
    novo->num_ferramentas = 0;
    novo->proximo = NULL;

    return novo;
}

void adicionarIncidente(Incidente** lista, Incidente* novo) {
    if (*lista == NULL) {
        *lista = novo;
        return;
    }

    Incidente* atual = *lista;
    while (atual->proximo != NULL) {
        atual = atual->proximo;
    }
    atual->proximo = novo;
}

void removerIncidente(Incidente** lista, int id) {
    if (*lista == NULL) return;

    Incidente* atual = *lista;
    Incidente* anterior = NULL;

    while (atual != NULL && atual->id != id) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual == NULL) return;

    if (anterior == NULL) {
        *lista = atual->proximo;
    } else {
        anterior->proximo = atual->proximo;
    }

    free(atual->historico);
    free(atual->ferramentas);
    free(atual);
}

Incidente* procurarIncidente(Incidente* lista, int id) {
    Incidente* atual = lista;
    while (atual != NULL) {
        if (atual->id == id) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL;
}

void atualizarEstadoIncidente(Incidente* incidente, EstadoIncidente novo_estado) {
    if (incidente == NULL) return;
    incidente->estado = novo_estado;
    
    if (novo_estado == RESOLVIDO) {
        incidente->tempo_real = time(NULL) - incidente->data_hora;
    }
}

void adicionarAcaoHistorico(Incidente* incidente, const char* descricao, const char* tecnico) {
    if (incidente == NULL) return;

    HistoricoAcao* novo_historico = realloc(incidente->historico, 
                                          (incidente->num_acoes + 1) * sizeof(HistoricoAcao));
    
    if (novo_historico == NULL) return;

    incidente->historico = novo_historico;
    HistoricoAcao* nova_acao = &incidente->historico[incidente->num_acoes];
    
    nova_acao->data_hora = time(NULL);
    strncpy(nova_acao->descricao, descricao, sizeof(nova_acao->descricao) - 1);
    nova_acao->descricao[sizeof(nova_acao->descricao) - 1] = '\0';
    strncpy(nova_acao->tecnico, tecnico, sizeof(nova_acao->tecnico) - 1);
    nova_acao->tecnico[sizeof(nova_acao->tecnico) - 1] = '\0';
    
    incidente->num_acoes++;
}

void adicionarFerramenta(Incidente* incidente, const char* nome) {
    if (incidente == NULL) return;

    Ferramenta* novas_ferramentas = realloc(incidente->ferramentas,
                                          (incidente->num_ferramentas + 1) * sizeof(Ferramenta));
    
    if (novas_ferramentas == NULL) return;

    incidente->ferramentas = novas_ferramentas;
    Ferramenta* nova_ferramenta = &incidente->ferramentas[incidente->num_ferramentas];
    
    strncpy(nova_ferramenta->nome, nome, sizeof(nova_ferramenta->nome) - 1);
    nova_ferramenta->nome[sizeof(nova_ferramenta->nome) - 1] = '\0';
    nova_ferramenta->data_uso = time(NULL);
    
    incidente->num_ferramentas++;
}

void designarIncidente(Incidente* incidente, const char* novo_tecnico, const char* motivo) {
    if (incidente == NULL) return;

    char descricao[500];
    snprintf(descricao, sizeof(descricao), "Incidente delegado para %s. Motivo: %s", 
             novo_tecnico, motivo);
    
    adicionarAcaoHistorico(incidente, descricao, incidente->tecnico_responsavel);
    strncpy(incidente->tecnico_responsavel, novo_tecnico, sizeof(incidente->tecnico_responsavel) - 1);
    incidente->tecnico_responsavel[sizeof(incidente->tecnico_responsavel) - 1] = '\0';
}

void guardarIncidentes(Incidente* lista, const char* ficheiro) {
    FILE* file = fopen(ficheiro, "wb");
    if (file == NULL) return;

    Incidente* atual = lista;
    while (atual != NULL) {
        fwrite(atual, sizeof(Incidente), 1, file);
        
        // Guardar histórico
        fwrite(&atual->num_acoes, sizeof(int), 1, file);
        if (atual->num_acoes > 0) {
            fwrite(atual->historico, sizeof(HistoricoAcao), atual->num_acoes, file);
        }
        
        // Guardar ferramentas
        fwrite(&atual->num_ferramentas, sizeof(int), 1, file);
        if (atual->num_ferramentas > 0) {
            fwrite(atual->ferramentas, sizeof(Ferramenta), atual->num_ferramentas, file);
        }
        
        atual = atual->proximo;
    }

    fclose(file);
}

Incidente* carregarIncidentes(const char* ficheiro) {
    FILE* file = fopen(ficheiro, "rb");
    if (file == NULL) return NULL;

    Incidente* lista = NULL;
    Incidente* atual = NULL;

    while (!feof(file)) {
        Incidente* novo = (Incidente*)malloc(sizeof(Incidente));
        if (novo == NULL) break;

        if (fread(novo, sizeof(Incidente), 1, file) != 1) {
            free(novo);
            break;
        }

        // Carregar histórico
        fread(&novo->num_acoes, sizeof(int), 1, file);
        if (novo->num_acoes > 0) {
            novo->historico = malloc(novo->num_acoes * sizeof(HistoricoAcao));
            fread(novo->historico, sizeof(HistoricoAcao), novo->num_acoes, file);
        }

        // Carregar ferramentas
        fread(&novo->num_ferramentas, sizeof(int), 1, file);
        if (novo->num_ferramentas > 0) {
            novo->ferramentas = malloc(novo->num_ferramentas * sizeof(Ferramenta));
            fread(novo->ferramentas, sizeof(Ferramenta), novo->num_ferramentas, file);
        }

        novo->proximo = NULL;

        if (lista == NULL) {
            lista = novo;
            atual = novo;
        } else {
            atual->proximo = novo;
            atual = novo;
        }
    }

    fclose(file);
    return lista;
}

void criarRelatorio(Incidente* lista, const char* ficheiro, time_t inicio, time_t fim) {
    FILE* file = fopen(ficheiro, "w");
    if (file == NULL) return;

    fprintf(file, "Relatório de Incidentes\n");
    fprintf(file, "Período: %s a %s\n\n", ctime(&inicio), ctime(&fim));

    Incidente* atual = lista;
    while (atual != NULL) {
        if (atual->data_hora >= inicio && atual->data_hora <= fim) {
            fprintf(file, "ID: %d\n", atual->id);
            fprintf(file, "Tipo: %d\n", atual->tipo);
            fprintf(file, "Data/Hora: %s", ctime(&atual->data_hora));
            fprintf(file, "Descrição: %s\n", atual->descricao);
            fprintf(file, "Severidade: %d\n", atual->severidade);
            fprintf(file, "Estado: %d\n", atual->estado);
            fprintf(file, "Técnico: %s\n", atual->tecnico_responsavel);
            fprintf(file, "Tempo de Resolução: %ld segundos\n\n", atual->tempo_real);
        }
        atual = atual->proximo;
    }

    fclose(file);
}

void fornecerListaIncidentes(Incidente* lista) {
    while (lista != NULL) {
        Incidente* temp = lista;
        lista = lista->proximo;
        free(temp->historico);
        free(temp->ferramentas);
        free(temp);
    }
}
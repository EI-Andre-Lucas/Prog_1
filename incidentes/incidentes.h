#ifndef INCIDENTES_H
#define INCIDENTES_H

#include <time.h>
#include <stdbool.h>
#include "../utils/utils.h"

// Enumerações
typedef enum {
    PHISHING,
    MALWARE,
    ACESSO_NAO_AUTORIZADO,
    OUTRO
} TipoIncidente;

typedef enum {
    POR_TRATAR,
    EM_ANALISE,
    RESOLVIDO
} EstadoIncidente;

typedef enum {
    BAIXA,
    MEDIA,
    ALTA,
    CRITICA
} Severidade;

// Estrutura para histórico de ações
typedef struct {
    time_t data_hora;
    char descricao[500];
    char tecnico[50];
} HistoricoAcao;

// Estrutura para ferramentas utilizadas
typedef struct {
    char nome[100];
    time_t data_uso;
} Ferramenta;

// Estrutura principal do incidente
typedef struct Incidente {
    int id;
    TipoIncidente tipo;
    time_t data_hora;
    char descricao[500];
    Severidade severidade;
    EstadoIncidente estado;
    char tecnico_responsavel[50];
    time_t tempo_estimado;
    time_t tempo_real;
    HistoricoAcao* historico;
    int num_acoes;
    Ferramenta* ferramentas;
    int num_ferramentas;
} Incidente;

// Estrutura para lista duplamente ligada
typedef struct Elem {
    Incidente incidente;
    struct Elem* proximo;
    struct Elem* anterior;
} ELEM;

// Funções para gestão de incidentes
bool verificarIncidentesExistentes(ELEM* lista);
ELEM* criarIncidente(TipoIncidente tipo, const char* descricao, Severidade severidade);
void adicionarIncidente(ELEM** lista, ELEM* novo);
void removerIncidente(ELEM** lista, int id);
ELEM* procurarIncidente(ELEM* lista, int id);
void atualizarEstadoIncidente(ELEM* elem, EstadoIncidente novo_estado);
void adicionarAcaoHistorico(ELEM* elem, const char* descricao, const char* tecnico);
void adicionarFerramenta(ELEM* elem, const char* nome);
void designarIncidente(ELEM* elem, const char* novo_tecnico, const char* motivo);
void guardarIncidentes(ELEM* lista, const char* ficheiro);
ELEM* carregarIncidentes(const char* ficheiro);
void criarRelatorio(ELEM* lista, const char* ficheiro, time_t inicio, time_t fim);
void fornecerListaIncidentes(ELEM* lista);
void limparListaIncidentes(ELEM** lista);

#endif // INCIDENTES_H

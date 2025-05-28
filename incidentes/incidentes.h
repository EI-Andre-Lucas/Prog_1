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
    struct Incidente* proximo;
} Incidente;

// Funções para gestão de incidentes
bool verificarIncidentesExistentes(Incidente* lista);
Incidente* criarIncidente(int id, TipoIncidente tipo, const char* descricao, Severidade severidade);
void adicionarIncidente(Incidente** lista, Incidente* novo);
void removerIncidente(Incidente** lista, int id);
Incidente* procurarIncidente(Incidente* lista, int id);
void atualizarEstadoIncidente(Incidente* incidente, EstadoIncidente novo_estado);
void adicionarAcaoHistorico(Incidente* incidente, const char* descricao, const char* tecnico);
void adicionarFerramenta(Incidente* incidente, const char* nome);
void designarIncidente(Incidente* incidente, const char* novo_tecnico, const char* motivo);
void guardarIncidentes(Incidente* lista, const char* ficheiro);
Incidente* carregarIncidentes(const char* ficheiro);
void criarRelatorio(Incidente* lista, const char* ficheiro, time_t inicio, time_t fim);
void fornecerListaIncidentes(Incidente* lista);
void limparListaIncidentes(Incidente** lista);

#endif // INCIDENTES_H

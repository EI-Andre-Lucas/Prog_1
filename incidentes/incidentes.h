#ifndef INCIDENTES_H
#define INCIDENTES_H

#include <time.h>
#include <stdbool.h>
#include "../utils/utils.h"

// Enumerações
typedef enum {
    PHISHING = 1,
    MALWARE = 2,
    ACESSO_NAO_AUTORIZADO = 3,
    OUTRO = 4
} TipoIncidente;

// Enum para os estados do incidente
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
    int id;
    time_t data_hora;
    char descricao[500];
    char tecnico[50];
} HistoricoAcao;

// Estrutura para ferramentas utilizadas
typedef struct {
    int id;
    char nome[100];
    time_t data_uso;
} Ferramenta;

// Estrutura para respostas aos incidentes
typedef struct {
    int id;
    time_t data_hora;
    char resposta[1000];
    char autor[50];
    bool solucao;  // Indica se esta resposta resolveu o incidente
} RespostaIncidente;

// Estrutura principal do incidente
typedef struct Incidente {
    int id;
    TipoIncidente tipo;
    char descricao[100];
    int tempo_estimado;
    Severidade severidade;
    time_t data_hora;
    EstadoIncidente estado;
    char tecnico_responsavel[50];
    int tempo_real;
    HistoricoAcao* historico;
    Ferramenta* ferramentas;
    RespostaIncidente* respostas;
    int num_acoes;
    int num_ferramentas;
    int num_respostas;
} Incidente;

// Estrutura para lista duplamente ligada
typedef struct Elem {
    Incidente incidente;
    struct Elem* proximo;
    struct Elem* anterior;
} ELEM;

// Funções para gestão de incidentes
bool verificarIncidentesExistentes(ELEM* lista);
ELEM* criarIncidente(TipoIncidente tipo, const char* descricao, int tempo_estimado, Severidade severidade);
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

// Funções de ordenação
void ordenarIncidentes(ELEM** lista);
void ordenarPorID(ELEM* inicio);
void ordenarPorDataCriacao(ELEM* inicio);
void ordenarPorEstado(ELEM* inicio);
void ordenarPorTecnico(ELEM* inicio);
void trocarIncidentes(ELEM* a, ELEM* b);

// Funções para gestão de respostas
void adicionarResposta(ELEM* elem, const char* resposta, const char* autor, bool solucao);
void listarRespostas(ELEM* elem);
void marcarRespostaComoSolucao(ELEM* elem, int indice_resposta);

// Funções de relatório
void gerarRelatorioEstatistico(ELEM* lista, const char* ficheiro, time_t inicio, time_t fim);
void gerarRelatorioPeriodico(ELEM* lista);

#endif // INCIDENTES_H

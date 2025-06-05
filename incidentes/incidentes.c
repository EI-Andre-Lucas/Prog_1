#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "incidentes.h"
#include "../utils/utils.h"
#include "../logs/logs.h"
#include "../users/users.h"

// Variável global para a lista de incidentes
extern ELEM* lista_incidentes;

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

ELEM* criarIncidente(TipoIncidente tipo, const char* descricao, int tempo_estimado, Severidade severidade) {
    ELEM* novo = (ELEM*)malloc(sizeof(ELEM));
    if (!novo) return NULL;

    // Inicializa a estrutura com zeros
    memset(novo, 0, sizeof(ELEM));

    // Aloca memória para os arrays
    novo->incidente.historico = (HistoricoAcao*)calloc(100, sizeof(HistoricoAcao));
    novo->incidente.ferramentas = (Ferramenta*)calloc(50, sizeof(Ferramenta));
    novo->incidente.respostas = (RespostaIncidente*)calloc(50, sizeof(RespostaIncidente));

    if (!novo->incidente.historico || !novo->incidente.ferramentas || !novo->incidente.respostas) {
        free(novo->incidente.historico);
        free(novo->incidente.ferramentas);
        free(novo->incidente.respostas);
        free(novo);
        return NULL;
    }

    // Carrega a lista atual para obter o último ID
    if (!lista_incidentes) {
        lista_incidentes = carregarIncidentes("incidentes.bin");
    }

    // Encontrar o maior ID existente
    int max_id = 0;
    ELEM* atual = lista_incidentes;
    while (atual) {
        if (atual->incidente.id > max_id) {
            max_id = atual->incidente.id;
        }
        atual = atual->proximo;
    }
    
    // O novo ID será o maior ID + 1
    novo->incidente.id = max_id + 1;

    novo->incidente.tipo = tipo;
    strncpy(novo->incidente.descricao, descricao, sizeof(novo->incidente.descricao) - 1);
    novo->incidente.tempo_estimado = tempo_estimado;
    novo->incidente.severidade = severidade;
    novo->incidente.data_hora = time(NULL);
    novo->incidente.estado = POR_TRATAR;
    novo->incidente.num_acoes = 0;
    novo->incidente.num_ferramentas = 0;
    novo->incidente.num_respostas = 0;
    novo->proximo = NULL;

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
        printf("\nErro: Lista de incidentes vazia!\n");
        registarLog("SISTEMA", "Tentativa de remover incidente falhou - lista vazia");
        return;
    }

    ELEM* atual = *lista;
    while (atual != NULL && atual->incidente.id != id) {
        atual = atual->proximo;
    }

    if (atual == NULL) {
        printf("Erro: Incidente #%d não encontrado!\n", id);
        registarLog("SISTEMA", "Tentativa de remover incidente falhou - incidente não encontrado");
        return;
    }

    // Liberar memória dos arrays
    if (atual->incidente.historico) {
        free(atual->incidente.historico);
    }
    if (atual->incidente.ferramentas) {
        free(atual->incidente.ferramentas);
    }
    if (atual->incidente.respostas) {
        free(atual->incidente.respostas);
    }

    // Atualizar ponteiros da lista
    if (atual->anterior == NULL) {
        // É o primeiro elemento
        *lista = atual->proximo;
        if (*lista != NULL) {
            (*lista)->anterior = NULL;
        }
    } else {
        // Não é o primeiro elemento
        atual->anterior->proximo = atual->proximo;
        if (atual->proximo != NULL) {
            atual->proximo->anterior = atual->anterior;
        }
    }

    // Registrar a ação no log
    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        char log_message[150];
        snprintf(log_message, sizeof(log_message), "Removeu o incidente #%d", id);
        registarLog(current_user->username, log_message);
    }

    // Liberar o elemento
    free(atual);
    printf("Incidente #%d removido com sucesso!\n", id);

    // Salvar alterações no arquivo
    guardarIncidentes(*lista, "incidentes.bin");
    printf("Alterações salvas no arquivo.\n");
}

ELEM* procurarIncidente(ELEM* lista, int id) {
    if (!lista) {
        printf("ERRO: Lista de incidentes está vazia!\n");
        return NULL;
    }

    ELEM* atual = lista;
    int count = 0;
    while (atual) {
        if (atual->incidente.id == id) {
            return atual;
        }
        atual = atual->proximo;
        count++;
    }
    
    printf("ERRO: Incidente #%d não encontrado na lista (total de %d incidentes)\n", id, count);
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
    if (!elem || !descricao || !tecnico) {
        printf("\nErro: Parâmetros inválidos!\n");
        return;
    }

    // Verificar se o array de histórico existe
    if (!elem->incidente.historico) {
        elem->incidente.historico = (HistoricoAcao*)calloc(100, sizeof(HistoricoAcao));
        if (!elem->incidente.historico) {
            printf("\nErro ao alocar memória para histórico!\n");
            return;
        }
    }

    if (elem->incidente.num_acoes >= 100) {
        printf("\nNúmero máximo de ações atingido!\n");
        return;
    }

    // Adiciona a nova ação
    elem->incidente.historico[elem->incidente.num_acoes].id = elem->incidente.num_acoes + 1;
    elem->incidente.historico[elem->incidente.num_acoes].data_hora = time(NULL);
    
    // Usar strncpy com tamanho máximo e garantir que termina com \0
    strncpy(elem->incidente.historico[elem->incidente.num_acoes].descricao, descricao, sizeof(elem->incidente.historico[elem->incidente.num_acoes].descricao) - 1);
    elem->incidente.historico[elem->incidente.num_acoes].descricao[sizeof(elem->incidente.historico[elem->incidente.num_acoes].descricao) - 1] = '\0';
    
    strncpy(elem->incidente.historico[elem->incidente.num_acoes].tecnico, tecnico, sizeof(elem->incidente.historico[elem->incidente.num_acoes].tecnico) - 1);
    elem->incidente.historico[elem->incidente.num_acoes].tecnico[sizeof(elem->incidente.historico[elem->incidente.num_acoes].tecnico) - 1] = '\0';
    
    elem->incidente.num_acoes++;
    
    // Salvar alterações no arquivo
    guardarIncidentes(lista_incidentes, "incidentes.bin");
    
    printf("\nAção adicionada com sucesso!\n");
}

void adicionarFerramenta(ELEM* elem, const char* nome) {
    if (elem == NULL || nome == NULL) {
        printf("Erro: Parâmetros inválidos!\n");
        return;
    }

    if (elem->incidente.num_ferramentas >= 50) {
        printf("Número máximo de ferramentas atingido!\n");
        return;
    }

    // Alocar memória para o array de ferramentas se ainda não existir
    if (elem->incidente.ferramentas == NULL) {
        elem->incidente.ferramentas = (Ferramenta*)malloc(50 * sizeof(Ferramenta));
        if (elem->incidente.ferramentas == NULL) {
            printf("Erro ao alocar memória para ferramentas!\n");
            return;
        }
    }

    // Criar nova ferramenta
    Ferramenta nova_ferramenta;
    nova_ferramenta.id = elem->incidente.num_ferramentas + 1;
    strncpy(nova_ferramenta.nome, nome, sizeof(nova_ferramenta.nome) - 1);
    nova_ferramenta.nome[sizeof(nova_ferramenta.nome) - 1] = '\0';
    nova_ferramenta.data_uso = time(NULL);

    // Adicionar ao array
    elem->incidente.ferramentas[elem->incidente.num_ferramentas] = nova_ferramenta;
    elem->incidente.num_ferramentas++;

    printf("Ferramenta '%s' adicionada com sucesso!\n", nome);
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
    FILE* f = fopen(ficheiro, "wb");
    if (!f) {
        printf("Erro ao abrir ficheiro de incidentes para escrita\n");
        return;
    }

    // Contar número de incidentes
    int num_incidentes = 0;
    ELEM* atual = lista;
    while (atual) {
        num_incidentes++;
        atual = atual->proximo;
    }

    // Escrever número de incidentes
    fwrite(&num_incidentes, sizeof(int), 1, f);

    // Escrever cada incidente
    atual = lista;
    while (atual) {
        // Escrever dados básicos do incidente
        fwrite(&atual->incidente.id, sizeof(int), 1, f);
        fwrite(&atual->incidente.tipo, sizeof(TipoIncidente), 1, f);
        fwrite(atual->incidente.descricao, sizeof(char), 100, f);
        fwrite(&atual->incidente.tempo_estimado, sizeof(int), 1, f);
        fwrite(&atual->incidente.severidade, sizeof(Severidade), 1, f);
        fwrite(&atual->incidente.data_hora, sizeof(time_t), 1, f);
        fwrite(&atual->incidente.estado, sizeof(EstadoIncidente), 1, f);
        fwrite(atual->incidente.tecnico_responsavel, sizeof(char), 50, f);
        fwrite(&atual->incidente.tempo_real, sizeof(int), 1, f);

        // Escrever número de elementos em cada array
        fwrite(&atual->incidente.num_acoes, sizeof(int), 1, f);
        fwrite(&atual->incidente.num_ferramentas, sizeof(int), 1, f);
        fwrite(&atual->incidente.num_respostas, sizeof(int), 1, f);

        // Escrever arrays
        fwrite(atual->incidente.historico, sizeof(HistoricoAcao), atual->incidente.num_acoes, f);
        fwrite(atual->incidente.ferramentas, sizeof(Ferramenta), atual->incidente.num_ferramentas, f);
        fwrite(atual->incidente.respostas, sizeof(RespostaIncidente), atual->incidente.num_respostas, f);

        atual = atual->proximo;
    }

    fclose(f);
}

ELEM* carregarIncidentes(const char* ficheiro) {
    FILE* f = fopen(ficheiro, "rb");
    if (!f) {
        printf("Ficheiro de incidentes não encontrado ou vazio\n");
        return NULL;
    }

    ELEM* lista = NULL;
    int num_incidentes;

    // Ler número de incidentes
    if (fread(&num_incidentes, sizeof(int), 1, f) != 1) {
        fclose(f);
        return NULL;
    }

    // Ler cada incidente
    for (int i = 0; i < num_incidentes; i++) {
        ELEM* novo = (ELEM*)malloc(sizeof(ELEM));
        if (!novo) {
            fclose(f);
            return lista;
        }

        // Ler dados básicos do incidente
        fread(&novo->incidente.id, sizeof(int), 1, f);
        fread(&novo->incidente.tipo, sizeof(TipoIncidente), 1, f);
        fread(novo->incidente.descricao, sizeof(char), 100, f);
        fread(&novo->incidente.tempo_estimado, sizeof(int), 1, f);
        fread(&novo->incidente.severidade, sizeof(Severidade), 1, f);
        fread(&novo->incidente.data_hora, sizeof(time_t), 1, f);
        fread(&novo->incidente.estado, sizeof(EstadoIncidente), 1, f);
        fread(novo->incidente.tecnico_responsavel, sizeof(char), 50, f);
        fread(&novo->incidente.tempo_real, sizeof(int), 1, f);

        // Ler número de elementos em cada array
        fread(&novo->incidente.num_acoes, sizeof(int), 1, f);
        fread(&novo->incidente.num_ferramentas, sizeof(int), 1, f);
        fread(&novo->incidente.num_respostas, sizeof(int), 1, f);

        // Alocar memória para os arrays
        novo->incidente.historico = (HistoricoAcao*)malloc(100 * sizeof(HistoricoAcao));
        novo->incidente.ferramentas = (Ferramenta*)malloc(50 * sizeof(Ferramenta));
        novo->incidente.respostas = (RespostaIncidente*)malloc(50 * sizeof(RespostaIncidente));

        if (!novo->incidente.historico || !novo->incidente.ferramentas || !novo->incidente.respostas) {
            free(novo->incidente.historico);
            free(novo->incidente.ferramentas);
            free(novo->incidente.respostas);
            free(novo);
            fclose(f);
            return lista;
        }

        // Ler arrays
        fread(novo->incidente.historico, sizeof(HistoricoAcao), novo->incidente.num_acoes, f);
        fread(novo->incidente.ferramentas, sizeof(Ferramenta), novo->incidente.num_ferramentas, f);
        fread(novo->incidente.respostas, sizeof(RespostaIncidente), novo->incidente.num_respostas, f);

        novo->proximo = NULL;
        novo->anterior = NULL;

        // Inserir na lista
        if (!lista) {
            lista = novo;
        } else {
            ELEM* atual = lista;
            while (atual->proximo) {
                atual = atual->proximo;
            }
            atual->proximo = novo;
            novo->anterior = atual;
        }
    }

    fclose(f);
    return lista;
}

void criarRelatorio(ELEM* lista, const char* ficheiro, time_t inicio, time_t fim) {
    FILE* file = fopen(ficheiro, "w");
    if (file == NULL) return;

    fprintf(file, "Relatorio de Incidentes\n");
    fprintf(file, "Periodo: %s a %s\n\n", ctime(&inicio), ctime(&fim));

    ELEM* atual = lista;
    while (atual != NULL) {
        if (atual->incidente.data_hora >= inicio && atual->incidente.data_hora <= fim) {
            fprintf(file, "ID: %d\n", atual->incidente.id);
            fprintf(file, "Tipo: %s\n", 
                    atual->incidente.tipo == PHISHING ? "Phishing" : 
                    atual->incidente.tipo == MALWARE ? "Malware" : 
                    atual->incidente.tipo == ACESSO_NAO_AUTORIZADO ? "Acesso Nao Autorizado" : "Outro");
            fprintf(file, "Data/Hora: %s", ctime(&atual->incidente.data_hora));
            fprintf(file, "Descricao: %s\n", atual->incidente.descricao);
            fprintf(file, "Severidade: %s\n", 
                    atual->incidente.severidade == BAIXA ? "Baixa" : 
                    atual->incidente.severidade == MEDIA ? "Media" : "Alta");
            fprintf(file, "Estado: %s\n", 
                    atual->incidente.estado == POR_TRATAR ? "Por Tratar" : 
                    atual->incidente.estado == EM_ANALISE ? "Em Analise" : "Resolvido");
            fprintf(file, "Tecnico: %s\n", atual->incidente.tecnico_responsavel);
            if (atual->incidente.estado == RESOLVIDO) {
                fprintf(file, "Tempo de Resolucao: %ld segundos\n", atual->incidente.tempo_real);
            }

            // Histórico de ações
            fprintf(file, "\nHistorico de Acoes:\n");
            for (int i = 0; i < atual->incidente.num_acoes; i++) {
                char acao_data[20];
                strftime(acao_data, sizeof(acao_data), "%d/%m/%Y %H:%M", 
                        localtime(&atual->incidente.historico[i].data_hora));
                fprintf(file, "%d. [%s] %s - %s\n", 
                       i + 1,
                       acao_data,
                       atual->incidente.historico[i].tecnico,
                       atual->incidente.historico[i].descricao);
            }

            // Ferramentas utilizadas
            fprintf(file, "\nFerramentas Utilizadas:\n");
            for (int i = 0; i < atual->incidente.num_ferramentas; i++) {
                fprintf(file, "%d. %s\n", i + 1, atual->incidente.ferramentas[i].nome);
            }

            // Respostas/comentários
            fprintf(file, "\nRespostas/Comentarios:\n");
            for (int i = 0; i < atual->incidente.num_respostas; i++) {
                char resposta_data[20];
                strftime(resposta_data, sizeof(resposta_data), "%d/%m/%Y %H:%M", 
                        localtime(&atual->incidente.respostas[i].data_hora));
                fprintf(file, "%d. [%s] %s - %s\n", 
                       i + 1,
                       resposta_data,
                       atual->incidente.respostas[i].autor,
                       atual->incidente.respostas[i].resposta);
                if (atual->incidente.respostas[i].solucao) {
                    fprintf(file, "   [SOLUCAO]\n");
                }
            }
            fprintf(file, "\n----------------------------------------\n\n");
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
    if (!elem || !resposta || !autor) {
        printf("\nErro: Parâmetros inválidos!\n");
        return;
    }

    // Verificar se o array de respostas existe
    if (!elem->incidente.respostas) {
        elem->incidente.respostas = (RespostaIncidente*)calloc(50, sizeof(RespostaIncidente));
        if (!elem->incidente.respostas) {
            printf("\nErro ao alocar memória para respostas!\n");
            return;
        }
    }

    if (elem->incidente.num_respostas >= 50) {
        printf("\nNúmero máximo de respostas atingido!\n");
        return;
    }

    // Adiciona a nova resposta
    elem->incidente.respostas[elem->incidente.num_respostas].id = elem->incidente.num_respostas + 1;
    elem->incidente.respostas[elem->incidente.num_respostas].data_hora = time(NULL);
    strncpy(elem->incidente.respostas[elem->incidente.num_respostas].resposta, resposta, sizeof(elem->incidente.respostas[elem->incidente.num_respostas].resposta) - 1);
    strncpy(elem->incidente.respostas[elem->incidente.num_respostas].autor, autor, sizeof(elem->incidente.respostas[elem->incidente.num_respostas].autor) - 1);
    elem->incidente.respostas[elem->incidente.num_respostas].solucao = solucao;
    
    elem->incidente.num_respostas++;

    // Se a resposta resolve o incidente, atualiza o estado
    if (solucao) {
        elem->incidente.estado = RESOLVIDO;
        elem->incidente.tempo_real = time(NULL) - elem->incidente.data_hora;
    }
    printf("\nResposta adicionada com sucesso!\n");
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
    if (elem->incidente.num_respostas == 0) {
        printf("\nNenhuma resposta registrada.\n");
        return;
    }

    printf("\n=== Respostas do Incidente #%d ===\n", elem->incidente.id);
    for (int i = 0; i < elem->incidente.num_respostas; i++) {
        RespostaIncidente resp = elem->incidente.respostas[i];
        printf("\nID: %d\n", resp.id);
        printf("Autor: %s\n", resp.autor);
        printf("Data/Hora: %s", ctime(&resp.data_hora));
        printf("Resposta: %s\n", resp.resposta);
        printf("Solução: %s\n", resp.solucao ? "Sim" : "Não");
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

void gerarRelatorioEstatistico(ELEM* lista, const char* ficheiro, time_t inicio, time_t fim) {
    FILE* file = fopen(ficheiro, "w");
    if (file == NULL) {
        printf("\nErro ao criar o relatório.\n");
        return;
    }

    // Contadores para estatísticas
    int total_incidentes = 0;
    int incidentes_resolvidos = 0;
    int incidentes_por_tipo[3] = {0}; // 0: Hardware, 1: Software, 2: Rede
    int incidentes_por_severidade[3] = {0}; // 0: Baixa, 1: Média, 2: Alta
    int incidentes_por_estado[3] = {0}; // 0: Por Tratar, 1: Em Análise, 2: Resolvido
    time_t tempo_total_resolucao = 0;
    int num_incidentes_resolvidos = 0;

    // Cabeçalho do relatório
    fprintf(file, "=== Relatório de Incidentes ===\n");
    fprintf(file, "Período: %s a %s\n\n", ctime(&inicio), ctime(&fim));

    // Análise dos incidentes
    ELEM* atual = lista;
    while (atual != NULL) {
        if (atual->incidente.data_hora >= inicio && atual->incidente.data_hora <= fim) {
            total_incidentes++;
            
            // Contagem por tipo
            incidentes_por_tipo[atual->incidente.tipo]++;
            
            // Contagem por severidade
            incidentes_por_severidade[atual->incidente.severidade]++;
            
            // Contagem por estado
            incidentes_por_estado[atual->incidente.estado]++;
            
            // Estatísticas de resolução
            if (atual->incidente.estado == RESOLVIDO) {
                incidentes_resolvidos++;
                tempo_total_resolucao += atual->incidente.tempo_real;
                num_incidentes_resolvidos++;
            }
        }
        atual = atual->proximo;
    }

    // Cálculo de médias e percentuais
    float media_tempo_resolucao = num_incidentes_resolvidos > 0 ? 
        (float)tempo_total_resolucao / num_incidentes_resolvidos : 0;
    
    float percentual_resolvidos = total_incidentes > 0 ? 
        (float)incidentes_resolvidos / total_incidentes * 100 : 0;

    // Escrever estatísticas no relatório
    fprintf(file, "=== Estatísticas Gerais ===\n");
    fprintf(file, "Total de Incidentes: %d\n", total_incidentes);
    fprintf(file, "Incidentes Resolvidos: %d (%.2f%%)\n", 
            incidentes_resolvidos, percentual_resolvidos);
    fprintf(file, "Tempo Médio de Resolução: %.2f segundos\n\n", media_tempo_resolucao);

    // Estatísticas por Tipo
    fprintf(file, "=== Incidentes por Tipo ===\n");
    fprintf(file, "Hardware: %d (%.2f%%)\n", 
            incidentes_por_tipo[0], 
            total_incidentes > 0 ? (float)incidentes_por_tipo[0] / total_incidentes * 100 : 0);
    fprintf(file, "Software: %d (%.2f%%)\n", 
            incidentes_por_tipo[1], 
            total_incidentes > 0 ? (float)incidentes_por_tipo[1] / total_incidentes * 100 : 0);
    fprintf(file, "Rede: %d (%.2f%%)\n\n", 
            incidentes_por_tipo[2], 
            total_incidentes > 0 ? (float)incidentes_por_tipo[2] / total_incidentes * 100 : 0);

    // Estatísticas por Severidade
    fprintf(file, "=== Incidentes por Severidade ===\n");
    fprintf(file, "Baixa: %d (%.2f%%)\n", 
            incidentes_por_severidade[0], 
            total_incidentes > 0 ? (float)incidentes_por_severidade[0] / total_incidentes * 100 : 0);
    fprintf(file, "Média: %d (%.2f%%)\n", 
            incidentes_por_severidade[1], 
            total_incidentes > 0 ? (float)incidentes_por_severidade[1] / total_incidentes * 100 : 0);
    fprintf(file, "Alta: %d (%.2f%%)\n\n", 
            incidentes_por_severidade[2], 
            total_incidentes > 0 ? (float)incidentes_por_severidade[2] / total_incidentes * 100 : 0);

    // Estatísticas por Estado
    fprintf(file, "=== Incidentes por Estado ===\n");
    fprintf(file, "Por Tratar: %d (%.2f%%)\n", 
            incidentes_por_estado[0], 
            total_incidentes > 0 ? (float)incidentes_por_estado[0] / total_incidentes * 100 : 0);
    fprintf(file, "Em Análise: %d (%.2f%%)\n", 
            incidentes_por_estado[1], 
            total_incidentes > 0 ? (float)incidentes_por_estado[1] / total_incidentes * 100 : 0);
    fprintf(file, "Resolvidos: %d (%.2f%%)\n\n", 
            incidentes_por_estado[2], 
            total_incidentes > 0 ? (float)incidentes_por_estado[2] / total_incidentes * 100 : 0);

    // Lista detalhada dos incidentes
    fprintf(file, "=== Lista Detalhada de Incidentes ===\n");
    atual = lista;
    while (atual != NULL) {
        if (atual->incidente.data_hora >= inicio && atual->incidente.data_hora <= fim) {
            fprintf(file, "\nID: %d\n", atual->incidente.id);
            fprintf(file, "Tipo: %s\n", 
                    atual->incidente.tipo == PHISHING ? "Phishing" : 
                    atual->incidente.tipo == MALWARE ? "Malware" : 
                    atual->incidente.tipo == ACESSO_NAO_AUTORIZADO ? "Acesso Não Autorizado" : "Outro");
            fprintf(file, "Data/Hora: %s", ctime(&atual->incidente.data_hora));
            fprintf(file, "Descrição: %s\n", atual->incidente.descricao);
            fprintf(file, "Severidade: %s\n", 
                    atual->incidente.severidade == 0 ? "Baixa" : 
                    atual->incidente.severidade == 1 ? "Média" : "Alta");
            fprintf(file, "Estado: %s\n", 
                    atual->incidente.estado == 0 ? "Por Tratar" : 
                    atual->incidente.estado == 1 ? "Em Análise" : "Resolvido");
            fprintf(file, "Técnico: %s\n", atual->incidente.tecnico_responsavel);
            if (atual->incidente.estado == RESOLVIDO) {
                fprintf(file, "Tempo de Resolução: %ld segundos\n", atual->incidente.tempo_real);
            }
            fprintf(file, "------------------------\n");
        }
        atual = atual->proximo;
    }

    fclose(file);
    printf("\nRelatório gerado com sucesso em: %s\n", ficheiro);
}

void gerarRelatorioPeriodico(ELEM* lista) {
    // Verificar se o usuário atual é administrador
    USERS* current_user = verificarSessaoAtiva();
    if (current_user == NULL || current_user->tipoUser != ADMINISTRADOR) {
        printf("\nErro: Apenas administradores podem gerar relatórios.\n");
        registarLog(current_user ? current_user->username : "SISTEMA", 
                   "Tentativa de gerar relatório sem permissão");
        return;
    }

    int opcao;
    time_t agora = time(NULL);
    time_t inicio, fim;
    struct tm* tm_info;
    char nome_arquivo[100];

    printf("\n=== Gerar Relatório Periódico ===\n");
    printf("1. Relatório Semanal\n");
    printf("2. Relatório Mensal\n");
    printf("3. Relatório Personalizado\n");
    printf("0. Voltar\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);
    limparBuffer();

    switch (opcao) {
        case 1: // Semanal
            fim = agora;
            inicio = fim - (7 * 24 * 60 * 60); // 7 dias atrás
            strftime(nome_arquivo, sizeof(nome_arquivo), "relatorio_semanal_%Y%m%d.txt", localtime(&agora));
            break;

        case 2: // Mensal
            fim = agora;
            inicio = fim - (30 * 24 * 60 * 60); // 30 dias atrás
            strftime(nome_arquivo, sizeof(nome_arquivo), "relatorio_mensal_%Y%m%d.txt", localtime(&agora));
            break;

        case 3: // Personalizado
            printf("\nData de início (DD/MM/AAAA): ");
            char data_inicio[11];
            fgets(data_inicio, sizeof(data_inicio), stdin);
            data_inicio[strcspn(data_inicio, "\n")] = 0;

            printf("Data de fim (DD/MM/AAAA): ");
            char data_fim[11];
            fgets(data_fim, sizeof(data_fim), stdin);
            data_fim[strcspn(data_fim, "\n")] = 0;

            // Converter strings de data para time_t
            struct tm tm_inicio = {0};
            struct tm tm_fim = {0};
            
            sscanf(data_inicio, "%d/%d/%d", &tm_inicio.tm_mday, &tm_inicio.tm_mon, &tm_inicio.tm_year);
            tm_inicio.tm_mon--; // Ajustar mês (0-11)
            tm_inicio.tm_year -= 1900; // Ajustar ano
            
            sscanf(data_fim, "%d/%d/%d", &tm_fim.tm_mday, &tm_fim.tm_mon, &tm_fim.tm_year);
            tm_fim.tm_mon--; // Ajustar mês (0-11)
            tm_fim.tm_year -= 1900; // Ajustar ano

            inicio = mktime(&tm_inicio);
            fim = mktime(&tm_fim);

            strftime(nome_arquivo, sizeof(nome_arquivo), "relatorio_personalizado_%Y%m%d.txt", localtime(&agora));
            break;

        case 0:
            return;

        default:
            printf("\nOpção inválida!\n");
            return;
    }

    gerarRelatorioEstatistico(lista, nome_arquivo, inicio, fim);
}

void listarIncidentes(ELEM* lista) {
    if (!lista) {
        printf("\nNenhum incidente registrado.\n");
        return;
    }

    printf("\n=== LISTA DE INCIDENTES ===\n");
    printf("ID  | Tipo           | Data/Hora           | Descricao                    | Severidade | Estado\n");
    printf("----|----------------|---------------------|------------------------------|------------|--------\n");

    ELEM* atual = lista;
    while (atual) {
        char data_str[20];
        strftime(data_str, sizeof(data_str), "%d/%m/%Y %H:%M", localtime(&atual->incidente.data_hora));
        
        const char* estado_str;
        switch(atual->incidente.estado) {
            case POR_TRATAR: estado_str = "Por tratar"; break;
            case EM_ANALISE: estado_str = "Em analise"; break;
            case RESOLVIDO: estado_str = "Resolvido"; break;
            default: estado_str = "Desconhecido";
        }

        const char* tipo_str;
        switch(atual->incidente.tipo) {
            case PHISHING: tipo_str = "Phishing"; break;
            case MALWARE: tipo_str = "Malware"; break;
            case ACESSO_NAO_AUTORIZADO: tipo_str = "Acesso Nao Autorizado"; break;
            default: tipo_str = "Outro";
        }

        const char* severidade_str;
        switch(atual->incidente.severidade) {
            case BAIXA: severidade_str = "Baixa"; break;
            case MEDIA: severidade_str = "Media"; break;
            case ALTA: severidade_str = "Alta"; break;
            default: severidade_str = "Desconhecida";
        }

        printf("%-3d | %-14s | %-19s | %-28s | %-10s | %s\n",
               atual->incidente.id,
               tipo_str,
               data_str,
               atual->incidente.descricao,
               severidade_str,
               estado_str);
        
        atual = atual->proximo;
    }
    printf("==================================================================================\n");
}
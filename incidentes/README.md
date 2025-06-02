# Módulo de Gestão de Incidentes

## Descrição
Este módulo gere todas as operações relacionadas com incidentes técnicos, incluindo criação, atribuição, atualização e resolução de incidentes.

## Estruturas de Dados

### EstadoIncidente
```c
typedef enum {
    PENDENTE = 0,
    EM_ANALISE = 1,
    EM_RESOLUCAO = 2,
    RESOLVIDO = 3,
    FECHADO = 4
} EstadoIncidente;
```
Enumeração que define os possíveis estados de um incidente.

### Incidente
```c
typedef struct {
    int id;
    char titulo[100];
    char descricao[500];
    char localizacao[100];
    char tecnico[50];
    EstadoIncidente estado;
    time_t dataCriacao;
    time_t dataAtualizacao;
    char ferramentas[5][50];
    int numFerramentas;
} Incidente;
```
Estrutura que armazena os dados de um incidente.

### ELEM
```c
typedef struct elem {
    Incidente incidente;
    struct elem *prox;
    struct elem *ant;
} ELEM;
```
Estrutura que representa um elemento da lista duplamente ligada de incidentes.

## Funções Principais

### Gestão de Incidentes
- `void criarIncidente()` - Cria um novo incidente
- `void listarIncidentes()` - Lista todos os incidentes
- `void atualizarEstadoIncidente()` - Atualiza o estado de um incidente
- `void designarIncidente()` - Atribui um incidente a um técnico
- `void adicionarFerramenta()` - Adiciona uma ferramenta a um incidente
- `void removerFerramenta()` - Remove uma ferramenta de um incidente

### Funções Auxiliares
- `ELEM* carregarIncidentes(const char* filename)` - Carrega incidentes de um ficheiro
- `void guardarIncidentes(ELEM* lista, const char* filename)` - Guarda incidentes num ficheiro
- `void libertarListaIncidentes(ELEM* lista)` - Liberta a memória da lista de incidentes

## Ficheiros de Dados
- `incidentes.bin` - Ficheiro binário que armazena os dados dos incidentes

## Fluxo de Trabalho
1. Criação do incidente por um administrador
2. Atribuição a um técnico
3. Atualização do estado pelo técnico
4. Adição/remoção de ferramentas conforme necessário
5. Resolução e fecho do incidente

## Exemplo de Utilização

### Criação de Incidente
```c
void criarIncidente() {
    // Implementação da criação de incidente
}
```

### Atualização de Estado
```c
void atualizarEstadoIncidente() {
    // Implementação da atualização de estado
}
```

### Designação de Técnico
```c
void designarIncidente() {
    // Implementação da designação de técnico
}
```

## Notas de Implementação
- Utiliza lista duplamente ligada para gestão eficiente
- Persistência de dados em ficheiro binário
- Validação de permissões por tipo de utilizador
- Registo de todas as operações no sistema de logs 
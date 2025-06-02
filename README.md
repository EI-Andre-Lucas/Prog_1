# Sistema de Gestão de Incidentes

## Descrição
Este sistema permite a gestão de incidentes técnicos, incluindo a criação, atribuição e resolução de incidentes por técnicos e administradores.

## Funcionalidades Principais

### Gestão de Utilizadores
- Login e registo de utilizadores
- Diferentes níveis de acesso (Administrador e Técnico)
- Gestão de perfis de utilizadores
- Alteração de palavra-passe
- Listagem de utilizadores

### Gestão de Incidentes
- Criação de novos incidentes
- Atribuição de incidentes a técnicos
- Atualização do estado dos incidentes
- Listagem e pesquisa de incidentes
- Gestão de ferramentas e equipamentos

### Sistema de Logs
- Registo de todas as ações realizadas no sistema
- Histórico de acessos
- Rastreabilidade de operações

## Estrutura do Projeto

### Diretórios
- `users/` - Gestão de utilizadores
- `incidentes/` - Gestão de incidentes
- `logs/` - Sistema de registo de logs
- `menus/` - Interfaces de utilizador
- `utils/` - Funções utilitárias

### Ficheiros Principais
- `users/users.h` - Definições de estruturas e funções de utilizadores
- `users/users.c` - Implementação das funções de gestão de utilizadores
- `incidentes/incidentes.h` - Definições de estruturas e funções de incidentes
- `incidentes/incidentes.c` - Implementação das funções de gestão de incidentes
- `logs/logs.h` - Definições do sistema de logs
- `logs/logs.c` - Implementação do sistema de logs

## Módulos do Sistema

### 1. Módulo de Gestão de Utilizadores

#### Estruturas de Dados
```c
typedef enum {
    TECNICO = 0,
    ADMINISTRADOR = 1
} TipoUser;

typedef struct users {
    int id;
    char username[50];
    char password[50];
    char primeiroNome[50];
    char ultimoNome[50];
    TipoUser tipoUser;
} USERS;
```

#### Funções Principais
- `bool login()` - Realiza o processo de autenticação
- `void logout()` - Termina a sessão do utilizador
- `void registo(bool criar_admin)` - Regista um novo utilizador
- `void listarUtilizadores()` - Lista todos os utilizadores
- `void removerUtilizador(const char* username)` - Remove um utilizador
- `void modificarTipoUtilizador(const char* username, int novo_tipo)` - Altera o tipo de um utilizador
- `void alterarPasswordUtilizador(const char* username, const char* nova_senha)` - Altera a palavra-passe de um utilizador

### 2. Módulo de Gestão de Incidentes

#### Estruturas de Dados
```c
typedef enum {
    PENDENTE = 0,
    EM_ANALISE = 1,
    EM_RESOLUCAO = 2,
    RESOLVIDO = 3,
    FECHADO = 4
} EstadoIncidente;

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

typedef struct elem {
    Incidente incidente;
    struct elem *prox;
    struct elem *ant;
} ELEM;
```

#### Funções Principais
- `void criarIncidente()` - Cria um novo incidente
- `void listarIncidentes()` - Lista todos os incidentes
- `void atualizarEstadoIncidente()` - Atualiza o estado de um incidente
- `void designarIncidente()` - Atribui um incidente a um técnico
- `void adicionarFerramenta()` - Adiciona uma ferramenta a um incidente
- `void removerFerramenta()` - Remove uma ferramenta de um incidente

### 3. Módulo de Sistema de Logs

#### Estrutura de Dados
```c
typedef struct {
    char username[50];
    char acao[100];
    time_t timestamp;
} Log;
```

#### Funções Principais
- `void registrarLog(const char* username, const char* acao)` - Regista uma nova ação
- `void listarLogs()` - Lista todos os registos de log
- `void limparLogs()` - Limpa os registos de log antigos

### 4. Módulo de Interfaces de Utilizador

#### Funções Principais
- `void menuPrincipal()` - Menu inicial do sistema
- `void menu_login_reg(int opcao)` - Menu de login e registo
- `void menuAdmin()` - Menu específico para administradores
- `void menuTecnico()` - Menu específico para técnicos
- `void menuGestaoUtilizadores()` - Submenu de gestão de utilizadores
- `void menuGestaoIncidentes()` - Submenu de gestão de incidentes
- `void menuGestaoFerramentas()` - Submenu de gestão de ferramentas
- `void menuLogs()` - Submenu de visualização de logs

### 5. Módulo de Funções Utilitárias

#### Funções Principais
- `void lerPassword(char *password, int max_size)` - Lê uma palavra-passe de forma segura
- `void clickEnter()` - Aguarda o utilizador pressionar Enter
- `void limparTela()` - Limpa a tela do terminal
- `bool validarUsername(const char* username)` - Valida um nome de utilizador
- `bool validarPassword(const char* password)` - Valida uma palavra-passe
- `void formatarData(time_t timestamp, char* output)` - Formata uma data
- `void formatarHora(time_t timestamp, char* output)` - Formata uma hora

## Instalação e Execução

### Requisitos
- Compilador C (gcc recomendado)
- Sistema operativo Windows ou Unix-like

### Compilação
```bash
gcc -o sistema *.c
```

### Execução
```bash
./sistema
```

## Utilização

### Primeiro Acesso
1. O sistema cria automaticamente um utilizador administrador
2. Credenciais padrão:
   - Username: admin
   - Password: admin

### Gestão de Utilizadores
- Administradores podem criar novos utilizadores
- Administradores podem alterar tipos de utilizadores
- Todos os utilizadores podem alterar a sua palavra-passe

### Gestão de Incidentes
- Administradores podem criar e atribuir incidentes
- Técnicos podem atualizar o estado dos incidentes
- Todos podem visualizar incidentes

## Segurança
- Palavras-passe encriptadas usando função de hash
- Sessões de utilizador controladas
- Logs de todas as operações
- Verificação de permissões por tipo de utilizador

## Manutenção
- Ficheiros de dados em formato binário
- Backups automáticos
- Sistema de logs para auditoria

## Boas Práticas
- Manter a interface limpa e organizada
- Fornecer instruções claras
- Validar todas as entradas do utilizador
- Tratar erros de forma adequada
- Usar funções utilitárias em vez de código duplicado
- Manter consistência na validação de dados
- Documentar parâmetros e retornos

## Suporte
Para questões técnicas ou suporte, contacte a equipa de desenvolvimento.

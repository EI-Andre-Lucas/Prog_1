# Módulo Users

Este módulo é responsável pela gestão de utilizadores no sistema, incluindo login, registo e gestão de sessões.

## Funcionalidades Implementadas

- **Login**: Permite aos utilizadores iniciar sessão no sistema.
- **Registo**: Permite a criação de novos utilizadores, com opção para criar administradores.
- **Gestão de Sessões**: Controla as sessões ativas dos utilizadores.
- **Alteração de Senha**: Permite aos utilizadores alterar suas senhas.
- **Listagem de Utilizadores**: Lista todos os utilizadores registrados no sistema.

## Utilização

Este módulo é essencial para a segurança e gestão do sistema, garantindo que apenas utilizadores autorizados possam acessar as funcionalidades do sistema.

## Descrição
Este módulo gere todas as operações relacionadas com utilizadores do sistema, incluindo autenticação, registo e gestão de perfis.

## Estruturas de Dados

### TipoUser
```c
typedef enum {
    TECNICO = 0,
    ADMINISTRADOR = 1
} TipoUser;
```
Enumeração que define os tipos de utilizadores no sistema.

### USERS
```c
typedef struct users {
    int id;
    char username[50];
    char password[50];
    char primeiroNome[50];
    char ultimoNome[50];
    TipoUser tipoUser;
} USERS;
```
Estrutura que armazena os dados de um utilizador.

## Funções Principais

### Autenticação
- `bool login()` - Realiza o processo de autenticação
- `void logout()` - Termina a sessão do utilizador
- `bool guardarSessao(USERS *user)` - Guarda os dados da sessão atual
- `USERS* verificarSessaoAtiva()` - Verifica se existe uma sessão ativa

### Gestão de Utilizadores
- `void FirstUserCreator()` - Cria o utilizador administrador inicial
- `void registo(bool criar_admin)` - Regista um novo utilizador
- `void listarUtilizadores()` - Lista todos os utilizadores
- `void removerUtilizador(const char* username)` - Remove um utilizador
- `void modificarTipoUtilizador(const char* username, int novo_tipo)` - Altera o tipo de um utilizador
- `void alterarPasswordUtilizador(const char* username, const char* nova_senha)` - Altera a palavra-passe de um utilizador

### Funções Auxiliares
- `unsigned long hash_string(const char *str)` - Gera um hash para encriptação
- `void hash_to_string(const char *password, char *output)` - Converte o hash para string
- `USERS* procurarTecnico(const char* username)` - Procura um técnico pelo username

## Ficheiros de Dados
- `users.bin` - Ficheiro binário que armazena os dados dos utilizadores
- `sessao.dat` - Ficheiro temporário que armazena os dados da sessão atual

## Segurança
- Palavras-passe são armazenadas como hashes
- Verificação de permissões por tipo de utilizador
- Sessões são validadas em cada operação

## Exemplo de Utilização

### Login
```c
if (login()) {
    printf("Login realizado com sucesso!\n");
} else {
    printf("Login falhou!\n");
}
```

### Registo de Novo Utilizador
```c
registo(false); // false para técnico, true para administrador
```

### Alteração de Palavra-passe
```c
alterarPasswordUtilizador("username", "nova_senha");
```

## Notas de Implementação
- Todas as operações são registadas no sistema de logs
- Os dados são persistidos em ficheiros binários
- A estrutura de dados é otimizada para leitura/escrita eficiente 
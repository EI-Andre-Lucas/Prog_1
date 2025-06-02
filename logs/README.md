# Módulo de Sistema de Logs

## Descrição
Este módulo gere o registo de todas as operações realizadas no sistema, permitindo a auditoria e rastreabilidade das ações dos utilizadores.

## Estruturas de Dados

### Log
```c
typedef struct {
    char username[50];
    char acao[100];
    time_t timestamp;
} Log;
```
Estrutura que armazena os dados de um registo de log.

## Funções Principais

### Gestão de Logs
- `void registrarLog(const char* username, const char* acao)` - Regista uma nova ação no sistema de logs
- `void listarLogs()` - Lista todos os registos de log
- `void limparLogs()` - Limpa os registos de log antigos

## Ficheiros de Dados
- `logs.bin` - Ficheiro binário que armazena os registos de log

## Formato dos Logs
Cada registo de log contém:
- Username do utilizador
- Ação realizada
- Data e hora da ação

## Exemplo de Utilização

### Registo de uma Ação
```c
registarLog("admin", "Login realizado com sucesso");
```

### Listagem de Logs
```c
void listarLogs() {
    // Implementação da listagem de logs
}
```

## Notas de Implementação
- Os logs são armazenados em formato binário
- Cada ação é registada com timestamp
- O sistema mantém um histórico completo de operações
- Os logs são essenciais para auditoria e segurança

## Boas Práticas
- Registrar todas as operações críticas
- Incluir informações relevantes no campo de ação
- Manter os logs organizados por data
- Realizar limpeza periódica de logs antigos

## Segurança
- Os logs não podem ser modificados
- Apenas administradores podem aceder aos logs
- Os logs são essenciais para detetar atividades suspeitas 
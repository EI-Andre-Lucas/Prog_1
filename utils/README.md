# Módulo de Funções Utilitárias

## Descrição
Este módulo contém funções utilitárias utilizadas em todo o sistema, fornecendo funcionalidades comuns e auxiliares.

## Funções Principais

### Entrada de Dados
- `void lerPassword(char *password, int max_size)` - Lê uma palavra-passe de forma segura
- `void clickEnter()` - Aguarda o utilizador pressionar Enter
- `void limparTela()` - Limpa a tela do terminal

### Validação
- `bool validarUsername(const char* username)` - Valida um nome de utilizador
- `bool validarPassword(const char* password)` - Valida uma palavra-passe
- `bool validarEmail(const char* email)` - Valida um endereço de email

### Formatação
- `void formatarData(time_t timestamp, char* output)` - Formata uma data
- `void formatarHora(time_t timestamp, char* output)` - Formata uma hora
- `void formatarNome(char* nome)` - Formata um nome próprio

### Manipulação de Strings
- `void removerEspacos(char* str)` - Remove espaços extras de uma string
- `void converterMaiusculas(char* str)` - Converte uma string para maiúsculas
- `void converterMinusculas(char* str)` - Converte uma string para minúsculas

## Exemplo de Utilização

### Leitura Segura de Palavra-passe
```c
char password[50];
lerPassword(password, sizeof(password));
```

### Validação de Dados
```c
if (validarUsername(username)) {
    // Username válido
} else {
    // Username inválido
}
```

### Formatação de Data
```c
char data_formatada[20];
formatarData(timestamp, data_formatada);
```

## Notas de Implementação
- Funções reutilizáveis em todo o sistema
- Validações consistentes
- Tratamento de erros padronizado
- Formatação uniforme

## Boas Práticas
- Usar funções utilitárias em vez de código duplicado
- Manter consistência na validação de dados
- Documentar parâmetros e retornos
- Tratar erros de forma adequada

## Segurança
- Validação rigorosa de entradas
- Proteção contra buffer overflow
- Sanitização de dados
- Tratamento seguro de palavras-passe 
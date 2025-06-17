# Sistema de Gestão de Ciberincidentes e Respostas de Segurança

Este projeto é um sistema de gestão de ciberincidentes e respostas de segurança, desenvolvido em C. O sistema permite a gestão de incidentes, utilizadores, logs e relatórios.

## Estrutura do Projeto

O projeto está organizado em vários módulos, cada um responsável por uma parte específica do sistema:

- **main.c**: Arquivo principal que inicializa o sistema e gerencia o fluxo de execução.
- **utils/**: Contém funções utilitárias como limpar a tela, ler senhas, etc.
- **users/**: Gerencia os utilizadores do sistema, incluindo login, registo e gestão de sessões.
- **incidentes/**: Gerencia os incidentes, incluindo criação, atualização e remoção.
- **menus/**: Contém os menus do sistema, permitindo a navegação entre diferentes funcionalidades.
- **logs/**: Gerencia os logs do sistema, registrando ações dos utilizadores.
- **relatorios/**: Gera relatórios sobre incidentes e utilizadores.

## Implementação

### Utilizadores

- **Gestão de Utilizadores**: Implementado em `users.c`, permite criar, remover e modificar utilizadores. As senhas são armazenadas de forma segura usando hash.

### Incidentes

- **Gestão de Incidentes**: Implementado em `incidentes.c`, permite criar, atualizar e remover incidentes. Cada incidente possui um histórico de ações e ferramentas utilizadas.

### Logs

- **Registro de Logs**: Implementado em `logs.c`, registra todas as ações dos utilizadores, permitindo auditoria e rastreamento de atividades.

### Relatórios

- **Geração de Relatórios**: Implementado em `relatorios.c`, gera relatórios detalhados sobre incidentes e utilizadores, incluindo estatísticas e análises.

### Menus

- **Navegação**: Implementado em `menus.c`, fornece uma interface de usuário para navegar entre as diferentes funcionalidades do sistema.

## Como Executar

Para executar o sistema, compile o arquivo `main.c` e execute o binário gerado. Certifique-se de que todos os arquivos de cabeçalho e bibliotecas necessárias estão presentes.

```bash
gcc -o sistema main.c
./sistema
```

## Contribuição

Contribuições são bem-vindas! Sinta-se à vontade para abrir um pull request ou reportar problemas.

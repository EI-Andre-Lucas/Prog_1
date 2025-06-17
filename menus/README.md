# Módulo Menus

Este módulo é responsável pela interface de utilizador, fornecendo menus para navegação entre as diferentes funcionalidades do sistema.

## Funcionalidades Implementadas

- **Menu Principal**: Interface inicial do sistema, permitindo acesso a todas as funcionalidades.
- **Menu de Gestão de Incidentes**: Permite a gestão de incidentes, incluindo criação, atualização e remoção.
- **Menu de Gestão de Utilizadores**: Permite a gestão de utilizadores, incluindo login, registo e alteração de senha.
- **Menu de Logs**: Permite visualizar os logs do sistema, registrando ações dos utilizadores.

## Utilização

Este módulo é essencial para a usabilidade do sistema, fornecendo uma interface clara e intuitiva para os utilizadores navegarem e interagirem com as funcionalidades do sistema.

## Descrição
Este módulo gere todas as interfaces de utilizador do sistema, incluindo menus e submenus para diferentes tipos de utilizadores.

## Funções Principais

### Menus Principais
- `void menuPrincipal()` - Menu inicial do sistema
- `void menu_login_reg(int opcao)` - Menu de login e registo
- `void menuAdmin()` - Menu específico para administradores
- `void menuTecnico()` - Menu específico para técnicos

### Submenus
- `void menuGestaoUtilizadores()` - Submenu de gestão de utilizadores
- `void menuGestaoIncidentes()` - Submenu de gestão de incidentes
- `void menuGestaoFerramentas()` - Submenu de gestão de ferramentas
- `void menuLogs()` - Submenu de visualização de logs

## Fluxo de Navegação

### Menu Principal
1. Login/Registo
2. Sair

### Menu Administrador
1. Gestão de Utilizadores
2. Gestão de Incidentes
3. Gestão de Ferramentas
4. Visualizar Logs
5. Logout

### Menu Técnico
1. Gestão de Incidentes
2. Gestão de Ferramentas
3. Logout

## Exemplo de Utilização

### Menu Principal
```c
void menuPrincipal() {
    // Implementação do menu principal
}
```

### Menu de Administrador
```c
void menuAdmin() {
    // Implementação do menu de administrador
}
```

## Notas de Implementação
- Interface baseada em texto
- Navegação intuitiva
- Validação de permissões
- Feedback claro para o utilizador

## Boas Práticas
- Manter a interface limpa e organizada
- Fornecer instruções claras
- Validar todas as entradas do utilizador
- Tratar erros de forma adequada

## Segurança
- Verificação de permissões em cada menu
- Proteção contra acesso não autorizado
- Registo de todas as ações nos logs 
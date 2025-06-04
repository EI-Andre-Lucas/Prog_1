#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "users.h"
#include "../menus/menus.h"
#include "../logs/logs.h"
#include "../utils/utils.h"

#define HASH_STR_SIZE 21
#define MAX_USERS 100
#define USERS_FILE "users/users.bin"

static USERS users[MAX_USERS];
static int num_users = 0;

// Função para carregar o número de usuários do arquivo
void load_user_count() {
    FILE *file = fopen(USERS_FILE, "rb");
    if (file != NULL) {
        fread(&num_users, sizeof(int), 1, file);
        fclose(file);
        registarLog("SISTEMA", "Número de utilizadores carregado com sucesso");
    } else {
        registarLog("SISTEMA", "Falha ao carregar número de utilizadores - ficheiro não encontrado");
    }
}

bool guardarSessao(USERS *user) {
    FILE *f = fopen("users/sessao.dat", "wb");
    if (!f) {
        registarLog("SISTEMA", "Falha ao guardar sessão - erro ao abrir ficheiro");
        return false;
    }
    fwrite(user, sizeof(USERS), 1, f);
    fclose(f);
    registarLog(user->username, "Sessão guardada com sucesso");
    return true;
}

// Função para salvar o número de usuários no arquivo
void save_user_count() {
    FILE *file = fopen(USERS_FILE, "wb");
    if (file != NULL) {
        fwrite(&num_users, sizeof(int), 1, file);
        fclose(file);
        registarLog("SISTEMA", "Número de utilizadores guardado com sucesso");
    } else {
        registarLog("SISTEMA", "Falha ao guardar número de utilizadores - erro ao abrir ficheiro");
    }
}

void remove_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

// Função de hash para encriptar passwords
unsigned long hash_string(const char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c

    return hash;
}

// Converte o hash numérico para string (para guardar)
void hash_to_string(const char *password, char *output) {
    unsigned long hash = hash_string(password);
    snprintf(output, HASH_STR_SIZE, "%lu", hash);
}

void FirstUserCreator() {
    FILE* file = fopen(USERS_FILE, "rb");
    if (file != NULL) {
        fread(&num_users, sizeof(int), 1, file);
        fclose(file);
        registarLog("SISTEMA", "Ficheiro de utilizadores já existe - administrador não criado");
        return;
    }

    USERS admin;
    admin.id = 1;
    strcpy(admin.username, "admin");
    strcpy(admin.primeiroNome, "Administrador");
    strcpy(admin.ultimoNome, "Sistema");
    hash_to_string("admin", admin.password);
    admin.tipoUser = ADMINISTRADOR;
    
    file = fopen(USERS_FILE, "wb");
    if (file == NULL) {
        printf("Erro ao criar ficheiro de utilizadores!\n");
        registarLog("SISTEMA", "Falha ao criar ficheiro de utilizadores - erro de permissão");
        return;
    }
    
    num_users = 1;
    fwrite(&num_users, sizeof(int), 1, file);
    fwrite(&admin, sizeof(USERS), 1, file);
    fclose(file);
    
    users[0] = admin;
    num_users = 1;
    
    printf("Utilizador administrador criado com sucesso!\n");
    registarLog("SISTEMA", "Utilizador administrador criado com sucesso");
}

bool login() {
    char username[50], password[50];
    char hashed_password[HASH_STR_SIZE];
    static USERS current_user;

    printf("\n=== Login ===\n");
    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Password: ");
    lerPassword(password, sizeof(password));

    hash_to_string(password, hashed_password);
    registarLog("SISTEMA", "Tentativa de início de sessão iniciada");

    FILE* file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        printf("Erro ao abrir ficheiro de utilizadores!\n");
        registarLog("SISTEMA", "Falha no início de sessão - ficheiro de utilizadores não encontrado");
        return false;
    }

    int num_users_file;
    if (fread(&num_users_file, sizeof(int), 1, file) != 1) {
        printf("Erro ao ler número de utilizadores!\n");
        registarLog("SISTEMA", "Falha no início de sessão - erro ao ler número de utilizadores");
        fclose(file);
        return false;
    }

    USERS* users = malloc(num_users_file * sizeof(USERS));
    for (int i = 0; i < num_users_file; i++) {
        if (fread(&users[i], sizeof(USERS), 1, file) != 1) {
            printf("Erro ao ler utilizador do ficheiro!\n");
            registarLog("SISTEMA", "Falha no início de sessão - erro ao ler dados dos utilizadores");
            free(users);
            fclose(file);
            return false;
        }
    }
    fclose(file);

    bool login_sucesso = false;
    for (int i = 0; i < num_users_file; i++) {
        if (strcmp(users[i].username, username) == 0) {
            if (strcmp(users[i].password, hashed_password) == 0) {
                current_user = users[i];
                login_sucesso = true;
                registarLog(username, "Início de sessão realizado com sucesso");
                break;
            } else {
                printf("Palavra-passe incorreta!\n");
                registarLog(username, "Tentativa de início de sessão falhou - palavra-passe incorreta");
                break;
            }
        }
    }
    free(users);

    if (login_sucesso) {
        if (guardarSessao(&current_user)) {
            return true;
        } else {
            printf("Erro ao guardar sessão!\n");
            registarLog(username, "Falha ao guardar sessão após início de sessão");
            clickEnter();
            return false;
        }
    }

    if (!login_sucesso) {
        printf("Início de sessão falhou! Username ou palavra-passe incorretos.\n");
        registarLog(username, "Tentativa de início de sessão falhou - credenciais inválidas");
        clickEnter();
    }
    return false;
}

void registo(bool criar_admin) {
    USERS novo_user;
    char password[50];

    memset(&novo_user, 0, sizeof(USERS));
    registarLog("SISTEMA", "Iniciando processo de registo");

    printf("\n=== Registo ===\n");
    
    printf("Primeiro Nome: ");
    fgets(novo_user.primeiroNome, sizeof(novo_user.primeiroNome), stdin);
    novo_user.primeiroNome[strcspn(novo_user.primeiroNome, "\n")] = 0;

    printf("Último Nome: ");
    fgets(novo_user.ultimoNome, sizeof(novo_user.ultimoNome), stdin);
    novo_user.ultimoNome[strcspn(novo_user.ultimoNome, "\n")] = 0;

    printf("Username: ");
    fgets(novo_user.username, sizeof(novo_user.username), stdin);
    novo_user.username[strcspn(novo_user.username, "\n")] = 0;

    printf("Password: ");
    lerPassword(password, sizeof(password));

    hash_to_string(password, novo_user.password);
    novo_user.tipoUser = criar_admin ? ADMINISTRADOR : TECNICO;

    #ifdef _WIN32
    system("if not exist users mkdir users");
    #else
    system("mkdir -p users");
    #endif

    FILE* file = fopen(USERS_FILE, "rb");
    int num_users = 0;
    if (file != NULL) {
        fread(&num_users, sizeof(int), 1, file);
        fclose(file);
        registarLog("SISTEMA", "Número atual de utilizadores carregado");
    }

    num_users++;

    if (num_users == 1) {
        file = fopen(USERS_FILE, "wb");
        if (file == NULL) {
            printf("Erro ao criar ficheiro de utilizadores!\n");
            registarLog("SISTEMA", "Falha ao criar ficheiro de utilizadores - erro de permissão");
            return;
        }
        fwrite(&num_users, sizeof(int), 1, file);
        fclose(file);
        registarLog("SISTEMA", "Ficheiro de utilizadores criado");
    } else {
        file = fopen(USERS_FILE, "r+b");
        if (file == NULL) {
            printf("Erro ao abrir ficheiro de utilizadores!\n");
            registarLog("SISTEMA", "Falha ao abrir ficheiro de utilizadores - erro de permissão");
            return;
        }
        fwrite(&num_users, sizeof(int), 1, file);
        fclose(file);
        registarLog("SISTEMA", "Número de utilizadores atualizado");
    }

    file = fopen(USERS_FILE, "ab");
    if (file == NULL) {
        printf("Erro ao abrir ficheiro de utilizadores!\n");
        registarLog("SISTEMA", "Falha ao abrir ficheiro para adicionar novo utilizador");
        return;
    }

    fwrite(&novo_user, sizeof(USERS), 1, file);
    fclose(file);

    printf("\nRegisto realizado com sucesso!\n");
    registarLog(novo_user.username, criar_admin ? "Novo administrador registado" : "Novo técnico registado");
    clickEnter();
}

void logout() {
    USERS* user = verificarSessaoAtiva();
    if (user != NULL) {
        registarLog(user->username, "Fim de sessão realizado");
    }
    
    if (remove("users/sessao.dat") == 0) {
        printf("\nFim de sessão realizado com sucesso!\n");
        registarLog("SISTEMA", "Ficheiro de sessão removido com sucesso");
    } else {
        printf("\nErro ao realizar fim de sessão!\n");
        registarLog("SISTEMA", "Erro ao remover ficheiro de sessão");
    }
    clickEnter();
    menuPrincipal();
}

USERS* verificarSessaoAtiva() {
    FILE *f = fopen("users/sessao.dat", "rb");
    if (!f) {
        registarLog("SISTEMA", "Nenhuma sessão ativa encontrada");
        return NULL;
    }

    static USERS user;
    if (fread(&user, sizeof(USERS), 1, f) != 1) {
        fclose(f);
        registarLog("SISTEMA", "Erro ao ler dados da sessão");
        return NULL;
    }

    fclose(f);
    registarLog("SISTEMA", "Sessão ativa verificada com sucesso");
    return &user;
}

void listarUtilizadores() {
    printf("\n=== Lista de Utilizadores ===\n");
    FILE* file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        printf("Erro ao abrir ficheiro de utilizadores!\n");
        registarLog("SISTEMA", "Erro ao listar utilizadores - ficheiro não encontrado");
        return;
    }

    int num_users_file;
    fread(&num_users_file, sizeof(int), 1, file);

    for (int i = 0; i < num_users_file; i++) {
        USERS user;
        fread(&user, sizeof(USERS), 1, file);
        printf("\nID: %d\n", user.id);
        printf("Nome: %s %s\n", user.primeiroNome, user.ultimoNome);
        printf("Username: %s\n", user.username);
        printf("Tipo: %s\n", user.tipoUser == ADMINISTRADOR ? "Administrador" : "Técnico");
    }
    fclose(file);
    
    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        registarLog(current_user->username, "Listou todos os utilizadores");
    }
    clickEnter();
}

void removerUtilizador(const char* username) {
    FILE* file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        printf("Erro ao abrir ficheiro de utilizadores!\n");
        registarLog("SISTEMA", "Erro ao remover utilizador - ficheiro não encontrado");
        return;
    }

    int num_users_file;
    fread(&num_users_file, sizeof(int), 1, file);

    USERS* users = malloc(num_users_file * sizeof(USERS));
    int new_num_users = 0;
    bool found = false;

    for (int i = 0; i < num_users_file; i++) {
        USERS user;
        fread(&user, sizeof(USERS), 1, file);
        if (strcmp(user.username, username) != 0) {
            users[new_num_users++] = user;
        } else {
            found = true;
        }
    }
    fclose(file);

    if (!found) {
        printf("Utilizador não encontrado!\n");
        registarLog("SISTEMA", "Tentativa de remover utilizador falhou - utilizador não encontrado");
        free(users);
        return;
    }

    file = fopen(USERS_FILE, "wb");
    if (file == NULL) {
        printf("Erro ao salvar ficheiro de utilizadores!\n");
        registarLog("SISTEMA", "Erro ao remover utilizador - falha ao salvar ficheiro");
        free(users);
        return;
    }

    fwrite(&new_num_users, sizeof(int), 1, file);
    fwrite(users, sizeof(USERS), new_num_users, file);
    fclose(file);
    free(users);

    printf("Utilizador removido com sucesso!\n");
    
    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        char log_message[150];
        snprintf(log_message, sizeof(log_message), "Removeu o utilizador %s", username);
        registarLog(current_user->username, log_message);
    }
}

void modificarTipoUtilizador(const char* username, int novo_tipo) {
    FILE* file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        printf("Erro ao abrir ficheiro de utilizadores!\n");
        registarLog("SISTEMA", "Erro ao modificar tipo de utilizador - ficheiro não encontrado");
        return;
    }

    int num_users_file;
    fread(&num_users_file, sizeof(int), 1, file);

    USERS* users = malloc(num_users_file * sizeof(USERS));
    bool found = false;

    for (int i = 0; i < num_users_file; i++) {
        fread(&users[i], sizeof(USERS), 1, file);
        if (strcmp(users[i].username, username) == 0) {
            users[i].tipoUser = novo_tipo;
            found = true;
        }
    }
    fclose(file);

    if (!found) {
        printf("Utilizador não encontrado!\n");
        registarLog("SISTEMA", "Tentativa de modificar tipo de utilizador falhou - utilizador não encontrado");
        free(users);
        return;
    }

    file = fopen(USERS_FILE, "wb");
    if (file == NULL) {
        printf("Erro ao salvar ficheiro de utilizadores!\n");
        registarLog("SISTEMA", "Erro ao modificar tipo de utilizador - falha ao salvar ficheiro");
        free(users);
        return;
    }

    fwrite(&num_users_file, sizeof(int), 1, file);
    fwrite(users, sizeof(USERS), num_users_file, file);
    fclose(file);
    free(users);

    printf("Tipo de utilizador alterado com sucesso!\n");
    
    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        char log_message[150];
        snprintf(log_message, sizeof(log_message), "Modificou o tipo do utilizador %s para %s", 
                username, novo_tipo == ADMINISTRADOR ? "Administrador" : "Técnico");
        registarLog(current_user->username, log_message);
    }
}

void alterarPasswordUtilizador(const char* username, const char* nova_senha) {
    FILE* file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        printf("Erro ao abrir ficheiro de utilizadores!\n");
        registarLog("SISTEMA", "Erro ao alterar password - ficheiro não encontrado");
        return;
    }

    int num_users_file;
    fread(&num_users_file, sizeof(int), 1, file);

    USERS* users = malloc(num_users_file * sizeof(USERS));
    bool found = false;

    for (int i = 0; i < num_users_file; i++) {
        fread(&users[i], sizeof(USERS), 1, file);
        if (strcmp(users[i].username, username) == 0) {
            char hashed_password[HASH_STR_SIZE];
            hash_to_string(nova_senha, hashed_password);
            strcpy(users[i].password, hashed_password);
            found = true;
        }
    }
    fclose(file);

    if (!found) {
        printf("Utilizador não encontrado!\n");
        registarLog("SISTEMA", "Tentativa de alterar password falhou - utilizador não encontrado");
        free(users);
        return;
    }

    file = fopen(USERS_FILE, "wb");
    if (file == NULL) {
        printf("Erro ao salvar ficheiro de utilizadores!\n");
        registarLog("SISTEMA", "Erro ao alterar password - falha ao salvar ficheiro");
        free(users);
        return;
    }

    fwrite(&num_users_file, sizeof(int), 1, file);
    fwrite(users, sizeof(USERS), num_users_file, file);
    fclose(file);
    free(users);

    printf("Senha alterada com sucesso!\n");
    
    USERS* current_user = verificarSessaoAtiva();
    if (current_user != NULL) {
        char log_message[150];
        snprintf(log_message, sizeof(log_message), "Alterou a password do utilizador %s", username);
        registarLog(current_user->username, log_message);
    }
}

USERS* procurarTecnico(const char* username) {
    FILE* file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        return NULL;
    }

    int num_users_file;
    fread(&num_users_file, sizeof(int), 1, file);

    static USERS user;
    for (int i = 0; i < num_users_file; i++) {
        fread(&user, sizeof(USERS), 1, file);
        if (strcmp(user.username, username) == 0) {
            fclose(file);
            return &user;
        }
    }

    fclose(file);
    return NULL; // Usuário não encontrado
}

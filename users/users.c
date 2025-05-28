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
#define USERS_FILE "users.dat"

static USERS users[MAX_USERS];
static int num_users = 0;

// Função para carregar o número de usuários do arquivo
void load_user_count() {
    FILE *file = fopen(USERS_FILE, "rb");
    if (file != NULL) {
        fread(&num_users, sizeof(int), 1, file);
        fclose(file);
    }
}

bool guardarSessao(USERS *user) {
    FILE *f = fopen("sessao.dat", "wb");
    if (!f) return false;
    fwrite(user, sizeof(USERS), 1, f);
    fclose(f);
    return true;
}

// Função para salvar o número de usuários no arquivo
void save_user_count() {
    FILE *file = fopen(USERS_FILE, "wb");
    if (file != NULL) {
        fwrite(&num_users, sizeof(int), 1, file);
        fclose(file);
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
    load_user_count(); // Carrega o número de usuários do arquivo
    
    if (num_users == 0) {
        USERS admin;
        admin.id = 1;
        strcpy(admin.username, "admin");
        strcpy(admin.primeiroNome, "Administrador");
        strcpy(admin.ultimoNome, "Sistema");
        hash_to_string("admin", admin.password);
        admin.tipoUser = ADMINISTRADOR;
        users[num_users++] = admin;
        save_user_count(); // Salva o número de usuários após criar o admin
    }
}

void login() {
    char username[50], password[50];
    char hashed_password[HASH_STR_SIZE];
    USERS user;

    printf("\n=== Login ===\n");
    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    // Encripta a password inserida para comparação
    hash_to_string(password, hashed_password);

    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, username) == 0 && 
            strcmp(users[i].password, hashed_password) == 0) {
            user = users[i];
            if (guardarSessao(&user)) {
                menuPrincipal(&user);
            } else {
                printf("Erro ao guardar sessão!\n");
                clickEnter();
            }
            return;
        }
    }

    printf("Login falhou! Username ou password incorretos.\n");
    clickEnter();
}

void registo() {
    if (num_users >= MAX_USERS) {
        printf("Número máximo de utilizadores atingido!\n");
        clickEnter();
        return;
    }

    USERS novo_user;
    novo_user.id = num_users + 1;

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

    // Verifica se o username já existe
    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, novo_user.username) == 0) {
            printf("Username já existe! Por favor escolha outro.\n");
            clickEnter();
            return;
        }
    }

    printf("Password: ");
    char password[50];
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    // Encripta a password antes de guardar
    hash_to_string(password, novo_user.password);

    novo_user.tipoUser = TECNICO; // Por padrão, novos usuários são técnicos

    users[num_users++] = novo_user;
    save_user_count(); // Salva o novo número de usuários
    printf("Registo realizado com sucesso!\n");
    clickEnter();
}

void logout() {
    if (remove("sessao.dat") == 0) {
        printf("\nLogout realizado com sucesso!\n");
    } else {
        printf("\nErro ao realizar logout!\n");
    }
    clickEnter();
}
USERS* verificarSessaoAtiva() {
    FILE *f = fopen("sessao.dat", "rb");
    if (!f) return NULL;

    static USERS user;
    if (fread(&user, sizeof(USERS), 1, f) != 1) {
        fclose(f);
        return NULL;
    }

    fclose(f);
    return &user;
}

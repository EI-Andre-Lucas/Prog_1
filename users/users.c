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
#define USERS_FILE "users/users.dat"

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
    FILE *f = fopen("users/sessao.dat", "wb");
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
    // Verifica se o arquivo já existe
    FILE* file = fopen(USERS_FILE, "rb");
    if (file != NULL) {
        fread(&num_users, sizeof(int), 1, file);
        fclose(file);
        return; // Se o arquivo existe, não precisa criar o admin
    }

    // Se o arquivo não existe, cria o admin
    USERS admin;
    admin.id = 1;
    strcpy(admin.username, "admin");
    strcpy(admin.primeiroNome, "Administrador");
    strcpy(admin.ultimoNome, "Sistema");
    hash_to_string("admin", admin.password);
    admin.tipoUser = ADMINISTRADOR;
    
    // Salva o admin no arquivo
    file = fopen(USERS_FILE, "wb");
    if (file == NULL) {
        printf("Erro ao criar arquivo de utilizadores!\n");
        return;
    }
    
    num_users = 1; // Define o número de utilizadores como 1
    fwrite(&num_users, sizeof(int), 1, file);
    fwrite(&admin, sizeof(USERS), 1, file);
    fclose(file);
    
    // Atualiza o array em memória
    users[0] = admin;
    num_users = 1;
    
    printf("Utilizador admin criado com sucesso!\n");
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

    // Encripta a password inserida para comparação
    hash_to_string(password, hashed_password);

    // Abre o arquivo de utilizadores
    FILE* file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        printf("Erro ao abrir arquivo de utilizadores!\n");
        return false;
    }

    int num_users_file;
    fread(&num_users_file, sizeof(int), 1, file);

    bool login_sucesso = false;
    for (int i = 0; i < num_users_file; i++) {
        USERS user;
        fread(&user, sizeof(USERS), 1, file);
        if (strcmp(user.username, username) == 0 && 
            strcmp(user.password, hashed_password) == 0) {
            current_user = user;
            login_sucesso = true;
            break;
        }
    }
    fclose(file);

    if (login_sucesso) {
        if (guardarSessao(&current_user)) {
            registrarLog(username, "Login realizado com sucesso");
            return true;
        } else {
            printf("Erro ao guardar sessão!\n");
            registrarLog(username, "Falha ao guardar sessão");
            clickEnter();
            return false;
        }
    }

    printf("Login falhou! Username ou password incorretos.\n");
    registrarLog(username, "Tentativa de login falhou - credenciais inválidas");
    clickEnter();
    return false;
}

void registo(bool criar_admin) {
    if (num_users >= MAX_USERS) {
        printf("Número máximo de utilizadores atingido!\n");
        registrarLog("SISTEMA", "Tentativa de registo falhou - limite de utilizadores atingido");
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

    bool username_valido = false;
    while (!username_valido) {
        printf("Username: ");
        fgets(novo_user.username, sizeof(novo_user.username), stdin);
        novo_user.username[strcspn(novo_user.username, "\n")] = 0;

        // Verifica se o username está vazio
        if (strlen(novo_user.username) == 0) {
            printf("Username não pode estar vazio! Tente novamente.\n");
            continue;
        }

        // Verifica se o username já existe no array em memória
        bool username_existe = false;
        for (int i = 0; i < num_users; i++) {
            if (strcmp(users[i].username, novo_user.username) == 0) {
                username_existe = true;
                break;
            }
        }

        // Se não encontrou no array, verifica no arquivo
        if (!username_existe) {
            FILE* file = fopen(USERS_FILE, "rb");
            if (file != NULL) {
                int num_users_file;
                fread(&num_users_file, sizeof(int), 1, file);

                for (int i = 0; i < num_users_file; i++) {
                    USERS user;
                    fread(&user, sizeof(USERS), 1, file);
                    if (strcmp(user.username, novo_user.username) == 0) {
                        username_existe = true;
                        break;
                    }
                }
                fclose(file);
            }
        }

        if (username_existe) {
            printf("Username já existe! Por favor escolha outro.\n");
            registrarLog(novo_user.username, "Tentativa de registo falhou - username já existe");
        } else {
            username_valido = true;
        }
    }

    printf("Password: ");
    char password[50];
    lerPassword(password, sizeof(password));

    // Encripta a password antes de guardar
    hash_to_string(password, novo_user.password);

    // Define o tipo de utilizador baseado no parâmetro criar_admin
    novo_user.tipoUser = criar_admin ? ADMINISTRADOR : TECNICO;

    users[num_users++] = novo_user;
    save_user_count(); // Salva o novo número de utilizadores
    printf("Registo realizado com sucesso!\n");
    registrarLog(novo_user.username, criar_admin ? "Novo administrador registrado" : "Novo técnico registrado");
    clickEnter();
}

void logout() {
    USERS* user = verificarSessaoAtiva();
    if (user != NULL) {
        registrarLog(user->username, "Logout realizado");
    }
    
    if (remove("users/sessao.dat") == 0) {
        printf("\nLogout realizado com sucesso!\n");
    } else {
        printf("\nErro ao realizar logout!\n");
        registrarLog("SISTEMA", "Erro ao realizar logout");
    }
    clickEnter();
    menuPrincipal(); // Retorna ao menu inicial após o logout
}
USERS* verificarSessaoAtiva() {
    FILE *f = fopen("users/sessao.dat", "rb");
    if (!f) return NULL;

    static USERS user;
    if (fread(&user, sizeof(USERS), 1, f) != 1) {
        fclose(f);
        return NULL;
    }

    fclose(f);
    return &user;
}

void listarUtilizadores() {
    printf("\n=== Lista de Utilizadores ===\n");
    FILE* file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        printf("Erro ao abrir arquivo de utilizadores!\n");
        registrarLog("SISTEMA", "Erro ao listar utilizadores - arquivo não encontrado");
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
        registrarLog(current_user->username, "Listou todos os utilizadores");
    }
}

void removerUtilizador(const char* username) {
    FILE* file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        printf("Erro ao abrir arquivo de utilizadores!\n");
        registrarLog("SISTEMA", "Erro ao remover utilizador - arquivo não encontrado");
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
        registrarLog("SISTEMA", "Tentativa de remover utilizador falhou - utilizador não encontrado");
        free(users);
        return;
    }

    file = fopen(USERS_FILE, "wb");
    if (file == NULL) {
        printf("Erro ao salvar arquivo de utilizadores!\n");
        registrarLog("SISTEMA", "Erro ao remover utilizador - falha ao salvar arquivo");
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
        registrarLog(current_user->username, log_message);
    }
}

void modificarTipoUtilizador(const char* username, int novo_tipo) {
    FILE* file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        printf("Erro ao abrir arquivo de utilizadores!\n");
        registrarLog("SISTEMA", "Erro ao modificar tipo de utilizador - arquivo não encontrado");
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
        registrarLog("SISTEMA", "Tentativa de modificar tipo de utilizador falhou - utilizador não encontrado");
        free(users);
        return;
    }

    file = fopen(USERS_FILE, "wb");
    if (file == NULL) {
        printf("Erro ao salvar arquivo de utilizadores!\n");
        registrarLog("SISTEMA", "Erro ao modificar tipo de utilizador - falha ao salvar arquivo");
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
        registrarLog(current_user->username, log_message);
    }
}

void alterarPasswordUtilizador(const char* username, const char* nova_senha) {
    FILE* file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        printf("Erro ao abrir arquivo de utilizadores!\n");
        registrarLog("SISTEMA", "Erro ao alterar password - arquivo não encontrado");
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
        registrarLog("SISTEMA", "Tentativa de alterar password falhou - utilizador não encontrado");
        free(users);
        return;
    }

    file = fopen(USERS_FILE, "wb");
    if (file == NULL) {
        printf("Erro ao salvar arquivo de utilizadores!\n");
        registrarLog("SISTEMA", "Erro ao alterar password - falha ao salvar arquivo");
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
        registrarLog(current_user->username, log_message);
    }
}

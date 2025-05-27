#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "users.h"
#include "../menus/menus.h"
#include "../logs/logs.h"

#define HASH_STR_SIZE 21

void remove_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

//STACKOVERFLOW https://stackoverflow.com/questions/7666509/hash-function-for-string
unsigned long hash_string(const char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c

    return hash;
}

USERS getsessao(){
    USERS user;
    FILE *fp = fopen("sessao.dat","rb");
    if(fp == NULL){
        printf("\nErro ao abrir o ficheiro");
        user.username[0] = '\0';  // Retorna um user vazio em caso de erro
        return user;
    }
    fread(&user, sizeof(USERS), 1, fp);
    fclose(fp);
    return user;
}

// Converte o hash numérico para string (para guardar)
void hash_to_string(const char *password, char *output) {
    unsigned long hash = hash_string(password);
    snprintf(output, HASH_STR_SIZE, "%lu", hash);
}

void FirstUserCreator(){
    USERS user;
    char TempPassword[100] = {"admin"};
    strcpy(user.username, "admin");
    hash_to_string(TempPassword, user.password);
    user.tipoUser = 0;
    FILE *fp = fopen("../users/users.dat","ab+");
    if(fp==NULL){
        printf("\nErro ao abrir o ficheiro, o programa vai ser encerrado");
        return;
    }
    fwrite(&user, sizeof(USERS), 1, fp);
    fclose(fp);
}

bool guardarSessao(USERS *user) {
    FILE *f = fopen("sessao.dat", "wb");
    if (!f) return false;
    fwrite(user, sizeof(USERS), 1, f);
    fclose(f);
    return true;
}

void logout() {
    remove("sessao.dat");
}

void login(){
    FILE *fp = fopen("users.dat","rb");

    if(fp == NULL){
        printf("Erro ao abrir o ficheiro");
        return;
    }

    USERS user;
    char tempP[100], username[100];
    char hashedPassword[HASH_STR_SIZE];

    printf("\nUsername: ");
    fgets(username, 100, stdin);
    remove_newline(username);

    printf("\nPassword: ");
    fgets(tempP, 100, stdin);
    remove_newline(tempP);
    
    hash_to_string(tempP, hashedPassword);

    while(fread(&user, sizeof(USERS), 1, fp) == 1){
        if(strcmp(username, user.username) == 0){
            if(strcmp(hashedPassword, user.password) == 0){
                guardarSessao(&user);
                printf("\nLogin bem sucedido.");
                clickEnter();
            }else{
                clrscr();
                printf("Palavra passe ou username errados, tente novamente mais tarde.");
                logmessage("Tentativa falha de login",user);
            }
        }
    }
    
    fclose(fp);
    clickEnter();
    menu_login_reg(0);
}

void registo(){
    USERS user;
    char TempPassword[100];
    printf("| Registar |\n");
    printf("Se por um acaso se tenha enganado e entrou neste menu por engano escreva \";\" no username e voltará ao menu anterior\n");
    printf("\nUsername: ");
    fgets(user.username, 100, stdin);
    remove_newline(user.username);
    if(strcmp(user.username,";")==0){
        menu_login_reg();
    }
    printf("\nPrimeiro Nome: ");
    fgets(user.PrimeiroNome, 100, stdin);
    remove_newline(user.PrimeiroNome);
    printf("\nÚltimo Nome: ");
    fgets(user.UltimoNome, 100, stdin);
    remove_newline(user.UltimoNome);
    printf("\nEmail: ");
    fgets(user.email, 100, stdin);
    remove_newline(user.email);
    printf("\nPassword: ");
    fgets(TempPassword, 100, stdin);
    remove_newline(TempPassword);
    hash_to_string(TempPassword, user.password);
    user.tipoUser = 1;
    FILE *fp = fopen("../users/users.dat","ab+");
    if(fp==NULL){
        printf("\nErro ao abrir o ficheiro, o programa vai ser encerrado");
        return;
    }
    fwrite(&user, sizeof(USERS), 1, fp);
    fclose(fp);

    printf("\nFoste registado!");
    clickEnter();

}

void fornecerTodosUsers() {
    FILE *file = fopen("users.dat", "rb");
    if (!file) {
        printf("Erro ao abrir o arquivo de usuários!\n");
        return;
    }

    printf("\n=== Lista de Usuários ===\n");
    USERS user;
    while (fread(&user, sizeof(USERS), 1, file) == 1) {
        printf("Username: %s\n", user.username);
        printf("Tipo: %s\n", user.tipoUser == ADMINISTRADOR ? "Administrador" : "Técnico");
        printf("Nome: %s %s\n", user.PrimeiroNome, user.UltimoNome);
        printf("Email: %s\n", user.email);
        printf("------------------------\n");
    }
    fclose(file);
}

bool adicionarUser(const char* username, const char* password, int tipo) {
    // Verificar se o usuário já existe
    FILE *file = fopen("users.dat", "rb");
    if (file) {
        USERS user;
        while (fread(&user, sizeof(USERS), 1, file) == 1) {
            if (strcmp(user.username, username) == 0) {
                fclose(file);
                return false; // Usuário já existe
            }
        }
        fclose(file);
    }

    // Criar novo usuário
    USERS novo_user;
    strncpy(novo_user.username, username, sizeof(novo_user.username) - 1);
    hash_to_string(password, novo_user.password);
    novo_user.tipoUser = (tipo == 1) ? ADMINISTRADOR : TECNICO;
    novo_user.email[0] = '\0';
    novo_user.PrimeiroNome[0] = '\0';
    novo_user.UltimoNome[0] = '\0';

    // Adicionar ao arquivo
    file = fopen("users.dat", "ab");
    if (!file) return false;

    bool sucesso = (fwrite(&novo_user, sizeof(USERS), 1, file) == 1);
    fclose(file);

    if (sucesso) {
        USERS admin = getsessao();
        char log_msg[300];
        snprintf(log_msg, sizeof(log_msg), "Novo usuário criado: %s (Tipo: %s)", 
                username, tipo == 1 ? "Administrador" : "Técnico");
        logmessage(log_msg, admin);
    }

    return sucesso;
}

bool removerUser(const char* username) {
    // Não permitir remover o último administrador
    if (strcmp(username, "admin") == 0) {
        printf("Não é possível remover o usuário admin!\n");
        return false;
    }

    FILE *file = fopen("users.dat", "rb");
    if (!file) return false;

    FILE *temp = fopen("temp.dat", "wb");
    if (!temp) {
        fclose(file);
        return false;
    }

    USERS user;
    bool encontrado = false;
    bool sucesso = true;

    while (fread(&user, sizeof(USERS), 1, file) == 1) {
        if (strcmp(user.username, username) == 0) {
            encontrado = true;
            continue; // Pular este usuário
        }
        if (fwrite(&user, sizeof(USERS), 1, temp) != 1) {
            sucesso = false;
            break;
        }
    }

    fclose(file);
    fclose(temp);

    if (sucesso && encontrado) {
        remove("users.dat");
        rename("temp.dat", "users.dat");

        // Registrar no log
        USERS admin = getsessao();
        char log_msg[300];
        snprintf(log_msg, sizeof(log_msg), "Usuário removido: %s", username);
        logmessage(log_msg, admin);
    } else {
        remove("temp.dat");
    }

    return sucesso && encontrado;
}

bool modificarUser(const char* username, const char* nova_password) {
    FILE *file = fopen("users.dat", "rb+");
    if (!file) return false;

    USERS user;
    bool encontrado = false;
    bool sucesso = false;

    while (fread(&user, sizeof(USERS), 1, file) == 1) {
        if (strcmp(user.username, username) == 0) {
            encontrado = true;
            hash_to_string(nova_password, user.password);
            
            // Voltar ao início do registro
            fseek(file, -sizeof(USERS), SEEK_CUR);
            if (fwrite(&user, sizeof(USERS), 1, file) == 1) {
                sucesso = true;
                
                // Registrar no log
                USERS admin = getsessao();
                char log_msg[300];
                snprintf(log_msg, sizeof(log_msg), "Senha do usuário %s modificada", username);
                logmessage(log_msg, admin);
            }
            break;
        }
    }

    fclose(file);
    return encontrado && sucesso;
}

bool alterarRoleUser(const char* username, int novo_tipo) {
    // Não permitir alterar o tipo do admin
    if (strcmp(username, "admin") == 0) {
        printf("Não é possível alterar o tipo do usuário admin!\n");
        return false;
    }

    FILE *file = fopen("users.dat", "rb+");
    if (!file) return false;

    USERS user;
    bool encontrado = false;
    bool sucesso = false;

    while (fread(&user, sizeof(USERS), 1, file) == 1) {
        if (strcmp(user.username, username) == 0) {
            encontrado = true;
            user.tipoUser = (novo_tipo == 1) ? ADMINISTRADOR : TECNICO;
            
            // Voltar ao início do registro
            fseek(file, -sizeof(USERS), SEEK_CUR);
            if (fwrite(&user, sizeof(USERS), 1, file) == 1) {
                sucesso = true;
                
                // Registrar no log
                USERS admin = getsessao();
                char log_msg[300];
                snprintf(log_msg, sizeof(log_msg), "Tipo do usuário %s alterado para %s", 
                        username, novo_tipo == 1 ? "Administrador" : "Técnico");
                logmessage(log_msg, admin);
            }
            break;
        }
    }

    fclose(file);
    return encontrado && sucesso;
}

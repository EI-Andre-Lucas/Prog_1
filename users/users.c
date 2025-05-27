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

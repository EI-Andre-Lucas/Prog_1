#include "utils.h"
#include <stdlib.h>
#include <conio.h>

void clickEnter() {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

void limparEcra() {
    system("cls");
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void lerPassword(char* password, int tamanho_max) {
    int i = 0;
    char c;
    
    while (i < tamanho_max - 1) {
        c = getch(); // Lê um caractere sem mostrar na tela
        
        if (c == 13) { // Enter
            break;
        } else if (c == 8) { // Backspace
            if (i > 0) {
                i--;
                printf("\b \b"); // Apaga o asterisco anterior
            }
        } else {
            password[i] = c;
            printf("*"); // Mostra asterisco no lugar do caractere
            i++;
        }
    }
    
    password[i] = '\0'; // Termina a string
    printf("\n");
} 
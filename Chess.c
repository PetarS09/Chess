#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define MAX_SIZE 26 

char board[MAX_SIZE][MAX_SIZE];
int boardSize = 8;

void clearBoard() {
    for (int i = 0; i < boardSize; i++)
        for (int j = 0; j < boardSize; j++)
            board[i][j] = ' ';
}

void printBoard() {
    printf("\n   ");
    for (int j = 0; j < boardSize; j++) printf(" %c  ", 'A' + j);
    printf("\n  +");
    for (int j = 0; j < boardSize; j++) printf("---+");
    printf("\n");

    for (int i = 0; i < boardSize; i++) {
        printf("%2d |", boardSize - i);
        for (int j = 0; j < boardSize; j++) {
            printf(" %c |", board[i][j]);
        }
        printf(" %d\n  +", boardSize - i);
        for (int j = 0; j < boardSize; j++) printf("---+");
        printf("\n");
    }
}

void changeBoardSize() {
    int newSize;
    printf("\nВъведете нов размер (4-%d): ", MAX_SIZE);
    if (scanf("%d", &newSize) == 1 && newSize >= 4 && newSize <= MAX_SIZE) {
        boardSize = newSize;
        printf("Успешно променен на %dx%d!\n", boardSize, boardSize);
    }
}

int main() {
    int choice;
    while (1) {
        printf("\n=== МЕНЮ ===\n1. Нова игра\n2. Размер дъска (Сега: %dx%d)\n3. Replay\n4. Изход\nИзбор: ", boardSize, boardSize);
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }
        if (choice == 2) changeBoardSize();
        else if (choice == 4) return 0;
    }
    return 0;
}
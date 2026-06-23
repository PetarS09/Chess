#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define MAX_SIZE 26 

typedef struct {
    int r, c;
} Pos;

char board[MAX_SIZE][MAX_SIZE];
int boardSize = 8; 

Pos playerKing, rook1, rook2, compKing;
bool rook1Alive = true, rook2Alive = true;

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

bool inBounds(int r, int c) {
    return (r >= 0 && r < boardSize && c >= 0 && c < boardSize);
}

void generatePositions() {
    clearBoard();
    compKing.r = rand() % boardSize;
    compKing.c = rand() % boardSize;
    board[compKing.r][compKing.c] = 'k';

    do {
        playerKing.r = rand() % boardSize;
        playerKing.c = rand() % boardSize;
    } while (board[playerKing.r][playerKing.c] != ' ' || 
             (abs(playerKing.r - compKing.r) <= 1 && abs(playerKing.c - compKing.c) <= 1));
    board[playerKing.r][playerKing.c] = 'K';

    do {
        rook1.r = rand() % boardSize;
        rook1.c = rand() % boardSize;
    } while (board[rook1.r][rook1.c] != ' ');
    board[rook1.r][rook1.c] = 'R';

    do {
        rook2.r = rand() % boardSize;
        rook2.c = rand() % boardSize;
    } while (board[rook2.r][rook2.c] != ' ');
    board[rook2.r][rook2.c] = 'W';
}

bool computerMakeMove() {
    int dr[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int dc[] = {-1,  0,  1, -1, 1, -1, 0,  1};

    for (int i = 0; i < 8; i++) {
        int nr = compKing.r + dr[i];
        int nc = compKing.c + dc[i];

        if (inBounds(nr, nc) && board[nr][nc] == ' ') {
            board[compKing.r][compKing.c] = ' ';
            compKing.r = nr;
            compKing.c = nc;
            board[compKing.r][compKing.c] = 'k';
            return true;
        }
    }
    return false;
}

void startNewGame() {
    generatePositions();
    printf("\n--- ИГРАТА ЗАПОЧНА ---\n");

    while (1) {
        printBoard();

        printf("\nВъведете вашия ход (например: A2 A4): ");
        char col1, col2; 
        int row1, row2;
        
        if (scanf(" %c%d %c%d", &col1, &row1, &col2, &row2) != 4) {
            printf("Грешка: Невалиден формат на въвеждане!\n");
            while (getchar() != '\n'); 
            continue;
        }

        int fromC = col1 - (col1 >= 'a' ? 'a' : 'A'); 
        int fromR = boardSize - row1;
        int toC = col2 - (col2 >= 'a' ? 'a' : 'A'); 
        int toR = boardSize - row2;

        if (!inBounds(fromR, fromC) || !inBounds(toR, toC)) {
            printf("Грешка: Избраните полета са извън дъската!\n");
            continue;
        }

        char piece = board[fromR][fromC];
        if (piece != 'K' && piece != 'R' && piece != 'W') {
            printf("Грешка: На началното поле няма ваша фигура!\n");
            continue;
        }

        board[fromR][fromC] = ' ';
        board[toR][toC] = piece;

        if (piece == 'K') { playerKing.r = toR; playerKing.c = toC; }
        if (piece == 'R') { rook1.r = toR; rook1.c = toC; }
        if (piece == 'W') { rook2.r = toR; rook2.c = toC; }

        bool compMoved = computerMakeMove();
        
        if (!compMoved) {
            printf("\nКомпютърът няма валидни ходове! Край на играта.\n");
            break;
        }
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
    srand(time(NULL));
    int choice;
    while (1) {
        printf("\n=== МЕНЮ ===\n1. Нова игра\n2. Размер дъска (Сега: %dx%d)\n3. Replay\n4. Изход\nИзбор: ", boardSize, boardSize);
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }
        if (choice == 1) startNewGame();
        else if (choice == 2) changeBoardSize();
        else if (choice == 4) return 0;
    }
    return 0;
}
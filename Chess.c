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

typedef struct {
    char piece;
    int fromR, fromC, toR, toC;
    bool gaveCheck;
} Move;

typedef struct {
    Move* moves;
    int count, capacity;
    unsigned int seed;
    int boardSize;
} GameLog;

GameLog gameLog;

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

bool isRookPathClear(int fromR, int fromC, int toR, int toC) {
    if (fromR != toR && fromC != toC) return false;
    
    int stepR = (toR > fromR) ? 1 : (toR < fromR ? -1 : 0);
    int stepC = (toC > fromC) ? 1 : (toC < fromC ? -1 : 0);
    
    int currR = fromR + stepR;
    int currC = fromC + stepC;
    
    while (currR != toR || currC != toC) {
        if (board[currR][currC] != ' ') return false;
        currR += stepR;
        currC += stepC;
    }
    return true;
}

bool isSquareAttackedByPlayer(int r, int c) {
    if (abs(r - playerKing.r) <= 1 && abs(c - playerKing.c) <= 1) return true;
    if (rook1Alive && isRookPathClear(rook1.r, rook1.c, r, c)) return true;
    if (rook2Alive && isRookPathClear(rook2.r, rook2.c, r, c)) return true;
    return false;
}

void generatePositions() {
    do {
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
        rook1Alive = true;

        do {
            rook2.r = rand() % boardSize;
            rook2.c = rand() % boardSize;
        } while (board[rook2.r][rook2.c] != ' ');
        board[rook2.r][rook2.c] = 'W';
        rook2Alive = true;
    } while (isSquareAttackedByPlayer(compKing.r, compKing.c));
}

bool computerMakeMove() {
    int dr[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int dc[] = {-1,  0,  1, -1, 1, -1, 0,  1};
    Pos validMoves[8];
    bool isCaptureMove[8];
    int count = 0;

    for (int i = 0; i < 8; i++) {
        int nr = compKing.r + dr[i];
        int nc = compKing.c + dc[i];
        if (inBounds(nr, nc)) {
            if (board[nr][nc] == ' ' || board[nr][nc] == 'R' || board[nr][nc] == 'W') {
                char backup = board[nr][nc];
                Pos oldPos = compKing;
                bool oldRook1Alive = rook1Alive; bool oldRook2Alive = rook2Alive;
                if (backup == 'R') rook1Alive = false;
                if (backup == 'W') rook2Alive = false;

                board[compKing.r][compKing.c] = ' '; board[nr][nc] = 'k';
                compKing.r = nr; compKing.c = nc;

                if (!isSquareAttackedByPlayer(nr, nc) && !(abs(nr - playerKing.r) <= 1 && abs(nc - playerKing.c) <= 1)) {
                    validMoves[count] = compKing;
                    isCaptureMove[count] = (backup == 'R' || backup == 'W');
                    count++;
                }
                board[oldPos.r][oldPos.c] = 'k'; board[nr][nc] = backup; compKing = oldPos;
                rook1Alive = oldRook1Alive; rook2Alive = oldRook2Alive;
            }
        }
    }
    if (count > 0) {
        int chosenIndex = 0;
        for (int i = 0; i < count; i++) { if (isCaptureMove[i]) { chosenIndex = i; break; } }
        Pos nextPos = validMoves[chosenIndex];
        if (board[nextPos.r][nextPos.c] == 'R') rook1Alive = false;
        if (board[nextPos.r][nextPos.c] == 'W') rook2Alive = false;
        board[compKing.r][compKing.c] = ' '; compKing = nextPos; board[compKing.r][compKing.c] = 'k';
        return true; 
    }
    return false;
}

void initLog(unsigned int seed) {
    gameLog.seed = seed;
    gameLog.boardSize = boardSize;
    gameLog.count = 0;
    gameLog.capacity = 16;
    gameLog.moves = malloc(gameLog.capacity * sizeof(Move));
}

void logMove(char piece, int fromR, int fromC, int toR, int toC, bool gaveCheck) {
    if (gameLog.count == gameLog.capacity) {
        gameLog.capacity *= 2;
        gameLog.moves = realloc(gameLog.moves, gameLog.capacity * sizeof(Move));
    }
    Move m = { piece, fromR, fromC, toR, toC, gaveCheck };
    gameLog.moves[gameLog.count++] = m;
}

void freeLog() {
    if (gameLog.moves != NULL) {
        free(gameLog.moves);
        gameLog.moves = NULL;
    }
    gameLog.count = 0;
    gameLog.capacity = 0;
}

void coordToStr(int r, int c, int bSize, char* out) {
    out[0] = 'A' + c;
    sprintf(out + 1, "%d", bSize - r);
}

void printStats(GameLog* log) {
    printf("\n--- STATISTICS ---\n");
    printf("Total moves: %d\n", log->count);

    int countK = 0, countR = 0, countW = 0, countComp = 0, checks = 0;
    for (int i = 0; i < log->count; i++) {
        Move* m = &log->moves[i];
        if      (m->piece == 'K') countK++;
        else if (m->piece == 'R') countR++;
        else if (m->piece == 'W') countW++;
        else if (m->piece == 'k') countComp++;
        if (m->gaveCheck) checks++;
    }

    printf("Moves by K: %d\n", countK);
    printf("Moves by R: %d\n", countR);
    printf("Moves by W: %d\n", countW);
    printf("Moves by k (Computer): %d\n", countComp);
    printf("Times computer was in check: %d\n", checks);

    printf("\nPlayer Moves:\n");
    char from[8], to[8];
    for (int i = 0; i < log->count; i++) {
        Move* m = &log->moves[i];
        if (m->piece == 'k') continue;
        coordToStr(m->fromR, m->fromC, log->boardSize, from);
        coordToStr(m->toR,   m->toC,   log->boardSize, to);
        printf("  %c: %s -> %s%s\n", m->piece, from, to, m->gaveCheck ? " (CHECK!)" : "");
    }
}

void saveGame(GameLog* log, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) { printf("Error: Cannot open %s for writing!\n", filename); return; }
    fprintf(f, "%u\n%d\n%d\n", log->seed, log->boardSize, log->count);
    for (int i = 0; i < log->count; i++) {
        Move* m = &log->moves[i];
        fprintf(f, "%c %d %d %d %d %d\n", m->piece, m->fromR, m->fromC, m->toR, m->toC, m->gaveCheck ? 1 : 0);
    }
    fclose(f);
}

bool loadGame(GameLog* log, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) { printf("Error: Cannot open %s!\n", filename); return false; }
    
    unsigned int seed; int bSize, count;
    if (fscanf(f, "%u %d %d", &seed, &bSize, &count) != 3) { 
        fclose(f); 
        printf("Error: Corrupted file!\n"); 
        return false; 
    }
    
    log->seed = seed;
    log->boardSize = bSize;
    log->count = 0;
    log->capacity = count > 0 ? count : 1;
    log->moves = malloc(log->capacity * sizeof(Move));
    
    for (int i = 0; i < count; i++) {
        char piece; int fromR, fromC, toR, toC, check;
        if (fscanf(f, " %c %d %d %d %d %d", &piece, &fromR, &fromC, &toR, &toC, &check) != 6) {
            fclose(f); 
            printf("Error: Corrupted log entry at move %d!\n", i + 1); 
            return false;
        }
        
        if (log->count == log->capacity) {
            log->capacity *= 2;
            log->moves = realloc(log->moves, log->capacity * sizeof(Move));
        }
        Move m = { piece, fromR, fromC, toR, toC, check == 1 };
        log->moves[log->count++] = m;
    }
    fclose(f);
    return true;
}

void applyMove(Move* m) {
    char dest = board[m->toR][m->toC];
    if (dest == 'R') rook1Alive = false;
    if (dest == 'W') rook2Alive = false;
    board[m->fromR][m->fromC] = ' ';
    board[m->toR][m->toC] = m->piece;
    if (m->piece == 'K') { playerKing.r = m->toR; playerKing.c = m->toC; }
    else if (m->piece == 'R') { rook1.r = m->toR; rook1.c = m->toC; }
    else if (m->piece == 'W') { rook2.r = m->toR; rook2.c = m->toC; }
    else if (m->piece == 'k') { compKing.r = m->toR; compKing.c = m->toC; }
}

void replayGame(const char* filename) {
    GameLog log;
    log.moves = NULL;
    if (!loadGame(&log, filename)) return;
    
    boardSize = log.boardSize;
    srand(log.seed);
    generatePositions();
    
    printf("\n--- REPLAY (Press Enter for next move) ---\n");
    printBoard();

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF); 

    for (int i = 0; i < log.count; i++) {
        printf("\n[Press Enter to see move %d/%d...]", i + 1, log.count);
        while (getchar() != '\n'); 
        
        applyMove(&log.moves[i]);
        printBoard();
        if (log.moves[i].gaveCheck) printf("[CHECK!]\n");
    }
    
    printf("\n--- END OF REPLAY ---\n");
    free(log.moves);
}

void startNewGame() {
    unsigned int seed = (unsigned int)time(NULL);
    srand(seed);
    generatePositions();
    initLog(seed);
    printf("\n--- GAME STARTED ---\n");
    while (1) {
        printBoard();
        if (isSquareAttackedByPlayer(compKing.r, compKing.c)) printf("\n[WARNING] The computer is in CHECK!\n");

        printf("\nEnter your move (e.g., A2 A4): ");
        char col1, col2; int row1, row2;
        if (scanf(" %c%d %c%d", &col1, &row1, &col2, &row2) != 4) {
            printf("Error: Invalid input format!\n"); while (getchar() != '\n'); continue;
        }
        int fromC = col1 - (col1 >= 'a' ? 'a' : 'A'); int fromR = boardSize - row1;
        int toC = col2 - (col2 >= 'a' ? 'a' : 'A'); int toR = boardSize - row2;

        if (!inBounds(fromR, fromC) || !inBounds(toR, toC)) { printf("Error: Coordinates out of bounds!\n"); continue; }
        char piece = board[fromR][fromC];
        if (piece != 'K' && piece != 'R' && piece != 'W') { printf("Error: There is no piece belonging to you at that square!\n"); continue; }
        if (board[toR][toC] == 'K' || board[toR][toC] == 'R' || board[toR][toC] == 'W') { printf("Error: You cannot capture your own piece!\n"); continue; }

        bool validMove = false;
        if (piece == 'K') { 
            if (abs(toR - fromR) <= 1 && abs(toC - fromC) <= 1) {
                if (abs(toR - compKing.r) > 1 || abs(toC - compKing.c) > 1) validMove = true;
                else printf("Error: Kings cannot stand on adjacent squares!\n");
            }
        } else { 
            if (isRookPathClear(fromR, fromC, toR, toC)) validMove = true; 
        }

        if (!validMove) { printf("Error: Invalid chess move!\n"); continue; }

        board[fromR][fromC] = ' '; board[toR][toC] = piece;
        if (piece == 'K') { playerKing.r = toR; playerKing.c = toC; }
        if (piece == 'R') { rook1.r = toR; rook1.c = toC; }
        if (piece == 'W') { rook2.r = toR; rook2.c = toC; }

        bool gaveCheck = isSquareAttackedByPlayer(compKing.r, compKing.c);
        logMove(piece, fromR, fromC, toR, toC, gaveCheck);

        Pos compBefore = compKing;
        bool compMoved = computerMakeMove();
        if (compMoved) logMove('k', compBefore.r, compBefore.c, compKing.r, compKing.c, false);

        if (!rook1Alive && !rook2Alive) { 
            printBoard(); 
            printf("\nDRAW: Insufficient mating material left!\n"); 
            printStats(&gameLog); saveGame(&gameLog, "save.txt"); freeLog(); break; 
        }

        if (!compMoved) {
            printBoard();
            if (isSquareAttackedByPlayer(compKing.r, compKing.c)) printf("\nVICTORY! CHECKMATE!\n");
            else printf("\nSTALEMATE! The game is a draw.\n");
            printStats(&gameLog); saveGame(&gameLog, "save.txt"); freeLog(); break;
        }
    }
}

void changeBoardSize() {
    int newSize;
    printf("\nEnter new size (4-%d): ", MAX_SIZE);
    if (scanf("%d", &newSize) == 1 && newSize >= 4 && newSize <= MAX_SIZE) {
        boardSize = newSize;
        printf("Board size successfully updated to %dx%d!\n", boardSize, boardSize);
    }
}

int main() {
    int choice;
    while (1) {
        printf("\n=== MENU ===\n1. New Game\n2. Board Size (Current: %dx%d)\n3. Replay\n4. Exit\nChoice: ", boardSize, boardSize);
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }
        if (choice == 1) startNewGame();
        else if (choice == 2) changeBoardSize();
        else if (choice == 3) replayGame("save.txt");
        else if (choice == 4) return 0;
    }
    return 0;
}
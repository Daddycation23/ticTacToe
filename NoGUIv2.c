#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>

char board[3][3];
const char PLAYER = 'X';
const char COM = 'O';

void resetBoard() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++)
            board[i][j] = ' ';
    }
}

void printBoard() {
    printf("  %c  |  %c  |  %c  \n", board[0][0], board[0][1], board[0][2]);
    printf("-----|-----|-----\n");
    printf("  %c  |  %c  |  %c  \n", board[1][0], board[1][1], board[1][2]);
    printf("-----|-----|-----\n");
    printf("  %c  |  %c  |  %c  \n", board[2][0], board[2][1], board[2][2]);
}


int checkFreeSpaces() {
    int freeSpaces = 9;

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if (board[i][j] != ' ') {
                freeSpaces--;
            }
        }
    }
    return freeSpaces;
}

void playerMove() {
    int x;
    int y;
    do {
        printf("Enter row# (1-3), 'space' and column# (1-3)(eg: 1 1): ");
        scanf("%d %d", &x, &y);
        x--, y--;

        if (board[x][y] != ' ') {
            printf("Invalid move, space is occupied!\n");
        } else {
            board[x][y] = PLAYER;
            break;
        }
    } while (board[x][y] != ' ');
}

void computerMove() {
    printf("Computer's turn...\n");
    srand(time(0));
    int x, y;

    if(checkFreeSpaces() > 0) {
        do {
            x = rand() % 3;
            y = rand() % 3;
        } while (board[x][y] != ' ');

        board[x][y] = COM;
    }
}

char checkWinner() {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if(board[i][0] == board[i][1] && board[i][0] == board[i][2] && board[i][0] != ' ') {
            return board[i][0];
        }
    }
    // Check columns
    for (int j = 0; j < 3; j++) {
        if(board[0][j] == board[1][j] && board[0][j] == board[2][j] && board[0][j] != ' ') {
            return board[0][j];
        }
    }
    // Check diagonals
    if(board[0][0] == board[1][1] && board[0][0] == board[2][2] && board[0][0] != ' ') {
        return board[0][0];
    }

    if(board[2][0] == board[1][1] && board[2][0] == board[0][2] && board[2][0] != ' ') {
        return board[2][0];
    }

    return ' ';
}

void printWinner(char winner) {
    if (winner == PLAYER) {
        printf("YAY u win!\n");
    } else if (winner == COM) {
        printf("U so Noob. Com win\n");
    } else {
        printf("DRAWWWWWW\n");
    }
}

int main(void) {
    char winner = ' ';
    char response = ' ';
    do {
        resetBoard();
        winner = ' ';
        response = ' ';
        while(winner == ' ' && checkFreeSpaces() != 0) {
            playerMove();

            // Check winner after player's move
            winner = checkWinner();
            printBoard();
            if (winner != ' ' || checkFreeSpaces() == 0) {
                break;
            }

            computerMove();
            Sleep(2000);
            printBoard();

            // Check winner after computer's move
            winner = checkWinner();
            if (winner != ' ' || checkFreeSpaces() == 0) {
                break;
            }
        }
        printWinner(winner);
        printf("Play again?(y/n): ");
        scanf(" %c", &response); // Added a space before %c to avoid issues with reading newline
        response = tolower(response);
    } while (response == 'y');

    return 0;
}

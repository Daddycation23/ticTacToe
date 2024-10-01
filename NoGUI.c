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
    printf(" %c   |  %c  |  %c ", board[0][0], board[0][1], board[0][2]);
    printf("\n --- | --- | ---\n");
    printf(" %c   | %c   |  %c ", board[1][0], board[1][1], board[1][2]);
    printf("\n --- | --- | ---\n");
    printf(" %c   | %c   |  %c ", board[2][0], board[2][1], board[2][2]);
    printf("\n");
}
int checkFreeSpaces() {
    int freeSpaces = 9;

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if (board[i][j] != ' ') {
                freeSpaces--;
            }
        }
    } return freeSpaces;
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
    srand(time(0));
    int x;
    int y;

    if(checkFreeSpaces() > 0) {
        do {
            x = rand() % 3;
            y = rand() % 3;
        } while (board[x][y] != ' ');

        board[x][y] = COM;
    }
    else {
        printWinner();
    }
}
char checkWinner() {
    //check rows
    for (int i = 0; i < 3; i++) {
        if(board[i][0] == board[i][1] && board[i][0] == board[i][2]) {
            return board[i][0];
        }
    }
    //check columns
    for (int j = 0; j < 3; j++) {
        if(board[0][j] == board[1][j] && board[0][j] == board[2][j]) {
            return board[0][j];
        }
    }
    //check diagonals
    if(board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
        return board[0][0];
    }

    if(board[2][0] == board[1][1] && board[2][0] == board[0][2]) {
        return board[2][0];
    }

    return ' ';
}

void printWinner(char winner) {
    if (winner == PLAYER) {
        printf("YAY u win!");
    } else if (winner == COM) {
        printf("U so Noob. Com win");
    } else {
        printf("DRAWWWWWW");
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
            printBoard();
            playerMove();
            if (winner != ' ' || checkFreeSpaces() == 0) {
                break;
            }
            printBoard();
            computerMove();
            Sleep(2000);
            if (winner != ' ' || checkFreeSpaces() == 0) {
                break;
            }
            winner = checkWinner();
        }
        printBoard();
        printWinner(winner);
        printf("Play again?(y/n): ");
        scanf("%s", &response);
        response = tolower(response);
    } while (response == 'y');
    return 0;
}

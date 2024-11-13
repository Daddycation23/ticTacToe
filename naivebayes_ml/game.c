#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "naivebayes.h"

// AI starts first
// void play_game(NaiveBayesModel *model) {
//     char board[NUM_POSITIONS + 1] = "bbbbbbbbb"; // Empty board

//     printf("Welcome to Tic-Tac-Toe! You're 'o' and the AI is 'x'.\n");
//     print_board(board);

//     for (int turn = 0; turn < NUM_POSITIONS; turn++) {
//         int move;
//         if (turn % 2 == 0) { // AI's turn
//             move = predict_move(model, board);
//             if (move == -1) {
//                 printf("AI cannot make a move.\n");
//                 break;
//             }
//             board[move] = 'x';
//             printf("AI chose position %d:\n", move + 1);
//             print_board(board);
//         } else { // Human's turn
//             printf("Your turn! Enter position (1-9): ");
//             scanf("%d", &move);
//             move--; // Adjust for zero-based index

//             if (move < 0 || move >= NUM_POSITIONS || board[move] != 'b') {
//                 printf("Invalid move. Try again.\n");
//                 turn--; // Retry the turn
//                 continue;
//             }
//             board[move] = 'o';
//             print_board(board);
//         }

//         // Check for a winner
//         int winner = check_winner(board);
//         if (winner != 0) {
//             if (winner == 1) printf("AI wins!\n");
//             else printf("You win!\n");
//             return;
//         }
//     }
//     printf("It's a draw!\n");
// }

// Player starts first
void play_game(NaiveBayesModel *model) {
    char board[NUM_POSITIONS + 1] = "bbbbbbbbb"; // Empty board

    printf("Welcome to Tic-Tac-Toe! You're 'o' and the AI is 'x'.\n");
    print_board(board);

    for (int turn = 0; turn < NUM_POSITIONS; turn++) {
        int move;
        if (turn % 2 == 0) { // Player's turn
            printf("Your turn! Enter position (1-9): ");
            scanf("%d", &move);
            move--; // Adjust for zero-based index

            if (move < 0 || move >= NUM_POSITIONS || board[move] != 'b') {
                printf("Invalid move. Try again.\n");
                turn--; // Retry the turn
                continue;
            }
            board[move] = 'o';
            print_board(board);
        } else { // AI's turn
            move = predict_move(model, board);
            if (move == -1) {
                printf("AI cannot make a move.\n");
                break;
            }
            board[move] = 'x';
            printf("AI chose position %d:\n", move + 1);
            print_board(board);
        }

        // Check for a winner
        int winner = check_winner(board);
        if (winner != 0) {
            if (winner == 1) printf("AI wins!\n");
            else printf("You win!\n");
            return;
        }
    }
    printf("It's a draw!\n");
}

int check_winner(const char board[]) {
    int win_patterns[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Rows
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Columns
        {0, 4, 8}, {2, 4, 6}             // Diagonals
    };

    for (int i = 0; i < 8; i++) {
        int a = win_patterns[i][0], b = win_patterns[i][1], c = win_patterns[i][2];
        if (board[a] != 'b' && board[a] == board[b] && board[b] == board[c]) {
            return (board[a] == 'x') ? 1 : -1;
        }
    }
    return 0; // No winner
}

void print_board(const char board[]) {
    for (int i = 0; i < NUM_POSITIONS; i++) {
        printf("%c ", board[i] == 'b' ? '.' : board[i]);
        if ((i + 1) % 3 == 0) printf("\n");
    }
    printf("\n");
}
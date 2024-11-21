#include "main.h"

// minimax algorithm, factorial design
int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit, int alpha, int beta) {
    if (depth >= depthLimit) return 0; // Return 0 if depth limit is reached
    int score = EvaluateBoard(board);
    if (score == 10) return score - depth; // O (AI) is the maximizing player
    if (score == -10) return score + depth; // X (human) is the minimizing player
    if (CheckDraw()) return 0; // Draw

    if (isMaximizing) {
        int bestScore = -1000; // Initialize the best score to a very low value
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) { // Iterate through each cell in the grid
                if (board[i][j] == EMPTY) { // If the cell is empty
                    board[i][j] = PLAYER_O; // Set the cell to PLAYER_O
                    bestScore = fmax(bestScore, Minimax(board, false, depth + 1, depthLimit, alpha, beta)); // Update the best score
                    board[i][j] = EMPTY; // Reset the cell to EMPTY
                    alpha = fmax(alpha, bestScore); // Update alpha (maximize)
                    if (beta <= alpha) break; // Beta cut-off (prune the branch, stopping the recursion)
                    
                }
            }
        }
        return bestScore;
    } else {
        int bestScore = 1000; // Initialize the best score to a very high value
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) { // Iterate through each cell in the grid
                if (board[i][j] == EMPTY) { // If the cell is empty
                    board[i][j] = PLAYER_X; // Set the cell to PLAYER_X
                    bestScore = fmin(bestScore, Minimax(board, true, depth + 1, depthLimit, alpha, beta)); // Update the best score
                    board[i][j] = EMPTY; // Reset the cell to EMPTY
                    beta = fmin(beta, bestScore); // Update beta (minimize)
                    if (beta <= alpha) break; // Alpha cut-off (prune the branch, stopping the recursion)
                }
            }
        }
        return bestScore; // Return the best score
    }
}

// Evaluate the board
int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE]) {
    // Check rows and columns for a win
    for (int row = 0; row < GRID_SIZE; row++) {
        if (board[row][0] == board[row][1] && board[row][0] == board[row][2]) {
            if (board[row][0] == PLAYER_O) return 10;
            else if (board[row][0] == PLAYER_X) return -10;
        }
    }
    for (int col = 0; col < GRID_SIZE; col++) {
        if (board[0][col] == board[1][col] && board[0][col] == board[2][col]) {
            if (board[0][col] == PLAYER_O) return 10;
            else if (board[0][col] == PLAYER_X) return -10;
        }
    }
    // Check diagonals for a win
    if (board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
        if (board[0][0] == PLAYER_O) return 10;
        else if (board[0][0] == PLAYER_X) return -10;
    }
    if (board[0][2] == board[1][1] && board[0][2] == board[2][0]) {
        if (board[0][2] == PLAYER_O) return 10;
        else if (board[0][2] == PLAYER_X) return -10;
    }

    return 0; // No winner
}
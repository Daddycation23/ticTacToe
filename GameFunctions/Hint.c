#include "../main.h"

// Declare external variables used in Hint.c
extern struct GetHint hint;
extern Cell grid[GRID_SIZE][GRID_SIZE];

// Clear Hint best move
void clearHint() {
    hint.row = -1;
    hint.col = -1;
}

// Get Hint
void getHint() {
    int bestScore = -1000; // Initialize the best score to a very low value
    int bestRow = -1;
    int bestCol = -1;
    int depthLimit = 9; // Full depth
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) { // Iterate through each cell in the grid
            if (grid[i][j] == EMPTY) { // If the cell is empty
                grid[i][j] = PLAYER_O; // Set the cell to PLAYER_O
                int score = Minimax(grid, false, 0, depthLimit, -1000, 1000); // Set the score
                grid[i][j] = EMPTY; // Clear the cell
                // Save best move
                if (score > bestScore) {
                    bestScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }
    // save best move
    if (bestRow != -1 && bestCol != -1) {
        hint.row = bestRow;
        hint.col = bestCol;
    }
}


#include "main.h"
extern struct GetHint hint;
extern Cell grid[GRID_SIZE][GRID_SIZE];
// Clear Hint best move
void clearHint() {
    hint.row = -1;
    hint.col = -1;
}
// Get Hint
void getHint() {
    int bestScore = -1000;
    int bestRow = -1;
    int bestCol = -1;
    int depthLimit = 9; // Full depth
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == EMPTY) {
                grid[i][j] = PLAYER_O;
                int score = Minimax(grid, false, 0, depthLimit, -1000, 1000);
                grid[i][j] = EMPTY;

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


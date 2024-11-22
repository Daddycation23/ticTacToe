#include "main.h"

// Declare external variables used in Check.c
extern Cell grid[GRID_SIZE][GRID_SIZE];
extern int winningCells[3][2];

// Function to check if a player has won
bool CheckWin(Cell player) {
    // Check rows
    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid[i][0] == player && grid[i][1] == player && grid[i][2] == player) {
            winningCells[0][0] = i; winningCells[0][1] = 0;
            winningCells[1][0] = i; winningCells[1][1] = 1;
            winningCells[2][0] = i; winningCells[2][1] = 2;
            return true;
        }
    }
    
    // Check columns
    for (int i = 0; i < GRID_SIZE; i++) {
        if (grid[0][i] == player && grid[1][i] == player && grid[2][i] == player) {
            winningCells[0][0] = 0; winningCells[0][1] = i;
            winningCells[1][0] = 1; winningCells[1][1] = i;
            winningCells[2][0] = 2; winningCells[2][1] = i;
            return true;
        }
    }

    // Check main diagonal
    if (grid[0][0] == player && grid[1][1] == player && grid[2][2] == player) {
        winningCells[0][0] = 0; winningCells[0][1] = 0;
        winningCells[1][0] = 1; winningCells[1][1] = 1;
        winningCells[2][0] = 2; winningCells[2][1] = 2;
        return true;
    }

    // Check anti diagonal
    if (grid[0][2] == player && grid[1][1] == player && grid[2][0] == player) {
        winningCells[0][0] = 0; winningCells[0][1] = 2;
        winningCells[1][0] = 1; winningCells[1][1] = 1;
        winningCells[2][0] = 2; winningCells[2][1] = 0;
        return true;
    }

    return false;
}

// Function to check if the game is a draw
bool CheckDraw() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == EMPTY) return false; // If there's an empty cell, it's not a draw
        }
    }
    return true; // All cells are filled
}
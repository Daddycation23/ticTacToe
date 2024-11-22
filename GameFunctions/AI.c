#include "../main.h"
#include "../DecisionTree_ML/decisiontree.h"
extern Cell grid[GRID_SIZE][GRID_SIZE];

// AI's turn using MiniMax algorithms and Machine Learning models
void AITurn(Sound victorySound, Sound loseSound, Sound drawSound, NaiveBayesModel *model)
{
    int bestScore = -1000;
    int bestRow = -1;
    int bestCol = -1;

    // Ensure this function only applies in medium and hard modes
    if (currentDifficulty == EASY) {
        predict_move(model, grid, &bestRow, &bestCol);
    }

    // Medium mode: use Minimax with limited depth search of 4
    else if (currentDifficulty == MEDIUM) {
        int depthLimit = 4; // Set a depth limit for medium difficulty
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
    }
    // Hard mode: full Minimax search
    else if (currentDifficulty == HARD) {
        int depthLimit = 9; // Full depth for hard mode
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
    }

    // Ensure a move is made
    if (bestRow != -1 && bestCol != -1) {
        grid[bestRow][bestCol] = PLAYER_O;
    }

    // Get the current mode's stats
    ModeStats* currentStats = GetCurrentModeStats();

    if (CheckWin(PLAYER_O)) {
        gameOver = true;
        winner = PLAYER_O;
        gameState = GAME_OVER;
        
        // Update the stats counter
        currentStats->aiWins++;
        currentStats->totalGames++;

        // Play sound immediately when a winner is detected
        if (!isTwoPlayer) {
            PlaySound(loseSound);  // Play lose sound for Player O
        } else {
            PlaySound(victorySound);  // Play victory sound for any winner in two-player mode
        }

    } 
    else if (CheckDraw()) {
        gameOver = true;
        gameState = GAME_OVER;
        winner = EMPTY;
        
        // Update the stats counter
        currentStats->draws++;
        currentStats->totalGames++;

        PlaySound(drawSound);  // Play draw sound
    } 
    else {
        currentPlayerTurn = PLAYER_X_TURN;
    }
}

// Decision Tree AI Turn function
void AITurnDecisionTree(Sound victorySound, Sound loseSound, Sound drawSound, DecisionTreeNode *TDmodel) {
    int bestScore = -1000;       // Initialize best score for evaluating moves
    int bestRow = -1;            // Initialize best row for AI move
    int bestCol = -1;            // Initialize best column for AI move
    int row, col;                // Variables for random fallback move
    double best_prob = 0.0;      // Probability of the best move
    char board[3][3];            // Buffer array for board layout

    // Convert the current grid state into a compatible format for the decision tree
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i][j] == EMPTY) {
                board[i][j] = 'b';  // Convert EMPTY cells to 'b' (blank)
            } else if (grid[i][j] == PLAYER_X) {
                board[i][j] = 'x';  // Convert PLAYER_X cells to 'x'
            } else if (grid[i][j] == PLAYER_O) {
                board[i][j] = 'o';  // Convert PLAYER_O cells to 'o'
            }
        }
    }

    // Print the decision tree structure for debugging
    print_tree(TDmodel, 2);

    // Use the decision tree to predict the best move for the AI
    dt_predict_best_move(TDmodel, board, PLAYER_O, &bestRow, &bestCol);

    // Fallback logic: Choose a random empty cell if the decision tree fails
    do {
        row = GetRandomValue(0, GRID_SIZE - 1);  // Generate random row index
        col = GetRandomValue(0, GRID_SIZE - 1);  // Generate random column index
    } while (grid[row][col] != EMPTY);           // Ensure the chosen cell is empty

    // Place the AI's move on the grid at the predicted or random position
    grid[bestRow][bestCol] = PLAYER_O;

    // Get the current stats for Easy mode
    ModeStats* currentStats = &decisionTreeStats;

    // Check if the AI's move results in a win
    if (CheckWin(PLAYER_O)) {
        gameOver = true;               // Mark the game as over
        winner = PLAYER_O;             // Set the winner to PLAYER_O
        gameState = GAME_OVER;         // Transition to GAME_OVER state
        currentStats->aiWins++;        // Increment AI win count
        currentStats->totalGames++;    // Increment total games count

        PlaySound(loseSound);          // Play losing sound for the player
    }
    // Check if the game results in a draw
    else if (CheckDraw()) {
        gameOver = true;               // Mark the game as over
        gameState = GAME_OVER;         // Transition to GAME_OVER state
        winner = EMPTY;                // Set the winner to NONE (draw)
        currentStats->draws++;         // Increment draw count
        currentStats->totalGames++;    // Increment total games count

        PlaySound(drawSound);          // Play draw sound
    }
    // If the game continues, pass the turn to the player
    else {
        currentPlayerTurn = PLAYER_X_TURN; // Set the turn to PLAYER_X
    }
}
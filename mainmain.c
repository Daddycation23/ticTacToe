#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 40
#define GRID_SIZE 3
#define CELL_SIZE (SCREEN_WIDTH / GRID_SIZE)
#define FEATURES 9 // Number of features (board positions)

typedef enum { EMPTY, PLAYER_X, PLAYER_O } Cell;
typedef enum { PLAYER_X_TURN, PLAYER_O_TURN } PlayerTurn;
typedef enum { MENU, DIFFICULTY_SELECT, GAME, GAME_OVER, AI_ANALYSIS } GameState;
typedef enum { EASY, MEDIUM, HARD } Difficulty;

typedef struct {
    int wins;
    int losses;
    int draws;
    int totalGames;
} DifficultyStats;

typedef struct {
    int tp, tn, fp, fn; // True Positives, True Negatives, False Positives, False Negatives
} ConfusionMatrix;

typedef struct {
    int correctPredictions;
    int totalPredictions;
} AccuracyResult;

DifficultyStats easyStats = {0, 0, 0, 0};
DifficultyStats mediumStats = {0, 0, 0, 0};
DifficultyStats hardStats = {0, 0, 0, 0};

Difficulty currentDifficulty = MEDIUM; // Default difficulty
Cell grid[GRID_SIZE][GRID_SIZE];
PlayerTurn currentPlayerTurn = PLAYER_X_TURN;
bool gameOver = false;
Cell winner = EMPTY;
GameState gameState = MENU;
bool isTwoPlayer = false; // Flag to check if it's a two-player or single-player game

ConfusionMatrix confusionMatrix = {0, 0, 0, 0};
float trainingAccuracy = 0.0f;
float testingAccuracy = 0.0f;

// Declare scroll variables
static float scrollY = 0.0f;
static const float scrollSpeed = 20.0f;

void InitGame();
void UpdateGame();
void HandlePlayerTurn();
void AITurn();
void DrawGame();
void DrawDifficultySelect(void);
bool CheckWin(Cell player);
bool CheckDraw();
void DrawMenu();
void DrawGameOver();
void LoadAndEvaluateDataset(void);
void DisplayDifficultyStats(void);

int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth);
int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE]);

void DrawAIAnalysis();
void DrawDifficultySection(const char* difficulty, DifficultyStats stats, int* y, Color color, int padding, int textFontSize);
void DrawButton(Rectangle bounds, const char* text, int fontSize, bool isHovered);

// Linear Regression Functions
void TrainLinearRegression(float weights[FEATURES + 1], float learningRate, int epochs);
float PredictLinearRegression(float weights[FEATURES + 1], float features[FEATURES]);
void EvaluateLinearRegression(float weights[FEATURES + 1]);

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic-Tac-Toe");

    // Initialize weights for linear regression
    float weights[FEATURES + 1] = {0}; // +1 for the bias term
    TrainLinearRegression(weights, 0.01, 1000); // Train the model
    EvaluateLinearRegression(weights); // Evaluate the model

    while (!WindowShouldClose())
    {
        if (gameState == MENU) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                // Single Player button
                if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
                    mousePos.y >= SCREEN_HEIGHT/2 && mousePos.y <= SCREEN_HEIGHT/2 + 40) {
                    isTwoPlayer = false;
                    gameState = DIFFICULTY_SELECT;  // go to difficulty selection instead of game
                }
                // Two Player button
                else if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
                        mousePos.y >= SCREEN_HEIGHT/2 + 60 && mousePos.y <= SCREEN_HEIGHT/2 + 100) {
                    isTwoPlayer = true;
                    gameState = GAME;
                    InitGame();
                }
                // AI Analysis button
                else if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
                    mousePos.y >= SCREEN_HEIGHT/2 + 120 && mousePos.y <= SCREEN_HEIGHT/2 + 160) {
                    LoadAndEvaluateDataset();
                    gameState = AI_ANALYSIS;  // Change to AI Analysis state instead of just displaying stats
                }
            }
        }
        else if (gameState == GAME)
        {
            UpdateGame();
        }
        else if (gameState == GAME_OVER)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mousePos = GetMousePosition();
                Rectangle menuBtn = {
                    SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
                    SCREEN_HEIGHT/2 + 40,  // Match the position in DrawGameOver
                    BUTTON_WIDTH,
                    BUTTON_HEIGHT
                };
                
                if (CheckCollisionPointRec(mousePos, menuBtn))
                {
                    gameState = MENU;
                    InitGame();  // Reset the game state
                }
            }
        }
        else if (gameState == DIFFICULTY_SELECT) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (mousePos.x >= SCREEN_WIDTH/2 - BUTTON_WIDTH/2 && 
                    mousePos.x <= SCREEN_WIDTH/2 + BUTTON_WIDTH/2) {
                    // easy button
                    if (mousePos.y >= SCREEN_HEIGHT/2 && 
                        mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT) {
                        currentDifficulty = EASY;
                        gameState = GAME;
                        InitGame();
                    }
                    // medium button
                    else if (mousePos.y >= SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20 && 
                             mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT * 2 + 20) {
                        currentDifficulty = MEDIUM;
                        gameState = GAME;
                        InitGame();
                    }
                    // hard button
                    else if (mousePos.y >= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 && 
                             mousePos.y <= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 + BUTTON_HEIGHT) {
                        currentDifficulty = HARD;
                        gameState = GAME;
                        InitGame();
                    }
                }
            }
        }
        else if (gameState == AI_ANALYSIS) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                Rectangle backBtn = {
                    SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
                    SCREEN_HEIGHT - BUTTON_HEIGHT - 20,
                    BUTTON_WIDTH,
                    BUTTON_HEIGHT
                };
                
                if (CheckCollisionPointRec(mousePos, backBtn)) {
                    gameState = MENU;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch(gameState) {
            case MENU:
                DrawMenu();
                break;
            case DIFFICULTY_SELECT:
                DrawDifficultySelect();
                break;
            case GAME:
                DrawGame();
                break;
            case GAME_OVER:
                DrawGame();
                DrawGameOver();
                break;
            case AI_ANALYSIS:
                DrawAIAnalysis();
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// Linear Regression Training
void TrainLinearRegression(float weights[FEATURES + 1], float learningRate, int epochs) {
    FILE *file = fopen("tic-tac-toe.data", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    char line[100];
    for (int epoch = 0; epoch < epochs; epoch++) {
        rewind(file); // Reset file pointer to the beginning
        while (fgets(line, sizeof(line), file)) {
            float features[FEATURES] = {0};
            int outcome = (line[9] == '1') ? 1 : 0; // Convert outcome to 0 or 1

            // Convert board state to features
            for (int i = 0; i < FEATURES; i++) {
                if (line[i] == 'x') features[i] = 1.0;
                else if (line[i] == 'o') features[i] = -1.0;
                else features[i] = 0.0;
            }

            // Calculate prediction
            float prediction = PredictLinearRegression(weights, features);

            // Update weights
            for (int i = 0; i < FEATURES; i++) {
                weights[i] += learningRate * (outcome - prediction) * features[i];
            }
            weights[FEATURES] += learningRate * (outcome - prediction); // Update bias
        }
    }

    fclose(file);
}

// Linear Regression Prediction
float PredictLinearRegression(float weights[FEATURES + 1], float features[FEATURES]) {
    float result = weights[FEATURES]; // Start with bias
    for (int i = 0; i < FEATURES; i++) {
        result += weights[i] * features[i];
    }
    return result > 0.5 ? 1.0 : 0.0; // Threshold at 0.5
}

// Evaluate Linear Regression
void EvaluateLinearRegression(float weights[FEATURES + 1]) {
    FILE *file = fopen("tic-tac-toe.data", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    char line[100];
    confusionMatrix.tp = confusionMatrix.tn = confusionMatrix.fp = confusionMatrix.fn = 0;

    while (fgets(line, sizeof(line), file)) {
        float features[FEATURES] = {0};
        int outcome = (line[9] == '1') ? 1 : 0; // Convert outcome to 0 or 1

        // Convert board state to features
        for (int i = 0; i < FEATURES; i++) {
            if (line[i] == 'x') features[i] = 1.0;
            else if (line[i] == 'o') features[i] = -1.0;
            else features[i] = 0.0;
        }

        // Calculate prediction
        float prediction = PredictLinearRegression(weights, features);

        // Update confusion matrix
        if (prediction == 1.0 && outcome == 1) confusionMatrix.tp++;
        else if (prediction == 0.0 && outcome == 0) confusionMatrix.tn++;
        else if (prediction == 1.0 && outcome == 0) confusionMatrix.fp++;
        else if (prediction == 0.0 && outcome == 1) confusionMatrix.fn++;
    }

    fclose(file);

    // Print confusion matrix
    printf("Confusion Matrix:\n");
    printf("TP: %d, TN: %d, FP: %d, FN: %d\n", confusionMatrix.tp, confusionMatrix.tn, confusionMatrix.fp, confusionMatrix.fn);
}

// function to draw difficulty selection screen
void DrawDifficultySelect() {
    const int titleFontSize = 40;
    const int buttonFontSize = 20;
    
    // Title
    const char* title = "Select Difficulty";
    DrawText(title, 
        SCREEN_WIDTH/2 - MeasureText(title, titleFontSize)/2, 
        SCREEN_HEIGHT/3,
        titleFontSize,
        BLACK);
    
    // Button rectangles
    Rectangle easyBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Rectangle mediumBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Rectangle hardBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

    Vector2 mousePos = GetMousePosition();
    
    // Check hover states
    bool easyHover = CheckCollisionPointRec(mousePos, easyBtn);
    bool mediumHover = CheckCollisionPointRec(mousePos, mediumBtn);
    bool hardHover = CheckCollisionPointRec(mousePos, hardBtn);

    // Draw buttons with hover effects
    DrawButton(easyBtn, "Easy", buttonFontSize, easyHover);
    DrawButton(mediumBtn, "Medium", buttonFontSize, mediumHover);
    DrawButton(hardBtn, "Hard", buttonFontSize, hardHover);

    // Set cursor based on any button hover
    SetMouseCursor((easyHover || mediumHover || hardHover) ? 
        MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void DisplayDifficultyStats() {
    printf("\n=== Difficulty Level Statistics ===\n");
    
    // Easy Stats
    printf("\nEasy Mode:");
    printf("\nAI Wins: %d", easyStats.wins);
    printf("\nAI Losses: %d", easyStats.losses);
    printf("\nAI Draws: %d", easyStats.draws);
    printf("\nAI Win Rate: %.2f%%\n", easyStats.totalGames > 0 ? (float)easyStats.wins/easyStats.totalGames * 100 : 0);
    
    // Medium Stats
    printf("\nMedium Mode:");
    printf("\nAI Wins: %d", mediumStats.wins);
    printf("\nAI Losses: %d", mediumStats.losses);
    printf("\nAI Draws: %d", mediumStats.draws);
    printf("\nAI Win Rate: %.2f%%\n", mediumStats.totalGames > 0 ? (float)mediumStats.wins/mediumStats.totalGames * 100 : 0);
    
    // Hard Stats
    printf("\nHard Mode:");
    printf("\nAI Wins: %d", hardStats.wins);
    printf("\nAI Losses: %d", hardStats.losses);
    printf("\nAI Draws: %d", hardStats.draws);
    printf("\nAI Win Rate: %.2f%%\n", hardStats.totalGames > 0 ? (float)hardStats.wins/hardStats.totalGames * 100 : 0);
}

bool simulateGame(char* gameState, int expectedOutcome) {
    Cell simulatedGrid[GRID_SIZE][GRID_SIZE];
    
    // Convert string to grid state
    int index = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            char c = gameState[index++];
            if (c == 'x' || c == 'X') simulatedGrid[i][j] = PLAYER_X;
            else if (c == 'o' || c == 'O') simulatedGrid[i][j] = PLAYER_O;
            else simulatedGrid[i][j] = EMPTY;
        }
    }
    
    int prediction = Minimax(simulatedGrid, true, 0);
    return (prediction > 0) == (expectedOutcome == 1);
}

// Split the dataset into training and testing sets and calculate accuracy
void evaluateAccuracy(FILE *file) { 
    ConfusionMatrix trainingCM = {0, 0, 0, 0};
    ConfusionMatrix testingCM = {0, 0, 0, 0};
    AccuracyResult trainAcc = {0, 0}, testAcc = {0, 0};
    
    char line[100];
    int lineCount = 0;

    while (fgets(line, sizeof(line), file)) {
        int outcome = line[9] - '0'; // Example: Assume outcome is in column 10

        if (lineCount < 800) { // 80% Training
            if (simulateGame(line, outcome)) {
                trainAcc.correctPredictions++;
                if (outcome == 1) trainingCM.tp++; // True Positive
                else trainingCM.tn++; // True Negative
            } else {
                if (outcome == 1) trainingCM.fn++; // False Negative
                else trainingCM.fp++; // False Positive
            }
            trainAcc.totalPredictions++;
        } else { // 20% Testing
            if (simulateGame(line, outcome)) {
                testAcc.correctPredictions++;
                if (outcome == 1) testingCM.tp++; // True Positive
                else testingCM.tn++; // True Negative
            } else {
                if (outcome == 1) testingCM.fn++; // False Negative
                else testingCM.fp++; // False Positive
            }
            testAcc.totalPredictions++;
        }
        lineCount++;
    }

    // Calculate and print training and testing accuracy
    printf("Training Accuracy: %.2f%%\n", (float)trainAcc.correctPredictions / trainAcc.totalPredictions * 100);
    printf("Testing Accuracy: %.2f%%\n", (float)testAcc.correctPredictions / testAcc.totalPredictions * 100);

    // Print confusion matrix
    printf("Training Confusion Matrix:\n");
    printf("TP: %d, TN: %d, FP: %d, FN: %d\n", trainingCM.tp, trainingCM.tn, trainingCM.fp, trainingCM.fn);
    printf("Testing Confusion Matrix:\n");
    printf("TP: %d, TN: %d, FP: %d, FN: %d\n", testingCM.tp, testingCM.tn, testingCM.fp, testingCM.fn);
}

void LoadDataset() {
    FILE *file = fopen("tic-tac-toe.data", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Print only summary statistics
    int totalGames = 0;
    char line[100];
    while (fgets(line, sizeof(line), file)) {
        totalGames++;
    }
    printf("\nTotal games in dataset: %d\n", totalGames);
    fclose(file);
}

// Call this function after loading the dataset
void LoadAndEvaluateDataset() {
    FILE *file = fopen("tic-tac-toe.data", "r");
    if (file == NULL) {
        return;
    }

    int totalCorrectTrain = 0;
    int totalTrain = 0;
    int totalCorrectTest = 0;
    int totalTest = 0;
    
    char line[100];
    int lineCount = 0;

    // Reset confusion matrix
    confusionMatrix.tp = 0;
    confusionMatrix.tn = 0;
    confusionMatrix.fp = 0;
    confusionMatrix.fn = 0;

    while (fgets(line, sizeof(line), file)) {
        int outcome = line[9] - '0';

        if (lineCount < 800) { // 80% Training
            if (simulateGame(line, outcome)) {
                totalCorrectTrain++;
                if (outcome == 1) confusionMatrix.tp++;
                else confusionMatrix.tn++;
            } else {
                if (outcome == 1) confusionMatrix.fn++;
                else confusionMatrix.fp++;
            }
            totalTrain++;
        } else { // 20% Testing
            if (simulateGame(line, outcome)) {
                totalCorrectTest++;
            }
            totalTest++;
        }
        lineCount++;
    }

    trainingAccuracy = (float)totalCorrectTrain / totalTrain * 100;
    testingAccuracy = (float)totalCorrectTest / totalTest * 100;

    fclose(file);
}

void HandlePlayerTurn()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        int row = (int)(mousePos.y / CELL_SIZE);
        int col = (int)(mousePos.x / CELL_SIZE);

        if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE)
        {
            if (grid[row][col] == EMPTY)
            {
                grid[row][col] = (currentPlayerTurn == PLAYER_X_TURN) ? PLAYER_X : PLAYER_O;
                if (CheckWin(grid[row][col]))
                {
                    gameOver = true;
                    winner = grid[row][col];
                    gameState = GAME_OVER;
                    
                    // Track AI losses when player wins
                    if (winner == PLAYER_X && !isTwoPlayer) {
                        switch(currentDifficulty) {
                            case EASY: easyStats.losses++; easyStats.totalGames++; break;
                            case MEDIUM: mediumStats.losses++; mediumStats.totalGames++; break;
                            case HARD: hardStats.losses++; hardStats.totalGames++; break;
                        }
                    }
                }
                else if (CheckDraw())
                {
                    gameOver = true;
                    gameState = GAME_OVER;
                    switch(currentDifficulty) {
                        case EASY: easyStats.draws++; easyStats.totalGames++; break;
                        case MEDIUM: mediumStats.draws++; mediumStats.totalGames++; break;
                        case HARD: hardStats.draws++; hardStats.totalGames++; break;
                    }
                }
                else
                {
                    currentPlayerTurn = (currentPlayerTurn == PLAYER_X_TURN) ? PLAYER_O_TURN : PLAYER_X_TURN;
                }
            }
        }
    }
}

void UpdateGame()
{
    if (gameOver) return;

    // quit button click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
            mousePos.y >= 10 && mousePos.y <= 40)
        {
            DisplayDifficultyStats();  // Show stats before returning to menu
            gameState = MENU;
            return;
        }
    }

    // Handle game moves
    if (currentPlayerTurn == PLAYER_X_TURN)
    {
        HandlePlayerTurn();
    }
    else if (currentPlayerTurn == PLAYER_O_TURN)
    {
        if (isTwoPlayer)
        {
            HandlePlayerTurn();
        }
        else
        {
            AITurn();
        }
    }
}

void AITurn()
{
    int bestScore = -1000;
    int bestRow = -1;
    int bestCol = -1;

    // Easy mode: 40% chance of random move
    if (currentDifficulty == EASY) {
        if (GetRandomValue(0, 100) < 40) {
            do {
                bestRow = GetRandomValue(0, 2);
                bestCol = GetRandomValue(0, 2);
            } while (grid[bestRow][bestCol] != EMPTY);
            grid[bestRow][bestCol] = PLAYER_O;
            
            // Update game state and return
            if (CheckWin(PLAYER_O)) {
                gameOver = true;
                winner = PLAYER_O;
                gameState = GAME_OVER;
                switch(currentDifficulty) {
                    case EASY: easyStats.wins++; easyStats.totalGames++; break;
                    case MEDIUM: mediumStats.wins++; mediumStats.totalGames++; break;
                    case HARD: hardStats.wins++; hardStats.totalGames++; break;
                }
            } else if (CheckDraw()) {
                gameOver = true;
                gameState = GAME_OVER;
                switch(currentDifficulty) {
                    case EASY: easyStats.draws++; easyStats.totalGames++; break;
                    case MEDIUM: mediumStats.draws++; mediumStats.totalGames++; break;
                    case HARD: hardStats.draws++; hardStats.totalGames++; break;
                }
            } else {
                currentPlayerTurn = PLAYER_X_TURN;
            }
            return;
        }
    }

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == EMPTY) {
                grid[i][j] = PLAYER_O;
                int score = Minimax(grid, false, 0);
                grid[i][j] = EMPTY;

                if (score > bestScore) {
                    bestScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }

    grid[bestRow][bestCol] = PLAYER_O;

    if (CheckWin(PLAYER_O)) {
        gameOver = true;
        winner = PLAYER_O;
        gameState = GAME_OVER;
        switch(currentDifficulty) {
            case EASY: easyStats.wins++; easyStats.totalGames++; break;
            case MEDIUM: mediumStats.wins++; mediumStats.totalGames++; break;
            case HARD: hardStats.wins++; hardStats.totalGames++; break;
        }
    } 
    else if (CheckDraw()) {
        gameOver = true;
        gameState = GAME_OVER;
        switch(currentDifficulty) {
            case EASY: easyStats.draws++; easyStats.totalGames++; break;
            case MEDIUM: mediumStats.draws++; mediumStats.totalGames++; break;
            case HARD: hardStats.draws++; hardStats.totalGames++; break;
        }
    } 
    else {
        currentPlayerTurn = PLAYER_X_TURN;
    }
}

bool CheckWin(Cell player)
{
    // check rows and columns
    for (int i = 0; i < GRID_SIZE; i++)
    {
        if (grid[i][0] == player && grid[i][1] == player && grid[i][2] == player) return true; // Row check
        if (grid[0][i] == player && grid[1][i] == player && grid[2][i] == player) return true; // Column check
    }

    // Check diagonals
    if (grid[0][0] == player && grid[1][1] == player && grid[2][2] == player) return true; // Main diagonal
    if (grid[0][2] == player && grid[1][1] == player && grid[2][0] == player) return true; // Anti diagonal

    return false; // No win found
}

bool CheckDraw()
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == EMPTY) return false; // If there's an empty cell, it's not a draw
        }
    }
    return true; // All cells are filled
}

void DrawGame()
{
    Vector2 mousePos = GetMousePosition();
    
    // Quit button hover (keep this part if you want the quit button to work)
    bool isQuitHovered = (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
                         mousePos.y >= 10 && mousePos.y <= 40);
    
    // Only set cursor for quit button if we're not in game over state
    if (!gameOver && isQuitHovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else if (!gameOver) {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    // the grid and pieces
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            Rectangle cell = {(float)(j * CELL_SIZE), (float)(i * CELL_SIZE), (float)CELL_SIZE, (float)CELL_SIZE};
            DrawRectangleRec(cell, LIGHTGRAY);

            if (grid[i][j] == PLAYER_X)
            {
                const char* text = "X";
                float fontSize = 100;
                float textWidth = MeasureText(text, fontSize);
                float textHeight = fontSize * 0.75f; // Approximate height of the text
                float textX = cell.x + (CELL_SIZE - textWidth) / 2;
                float textY = cell.y + (CELL_SIZE - textHeight) / 2;
                DrawText(text, textX, textY, fontSize, BLUE);
            }
            else if (grid[i][j] == PLAYER_O)
            {
                const char* text = "O";
                float fontSize = 100;
                float textWidth = MeasureText(text, fontSize);
                float textHeight = fontSize * 0.75f; // Approximate height of the text
                float textX = cell.x + (CELL_SIZE - textWidth) / 2;
                float textY = cell.y + (CELL_SIZE - textHeight) / 2;
                DrawText(text, textX, textY, fontSize, RED);
            }
        }
    }

    // grid lines
    for (int i = 1; i < GRID_SIZE; i++)
    {
        DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT, BLACK);
        DrawLine(0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE, BLACK);
    }

    // quit button
    Rectangle quitBtn = {
        SCREEN_WIDTH - 80, 10,  // position
        70, 30                  // size
    };
    
    // Use the existing mousePos variable instead of declaring a new one
    // Update isQuitHovered using CheckCollisionPointRec
    isQuitHovered = CheckCollisionPointRec(mousePos, quitBtn);
    
    DrawButton(quitBtn, "Quit", 20, isQuitHovered);

    // turn indicator
    if (!gameOver) {
        const char* turnText;
        if (currentPlayerTurn == PLAYER_X_TURN) {
            turnText = "Player X's Turn";
            DrawText(turnText, SCREEN_WIDTH/2 - MeasureText(turnText, 30)/2, 20, 30, BLUE);
        } 
        else {
            if (isTwoPlayer) {
                turnText = "Player O's Turn";
            } else {
                turnText = "AI's Turn";
            }
            DrawText(turnText, SCREEN_WIDTH/2 - MeasureText(turnText, 30)/2, 20, 30, RED);
        }
    }
}

void DrawMenu() {
    const int titleFontSize = 40;
    const int buttonFontSize = 20;
    
    // Title
    const char* title = "Tic-Tac-Toe";
    DrawText(title, 
        SCREEN_WIDTH/2 - MeasureText(title, titleFontSize)/2,
        SCREEN_HEIGHT/3,
        titleFontSize,
        BLACK);
    
    // Button rectangles
    Rectangle singlePlayerBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Rectangle twoPlayerBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Rectangle analysisBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

    Vector2 mousePos = GetMousePosition();
    
    // Check hover states
    bool singlePlayerHover = CheckCollisionPointRec(mousePos, singlePlayerBtn);
    bool twoPlayerHover = CheckCollisionPointRec(mousePos, twoPlayerBtn);
    bool analysisHover = CheckCollisionPointRec(mousePos, analysisBtn);

    // Draw buttons with hover effects
    DrawButton(singlePlayerBtn, "Single Player", buttonFontSize, singlePlayerHover);
    DrawButton(twoPlayerBtn, "Two Players", buttonFontSize, twoPlayerHover);
    DrawButton(analysisBtn, "View AI Analysis", buttonFontSize, analysisHover);

    // Set cursor based on any button hover
    SetMouseCursor((singlePlayerHover || twoPlayerHover || analysisHover) ? 
        MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void DrawGameOver() {
    const int titleFontSize = 40;
    const int buttonFontSize = 20;
    
    // Draw semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 100});
    
    // Result Text
    const char* resultText;
    Color resultColor;
    
    if (winner == PLAYER_X) {
        resultText = isTwoPlayer ? "Player X Wins!" : "You win!";
        resultColor = BLUE;
    } else if (winner == PLAYER_O) {
        resultText = isTwoPlayer ? "Player O Wins!" : "U so noob!";
        resultColor = RED;
    } else {
        resultText = "Draw!";
        resultColor = DARKGRAY;
    }
    
    // Draw result text with background
    int textWidth = MeasureText(resultText, titleFontSize);
    DrawRectangle(
        SCREEN_WIDTH/2 - textWidth/2 - 10,
        SCREEN_HEIGHT/3 - 10,
        textWidth + 20,
        titleFontSize + 20,
        WHITE
    );
    DrawText(resultText, 
        SCREEN_WIDTH/2 - textWidth/2,
        SCREEN_HEIGHT/3,
        titleFontSize,
        resultColor
    );
    
    // Back to Menu Button
    Rectangle menuBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + 40,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Vector2 mousePos = GetMousePosition();
    bool isHovering = CheckCollisionPointRec(mousePos, menuBtn);
    
    // Draw button with hover effect
    DrawButton(menuBtn, "Back to Menu", buttonFontSize, isHovering);
    
    // Set cursor
    SetMouseCursor(isHovering ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void InitGame()
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j] = EMPTY;
        }
    }
    gameOver = false;
    winner = EMPTY;
    currentPlayerTurn = PLAYER_X_TURN;
}

// Minimax algorithm
int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth)
{
    int score = EvaluateBoard(board);
    if (score == 10) return score - depth; // O (AI) is the maximizing player
    if (score == -10) return score + depth; // X (human) is the minimizing player
    if (CheckDraw()) return 0; // Draw

    if (isMaximizing)
    {
        int bestScore = -1000;
        for (int i = 0; i < GRID_SIZE; i++)
        {
            for (int j = 0; j < GRID_SIZE; j++)
            {
                if (board[i][j] == EMPTY)
                {
                    board[i][j] = PLAYER_O;
                    bestScore = fmax(bestScore, Minimax(board, false, depth + 1));
                    board[i][j] = EMPTY;
                }
            }
        }
        return bestScore;
    }
    else
    {
        int bestScore = 1000;
        for (int i = 0; i < GRID_SIZE; i++)
        {
            for (int j = 0; j < GRID_SIZE; j++)
            {
                if (board[i][j] == EMPTY)
                {
                    board[i][j] = PLAYER_X;
                    bestScore = fmin(bestScore, Minimax(board, true, depth + 1));
                    board[i][j] = EMPTY;
                }
            }
        }
        return bestScore;
    }
}

int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE])
{
    // Check rows and columns for a win
    for (int row = 0; row < GRID_SIZE; row++)
    {
        if (board[row][0] == board[row][1] && board[row][0] == board[row][2])
        {
            if (board[row][0] == PLAYER_O) return 10;
            else if (board[row][0] == PLAYER_X) return -10;
        }
    }
    for (int col = 0; col < GRID_SIZE; col++)
    {
        if (board[0][col] == board[1][col] && board[0][col] == board[2][col])
        {
            if (board[0][col] == PLAYER_O) return 10;
            else if (board[0][col] == PLAYER_X) return -10;
        }
    }

    // Check diagonals for a win
    if (board[0][0] == board[1][1] && board[0][0] == board[2][2])
    {
        if (board[0][0] == PLAYER_O) return 10;
        else if (board[0][0] == PLAYER_X) return -10;
    }
    if (board[0][2] == board[1][1] && board[0][2] == board[2][0])
    {
        if (board[0][2] == PLAYER_O) return 10;
        else if (board[0][2] == PLAYER_X) return -10;
    }

    return 0; // No winner
}

void DrawAIAnalysis() {
    const int titleFontSize = 35;
    const int textFontSize = 20;
    const int padding = 20;
    
    // Handle scrolling with mouse wheel
    scrollY += GetMouseWheelMove() * scrollSpeed;
    
    // Calculate total content height
    float totalContentHeight = 900;  // Total height of all content
    float visibleHeight = SCREEN_HEIGHT - (BUTTON_HEIGHT + padding);  // Visible area height
    
    // Limit scrolling
    float maxScroll = totalContentHeight - visibleHeight;
    if (scrollY > 0) scrollY = 0;
    if (scrollY < -maxScroll && maxScroll > 0) scrollY = -maxScroll;
    
    // Apply scroll offset to starting Y position
    int currentY = 40 + scrollY;
    
    BeginScissorMode(0, 0, SCREEN_WIDTH - 15, SCREEN_HEIGHT - (BUTTON_HEIGHT + padding));  // Leave space for scrollbar
    
    // Title
    const char* title = "AI Performance Analysis";
    DrawText(title, 
        SCREEN_WIDTH/2 - MeasureText(title, titleFontSize)/2,
        currentY,
        titleFontSize,
        BLACK);
    currentY += titleFontSize + padding;

    // Draw Dataset Statistics
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Total Games in Dataset: %d", easyStats.totalGames + mediumStats.totalGames + hardStats.totalGames);
    DrawText(buffer, padding, currentY, textFontSize, BLACK);
    currentY += textFontSize + padding;

    // Draw Training and Testing Results
    DrawText("Model Performance:", padding, currentY, textFontSize + 4, DARKBLUE);
    currentY += textFontSize + padding/2;

    snprintf(buffer, sizeof(buffer), "Training Accuracy: %.2f%%", trainingAccuracy);
    DrawText(buffer, padding * 2, currentY, textFontSize, BLACK);
    currentY += textFontSize + padding/2;

    snprintf(buffer, sizeof(buffer), "Testing Accuracy: %.2f%%", testingAccuracy);
    DrawText(buffer, padding * 2, currentY, textFontSize, BLACK);
    currentY += textFontSize + padding;

    // Draw Confusion Matrix
    DrawText("Confusion Matrix:", padding, currentY, textFontSize + 4, DARKBLUE);
    currentY += textFontSize + padding/2;

    snprintf(buffer, sizeof(buffer), "True Positives: %d", confusionMatrix.tp);
    DrawText(buffer, padding * 2, currentY, textFontSize, BLACK);
    currentY += textFontSize + padding/2;

    snprintf(buffer, sizeof(buffer), "True Negatives: %d", confusionMatrix.tn);
    DrawText(buffer, padding * 2, currentY, textFontSize, BLACK);
    currentY += textFontSize + padding/2;

    snprintf(buffer, sizeof(buffer), "False Positives: %d", confusionMatrix.fp);
    DrawText(buffer, padding * 2, currentY, textFontSize, BLACK);
    currentY += textFontSize + padding/2;

    snprintf(buffer, sizeof(buffer), "False Negatives: %d", confusionMatrix.fn);
    DrawText(buffer, padding * 2, currentY, textFontSize, BLACK);
    currentY += textFontSize + padding;

    // Draw Difficulty Statistics
    DrawText("Performance by Difficulty:", padding, currentY, textFontSize + 4, DARKBLUE);
    currentY += textFontSize + padding;

    DrawDifficultySection("Easy Mode", easyStats, &currentY, GREEN, padding, textFontSize);
    DrawDifficultySection("Medium Mode", mediumStats, &currentY, ORANGE, padding, textFontSize);
    DrawDifficultySection("Hard Mode", hardStats, &currentY, RED, padding, textFontSize);

    EndScissorMode();

    // Draw scrollbar
    if (maxScroll > 0) {
        float scrollbarHeight = (visibleHeight / totalContentHeight) * visibleHeight;
        float scrollbarY = (-scrollY / maxScroll) * (visibleHeight - scrollbarHeight);
        
        // Draw scrollbar background
        DrawRectangle(SCREEN_WIDTH - 15, 0, 15, visibleHeight, LIGHTGRAY);
        
        // Draw scrollbar handle
        DrawRectangle(
            SCREEN_WIDTH - 13,
            scrollbarY,
            11,
            scrollbarHeight,
            GRAY
        );
    }

    // Back button (outside scissor mode to always be visible)
    Rectangle backBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT - BUTTON_HEIGHT - padding/2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

    Vector2 mousePos = GetMousePosition();
    bool isHovering = CheckCollisionPointRec(mousePos, backBtn);

    DrawButton(backBtn, "Back to Menu", textFontSize, isHovering);

    SetMouseCursor(isHovering ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void DrawDifficultySection(const char* difficulty, DifficultyStats stats, int* y, Color color, int padding, int textFontSize) {
    char buffer[100];
    
    // Draw difficulty title
    DrawText(difficulty, padding, *y, textFontSize + 4, color);
    *y += textFontSize + padding/2;

    // Draw stats
    snprintf(buffer, sizeof(buffer), "Wins: %d", stats.wins);
    DrawText(buffer, padding * 2, *y, textFontSize, BLACK);
    *y += textFontSize + padding/3;

    snprintf(buffer, sizeof(buffer), "Losses: %d", stats.losses);
    DrawText(buffer, padding * 2, *y, textFontSize, BLACK);
    *y += textFontSize + padding/3;

    snprintf(buffer, sizeof(buffer), "Draws: %d", stats.draws);
    DrawText(buffer, padding * 2, *y, textFontSize, BLACK);
    *y += textFontSize + padding/3;

    float winRate = stats.totalGames > 0 ? 
        (float)stats.wins/stats.totalGames * 100 : 0;
    snprintf(buffer, sizeof(buffer), "Win Rate: %.1f%%", winRate);
    DrawText(buffer, padding * 2, *y, textFontSize, BLACK);
    *y += textFontSize + padding;
}

// Add the function definition
void DrawButton(Rectangle bounds, const char* text, int fontSize, bool isHovered) {
    DrawRectangleRec(bounds, isHovered ? GRAY : LIGHTGRAY);
    DrawRectangleLinesEx(bounds, 2, BLACK);
    DrawText(text,
        bounds.x + (bounds.width - MeasureText(text, fontSize))/2,
        bounds.y + (bounds.height - fontSize)/2,
        fontSize,
        BLACK);
}

// gcc -o lessSmartAI-Copy2 "lessSmartAI copy 2.c" -IC:\\msys64\\mingw64\\include -LC:\\msys64\\mingw64\\lib -lraylib -lopengl32 -lgdi32 -lwinmm
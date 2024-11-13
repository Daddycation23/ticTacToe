#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "naivebayes.h" 

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 40
#define GRID_SIZE 3
#define CELL_SIZE (SCREEN_WIDTH / GRID_SIZE)
#define FEATURES 9 // Number of features (board positions)

typedef enum { EMPTY, PLAYER_X, PLAYER_O } Cell;
typedef enum { PLAYER_X_TURN, PLAYER_O_TURN } PlayerTurn;
typedef enum { MENU, DIFFICULTY_SELECT, GAME, GAME_OVER, EXIT } GameState;
typedef enum { EASY, MEDIUM, HARD } Difficulty;

typedef struct {
    int wins;
    int losses;
    int draws;
    int totalGames;
} DifficultyStats;

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
NaiveBayesModel model;

void InitGame();
void UpdateGame();
void UpdateGameOver();
void HandlePlayerTurn();
void AITurn();
void AITurnNaiveBayes();
void LoadNaiveBayesModel();
void DrawGame();
void DrawDifficultySelect(void);
bool CheckWin(Cell player);
bool CheckDraw();
void DrawMenu();
void DrawGameOver();

int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit);
int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE]);

void DrawDifficultySection(const char* difficulty, DifficultyStats stats, int* y, Color color, int padding, int textFontSize);
void DrawButton(Rectangle bounds, const char* text, int fontSize, bool isHovered);

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic-Tac-Toe");

    // Load the image as a texture
    Texture2D background = LoadTexture("background.png");

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
                // Exit button, exit game when clicked
                else if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
                    mousePos.y >= SCREEN_HEIGHT/2 + 120 && mousePos.y <= SCREEN_HEIGHT/2 + 160) {
                    gameState = EXIT; // Change to Exit state
                }
            }
        }
        else if (gameState == DIFFICULTY_SELECT)  // Difficulty selection menu
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (mousePos.x >= SCREEN_WIDTH/2 - BUTTON_WIDTH/2 && 
                    mousePos.x <= SCREEN_WIDTH/2 + BUTTON_WIDTH/2) {
                    // Easy button
                    if (mousePos.y >= SCREEN_HEIGHT/2 && 
                        mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT) {
                        currentDifficulty = EASY;
                        gameState = GAME;
                        InitGame();
                    }
                    // Medium button
                    else if (mousePos.y >= SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20 && 
                            mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT * 2 + 20) {
                        currentDifficulty = MEDIUM;
                        gameState = GAME;
                        InitGame();
                    }
                    // Hard button
                    else if (mousePos.y >= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 && 
                            mousePos.y <= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 + BUTTON_HEIGHT) {
                        currentDifficulty = HARD;
                        gameState = GAME;
                        InitGame();
                    }
                }
            }
        }
        else if (gameState == GAME)
        {
            UpdateGame();
        }
        else if (gameState == GAME_OVER)
        {
            UpdateGameOver();
        }
        else if (gameState == EXIT)
        {
            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch(gameState) {
            case MENU:
                DrawTextureEx(background, (Vector2){0, 0}, 0.0f, (float)SCREEN_WIDTH / background.width, WHITE);
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
        }

        EndDrawing();
    }
    UnloadTexture(background); // Unload the texture when done
    CloseWindow();
    return 0;
}

void load_model(NaiveBayesModel *model, FILE *file) {
    // Read class probabilities
    fscanf(file, "Class Probabilities:\n");
    fscanf(file, "P(Positive): %lf\n", &model->class_probs[POSITIVE]);
    fscanf(file, "P(Negative): %lf\n", &model->class_probs[NEGATIVE]);

    // Read position probabilities
    for (int i = 0; i < NUM_POSITIONS; i++) {
        fscanf(file, "Position %d:\n", &i);
        fscanf(file, "P(x | Positive): %lf\n", &model->x_probs[i][POSITIVE]);
        fscanf(file, "P(x | Negative): %lf\n", &model->x_probs[i][NEGATIVE]);
        fscanf(file, "P(o | Positive): %lf\n", &model->o_probs[i][POSITIVE]);
        fscanf(file, "P(o | Negative): %lf\n", &model->o_probs[i][NEGATIVE]);
        fscanf(file, "P(b | Positive): %lf\n", &model->b_probs[i][POSITIVE]);
        fscanf(file, "P(b | Negative): %lf\n", &model->b_probs[i][NEGATIVE]);
    }
}

void LoadNaiveBayesModel()
{
    FILE *file = fopen("naivebayes_ml/model_weights.txt", "r");  // Adjust path if necessary
    if (file) {
        load_model(&model, file);  // Assuming `load_model` is the function in naivebayes.h to load the model
        fclose(file);
        printf("Model loaded successfully.\n");
    } else {
        printf("Failed to load model from 'model_weights.txt'. Make sure the file is available.\n");
    }
}

void UpdateGameOver() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        
        // Back to Menu Button
        Rectangle menuBtn = {
            SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
            SCREEN_HEIGHT/2 + 40,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        
        // Retry Button
        Rectangle retryBtn = {
            SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
            SCREEN_HEIGHT/2 + 100,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        
        if (CheckCollisionPointRec(mousePos, menuBtn)) {
            gameState = MENU;
            InitGame();  // Reset the game state
        } else if (CheckCollisionPointRec(mousePos, retryBtn)) {
            gameState = GAME;
            InitGame();  // Reset the game state for a new game
        }
    }
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
int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit)
{
    if (depth >= depthLimit) return 0; // Return 0 if depth limit is reached

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
                    bestScore = fmax(bestScore, Minimax(board, false, depth + 1, depthLimit));
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
                    bestScore = fmin(bestScore, Minimax(board, true, depth + 1, depthLimit));
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
                printf("Turn switched to: %d\n", currentPlayerTurn); // Debugging statement
            }
        }
    }
}

void UpdateGame()
{
    if (gameOver) return;
    printf("Current Player Turn: %d\n", currentPlayerTurn);

    // quit button click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
            mousePos.y >= 10 && mousePos.y <= 40)
        {
            gameState = MENU;
            return;
        }
    }

    // Check if it is now the AI's turn and game is not over
    if (currentPlayerTurn == PLAYER_X_TURN && !gameOver)
    {
        printf("Handling PLAYER_X's turn.\n");
        HandlePlayerTurn();

        // Check if Player X's move led to a game over (win or draw)
        if (gameOver) return;

        // If Player X has played a valid move, switch to AI's turn
        if (currentPlayerTurn == PLAYER_O_TURN)
        {
            printf("Switching to AI's turn.\n");
        }
    }

    // Handle AI's turn only if it's AI's turn and the game is not over
    if (currentPlayerTurn == PLAYER_O_TURN && !gameOver)
    {
        if (isTwoPlayer)
        {
            // In two-player mode, handle Player O's move
            HandlePlayerTurn();
        }
        else
        {
            // In single-player mode, AI makes a move based on difficulty level
            if (currentDifficulty == HARD)
            {
                printf("Using Minimax for Hard Mode\n");
                AITurn();  // Use Minimax for hard difficulty
            }
            else
            {
                printf("Using Naive Bayes for %s Mode\n", 
                    (currentDifficulty == EASY ) ? "Easy" : "Medium");
                AITurnNaiveBayes();  // Use Naive Bayes for easy and medium difficulties
            }

            // Print board state after AI move for debugging
            printf("Board state after AI move:\n");
            for (int i = 0; i < GRID_SIZE; i++) {
                for (int j = 0; j < GRID_SIZE; j++) {
                    printf("%d ", grid[i][j]);
                }
                printf("\n");
            }

            // Check if AI's move led to a game over (win or draw)
            if (gameOver) return;

            // After AI's move, switch back to Player X's turn if the game is not over
            currentPlayerTurn = PLAYER_X_TURN;
            printf("Switching turn back to PLAYER_X.\n");
        }
    }
}
void AITurnNaiveBayes()
{
    printf("Naive Bayes AI Turn: Difficulty Level - %s\n", 
           (currentDifficulty == EASY) ? "Easy" : "Medium"); 

    char board_str[FEATURES + 1];
    for (int i = 0, k = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++, k++) {
            board_str[k] = (grid[i][j] == PLAYER_X) ? 'x' :
                           (grid[i][j] == PLAYER_O) ? 'o' : 'b';
        }
    }
    board_str[FEATURES] = '\0';

    int move = predict_move(&model, board_str);
    printf("Naive Bayes predicted move: %d\n", move); 
    if (move != -1) {
        int row = move / GRID_SIZE;
        int col = move % GRID_SIZE;
        
        printf("AI selected move at row %d, col %d\n", row, col);

        if (grid[row][col] == EMPTY) {
            grid[row][col] = PLAYER_O; // AI makes its move
            printf("AI move applied on board at (%d, %d).\n", row, col);

            if (CheckWin(PLAYER_O)) {
                gameOver = true;
                winner = PLAYER_O;
                gameState = GAME_OVER;
                printf("AI wins the game.\n");
            } else if (CheckDraw()) {
                gameOver = true;
                gameState = GAME_OVER;
                printf("Game ended in a draw.\n");
            } else {
                currentPlayerTurn = PLAYER_X_TURN;
            }
        } else {
            printf("Error: AI chose an invalid move at (%d, %d).\n", row, col);
        }
    } else {
        printf("AI couldn't find a valid move.\n");
    }
}

void AITurn()
{
    int bestScore = -1000;
    int bestRow = -1;
    int bestCol = -1;

    // Hard mode: full Minimax search
    if (currentDifficulty == HARD) {
        int depthLimit = 9; // Full depth for hard mode
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] == EMPTY) {
                    grid[i][j] = PLAYER_O;
                    int score = Minimax(grid, false, 0, depthLimit);
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

    /* Commented out Easy and Medium Mode - Now handled by AITurnNaiveBayes
    // Easy mode: use Minimax with limited depth search of 2
    else if (currentDifficulty == EASY) {
        int depthLimit = 1; // Set a depth limit for easy difficulty
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] == EMPTY) {
                    grid[i][j] = PLAYER_O;
                    int score = Minimax(grid, false, 0, depthLimit);
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
    
    // Medium mode: use Minimax with limited depth search of 4
    else if (currentDifficulty == MEDIUM) {
        int depthLimit = 4; // Set a depth limit for medium difficulty
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] == EMPTY) {
                    grid[i][j] = PLAYER_O;
                    int score = Minimax(grid, false, 0, depthLimit);
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
    */

    // Ensure a move is made
    if (bestRow != -1 && bestCol != -1) {
        grid[bestRow][bestCol] = PLAYER_O;
    }

    if (CheckWin(PLAYER_O)) {
        gameOver = true;
        winner = PLAYER_O;
        gameState = GAME_OVER;
        hardStats.wins++;
        hardStats.totalGames++;
    } 
    else if (CheckDraw()) {
        gameOver = true;
        gameState = GAME_OVER;
        hardStats.draws++;
        hardStats.totalGames++;
    } 
    else {
        currentPlayerTurn = PLAYER_X_TURN;
    }
}
    /*
    // Ensure a move is made
    if (bestRow != -1 && bestCol != -1) {
        grid[bestRow][bestCol] = PLAYER_O;
    }

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
}*/

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

    Rectangle exitBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

    Vector2 mousePos = GetMousePosition();
    
    // Check hover states
    bool singlePlayerHover = CheckCollisionPointRec(mousePos, singlePlayerBtn);
    bool twoPlayerHover = CheckCollisionPointRec(mousePos, twoPlayerBtn);
    bool exitHover = CheckCollisionPointRec(mousePos, exitBtn);

    // Draw buttons with hover effects
    DrawButton(singlePlayerBtn, "Single Player", buttonFontSize, singlePlayerHover);
    DrawButton(twoPlayerBtn, "Two Players", buttonFontSize, twoPlayerHover);
    DrawButton(exitBtn, "Exit Game", buttonFontSize, exitHover);

    // Set cursor based on any button hover
    SetMouseCursor((singlePlayerHover || twoPlayerHover || exitHover) ? 
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
    
    // Retry Button
    Rectangle retryBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + 100, // Position below the menu button
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Vector2 mousePos = GetMousePosition();
    bool isHoveringMenu = CheckCollisionPointRec(mousePos, menuBtn);
    bool isHoveringRetry = CheckCollisionPointRec(mousePos, retryBtn);
    
    // Draw buttons with hover effect
    DrawButton(menuBtn, "Back to Menu", buttonFontSize, isHoveringMenu);
    DrawButton(retryBtn, "Retry", buttonFontSize, isHoveringRetry);
    
    // Set cursor
    SetMouseCursor((isHoveringMenu || isHoveringRetry) ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
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

// gcc -o main main.c -I. -L. -lraylib -lopengl32 -lgdi32 -lwinmm
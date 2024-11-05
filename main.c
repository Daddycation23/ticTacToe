#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define GRID_SIZE 3
#define CELL_SIZE (SCREEN_WIDTH / GRID_SIZE)
#define MAX_INSTANCES 958 // Total number of instances in the dataset
#define FEATURES 9 // Number of features (board positions)
#define CLASSES 2 // Number of classes (positive and negative)

typedef enum { EMPTY, PLAYER_X, PLAYER_O } Cell;
typedef enum { PLAYER_X_TURN, PLAYER_O_TURN } PlayerTurn;
typedef enum { MENU, GAME, GAME_OVER } GameState;

Cell grid[GRID_SIZE][GRID_SIZE];
PlayerTurn currentPlayerTurn = PLAYER_X_TURN;
bool gameOver = false;
Cell winner = EMPTY;
GameState gameState = MENU;
bool isTwoPlayer = false; // Flag to check if it's a two-player or single-player game

typedef struct {
    char board[FEATURES];
    int class; // 0 for negative, 1 for positive
} Instance;

Instance dataset[MAX_INSTANCES];
int datasetSize = 0;
double priorProbability[CLASSES];
double conditionalProbability[CLASSES][FEATURES][3]; // 3 possible values: 'x', 'o', 'b'

void InitGame();
void UpdateGame();
void HandlePlayerTurn();
void AITurn();
void DrawGame();
bool CheckWin(Cell player);
bool CheckDraw();
void DrawMenu();
void DrawGameOver();
void ResetGame();
int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing);
int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE]);
void loadDataset();
void trainNaiveBayes();
int predictNaiveBayes(Cell board[GRID_SIZE][GRID_SIZE]);
double calculateProbability(Cell board[GRID_SIZE][GRID_SIZE], int class);

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic-Tac-Toe");

    loadDataset();
    trainNaiveBayes();

    while (!WindowShouldClose())
    {
        if (gameState == MENU)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mousePos = GetMousePosition();
                if (mousePos.x > SCREEN_WIDTH / 2.0 - 100 && mousePos.x < SCREEN_WIDTH / 2.0 + 100)
                {
                    if (mousePos.y > SCREEN_HEIGHT / 2.0 - 60 && mousePos.y < SCREEN_HEIGHT / 2.0 - 20)
                    {
                        isTwoPlayer = false; // 1 Player mode
                        gameState = GAME;
                        InitGame();
                    }
                    else if (mousePos.y > SCREEN_HEIGHT / 2.0 + 20 && mousePos.y < SCREEN_HEIGHT / 2.0 + 60)
                    {
                        isTwoPlayer = true; // 2 Player mode
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
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mousePos = GetMousePosition();
                if (mousePos.x > SCREEN_WIDTH / 2.0 - 100 && mousePos.x < SCREEN_WIDTH / 2.0 + 100 &&
                    mousePos.y > SCREEN_HEIGHT / 2.0 + 40 && mousePos.y < SCREEN_HEIGHT / 2.0 + 80)
                {
                    gameState = MENU;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (gameState == MENU)
        {
            DrawMenu();
        }
        else if (gameState == GAME)
        {
            DrawGame();
        }
        else if (gameState == GAME_OVER)
        {
            DrawGame();
            DrawGameOver();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void UpdateGame()
{
    if (gameOver) return;

    if (currentPlayerTurn == PLAYER_X_TURN)
    {
        HandlePlayerTurn(); // Human's turn
    }
    else if (currentPlayerTurn == PLAYER_O_TURN)
    {
        if (isTwoPlayer)
        {
            HandlePlayerTurn(); // Player 2's turn in 2 Player mode
        }
        else
        {
            AITurn(); // AI's turn in 1 Player mode
        }
    }
}

void HandlePlayerTurn()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        int row = (int)(mousePos.y / CELL_SIZE);
        int col = (int)(mousePos.x / CELL_SIZE);

        if (grid[row][col] == EMPTY)
        {
            if (currentPlayerTurn == PLAYER_X_TURN)
            {
                grid[row][col] = PLAYER_X;
                if (CheckWin(PLAYER_X))
                {
                    gameOver = true;
                    winner = PLAYER_X;
                    gameState = GAME_OVER;
                }
                else if (CheckDraw())
                {
                    gameOver = true;
                    gameState = GAME_OVER;
                }
                else
                {
                    currentPlayerTurn = PLAYER_O_TURN;
                }
            }
            else if (currentPlayerTurn == PLAYER_O_TURN)
            {
                grid[row][col] = PLAYER_O;
                if (CheckWin(PLAYER_O))
                {
                    gameOver = true;
                    winner = PLAYER_O;
                    gameState = GAME_OVER;
                }
                else if (CheckDraw())
                {
                    gameOver = true;
                    gameState = GAME_OVER;
                }
                else
                {
                    currentPlayerTurn = PLAYER_X_TURN;
                }
            }
        }
    }
}

void AITurn()
{
    int bestMove[2] = {-1, -1};
    double bestProb = -1.0;

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == EMPTY)
            {
                Cell tempBoard[GRID_SIZE][GRID_SIZE];
                memcpy(tempBoard, grid, sizeof(grid));
                tempBoard[i][j] = PLAYER_O;
                
                int prediction = predictNaiveBayes(tempBoard);
                double prob = calculateProbability(tempBoard, prediction);
                
                if (prob > bestProb)
                {
                    bestProb = prob;
                    bestMove[0] = i;
                    bestMove[1] = j;
                }
            }
        }
    }

    if (bestMove[0] != -1 && bestMove[1] != -1)
    {
        grid[bestMove[0]][bestMove[1]] = PLAYER_O;
    }

    if (CheckWin(PLAYER_O))
    {
        gameOver = true;
        winner = PLAYER_O;
        gameState = GAME_OVER;
    }
    else if (CheckDraw())
    {
        gameOver = true;
        gameState = GAME_OVER;
    }
    else
    {
        currentPlayerTurn = PLAYER_X_TURN;
    }
}

int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing)
{
    if (CheckWin(PLAYER_O)) return 1;
    if (CheckWin(PLAYER_X)) return -1;
    if (CheckDraw()) return 0;

    // Reduce the depth of recursion to make the AI less intelligent
    static int depth = 0;
    if (depth >= 2) return 0;

    depth++;
    int bestScore = isMaximizing ? -1000 : 1000;

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (board[i][j] == EMPTY)
            {
                board[i][j] = isMaximizing ? PLAYER_O : PLAYER_X;
                int score = Minimax(board, !isMaximizing);
                board[i][j] = EMPTY;
                bestScore = isMaximizing ? (score > bestScore ? score : bestScore) : (score < bestScore ? score : bestScore);
            }
        }
    }

    depth--;
    return bestScore;
}

void InitGame()
{
    srand(time(NULL)); // Seed the random number generator

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j] = EMPTY;
        }
    }

    currentPlayerTurn = PLAYER_X_TURN;
    gameOver = false;
    winner = EMPTY;
}

void DrawGame()
{
    for (int i = 0; i <= GRID_SIZE; i++)
    {
        DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT, BLACK);
        DrawLine(0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE, BLACK);
    }

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == PLAYER_X)
            {
                DrawText("X", j * CELL_SIZE + CELL_SIZE / 2 - MeasureText("X", 100) / 2, i * CELL_SIZE + CELL_SIZE / 2 - 50, 100, RED);
            }
            else if (grid[i][j] == PLAYER_O)
            {
                DrawText("O", j * CELL_SIZE + CELL_SIZE / 2 - MeasureText("O", 100) / 2, i * CELL_SIZE + CELL_SIZE / 2 - 50, 100, BLUE);
            }
        }
    }

    // Display the current player's turn message
    if (!gameOver)
    {
        const char *message = (currentPlayerTurn == PLAYER_X_TURN) ? "Player X's turn" : "Player O's turn";
        DrawText(message, SCREEN_WIDTH / 2 - MeasureText(message, 20) / 2, 10, 20, BLACK);
    }
}

void DrawMenu()
{
    DrawText("Tic-Tac-Toe", SCREEN_WIDTH / 2 - MeasureText("Tic-Tac-Toe", 40) / 2, SCREEN_HEIGHT / 2 - 100, 40, BLACK);
    DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 60, 200, 40, LIGHTGRAY);
    DrawText("1 Player", SCREEN_WIDTH / 2 - MeasureText("1 Player", 20) / 2, SCREEN_HEIGHT / 2 - 50, 20, BLACK);
    DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 20, 200, 40, LIGHTGRAY);
    DrawText("2 Players", SCREEN_WIDTH / 2 - MeasureText("2 Players", 20) / 2, SCREEN_HEIGHT / 2 + 30, 20, BLACK);
}

void DrawGameOver()
{
    const char *message;
    if (winner == PLAYER_X)
    {
        message = "Player X Wins!";
    }
    else if (winner == PLAYER_O)
    {
        message = "Player O Wins!";
    }
    else
    {
        message = "It's a Draw!";
    }

    DrawText(message, SCREEN_WIDTH / 2 - MeasureText(message, 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, BLACK);
    DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 40, 200, 40, LIGHTGRAY);
    DrawText("Main Menu", SCREEN_WIDTH / 2 - MeasureText("Main Menu", 20) / 2, SCREEN_HEIGHT / 2 + 50, 20, BLACK);
}

bool CheckWin(Cell player)
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        if (grid[i][0] == player && grid[i][1] == player && grid[i][2] == player) return true;
        if (grid[0][i] == player && grid[1][i] == player && grid[2][i] == player) return true;
    }

    if (grid[0][0] == player && grid[1][1] == player && grid[2][2] == player) return true;
    if (grid[0][2] == player && grid[1][1] == player && grid[2][0] == player) return true;

    return false;
}

bool CheckDraw()
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == EMPTY) return false;
        }
    }
    return true;
}

void loadDataset()
{
    FILE *file = fopen("tic-tac-toe.data", "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        char *token = strtok(line, ",");
        int i = 0;
        while (token != NULL && i < FEATURES)
        {
            dataset[datasetSize].board[i] = token[0];
            token = strtok(NULL, ",");
            i++;
        }
        if (token != NULL)
        {
            dataset[datasetSize].class = (strcmp(token, "positive\n") == 0) ? 1 : 0;
        }
        datasetSize++;
    }

    fclose(file);
}

void trainNaiveBayes()
{
    int classCounts[CLASSES] = {0};
    int featureCounts[CLASSES][FEATURES][3] = {0};

    for (int i = 0; i < datasetSize; i++)
    {
        int class = dataset[i].class;
        classCounts[class]++;

        for (int j = 0; j < FEATURES; j++)
        {
            char value = dataset[i].board[j];
            int valueIndex = (value == 'x') ? 0 : (value == 'o') ? 1 : 2;
            featureCounts[class][j][valueIndex]++;
        }
    }

    for (int i = 0; i < CLASSES; i++)
    {
        priorProbability[i] = (double)classCounts[i] / datasetSize;

        for (int j = 0; j < FEATURES; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                conditionalProbability[i][j][k] = ((double)featureCounts[i][j][k] + 1) / (classCounts[i] + 3);
            }
        }
    }
}

int predictNaiveBayes(Cell board[GRID_SIZE][GRID_SIZE])
{
    double maxProb = -1.0;
    int bestClass = -1;

    for (int i = 0; i < CLASSES; i++)
    {
        double prob = calculateProbability(board, i);
        if (prob > maxProb)
        {
            maxProb = prob;
            bestClass = i;
        }
    }

    return bestClass;
}

double calculateProbability(Cell board[GRID_SIZE][GRID_SIZE], int class)
{
    double logProb = log(priorProbability[class]);

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            int feature = i * GRID_SIZE + j;
            int valueIndex;
            if (board[i][j] == PLAYER_X)
                valueIndex = 0;
            else if (board[i][j] == PLAYER_O)
                valueIndex = 1;
            else
                valueIndex = 2;

            logProb += log(conditionalProbability[class][feature][valueIndex]);
        }
    }

    return exp(logProb);
}

//gcc -o main main.c -I. -L. -lraylib -lopengl32 -lgdi32 -lwinmm//

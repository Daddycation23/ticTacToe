<<<<<<< HEAD
#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define GRID_SIZE 3
#define CELL_SIZE (SCREEN_WIDTH / GRID_SIZE)

typedef enum { EMPTY, PLAYER_X, PLAYER_O } Cell;
typedef enum { PLAYER_X_TURN, PLAYER_O_TURN } PlayerTurn;
typedef enum { MENU, GAME, GAME_OVER } GameState;

Cell grid[GRID_SIZE][GRID_SIZE];
PlayerTurn currentPlayerTurn = PLAYER_X_TURN;
bool gameOver = false;
Cell winner = EMPTY;
GameState gameState = MENU;
bool isTwoPlayer = false; // Flag to check if it's a two-player or single-player game

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

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic-Tac-Toe");

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
    int bestScore = -1000;
    int bestRow = -1;
    int bestCol = -1;

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == EMPTY)
            {
                grid[i][j] = PLAYER_O;
                int score = Minimax(grid, false);
                grid[i][j] = EMPTY;

                if (score > bestScore)
                {
                    bestScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }

    grid[bestRow][bestCol] = PLAYER_O;

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
                    int score = Minimax(board, false);
                    board[i][j] = EMPTY;
                    bestScore = (score > bestScore) ? score : bestScore;
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
                    int score = Minimax(board, true);
                    board[i][j] = EMPTY;
                    bestScore = (score < bestScore) ? score : bestScore;
                }
            }
        }
        return bestScore;
    }
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

//gcc -o main main.c -IC:\msys64\mingw64\include -LC:\msys64\mingw64\lib -lraylib -lopengl32 -lgdi32 -lwinmm//
=======
#include "C:\\raylib\\raylib\\src\\raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define GRID_SIZE 3
#define CELL_SIZE (SCREEN_WIDTH / GRID_SIZE)

typedef enum { EMPTY, PLAYER_X, PLAYER_O } Cell;
typedef enum { PLAYER_X_TURN, PLAYER_O_TURN } PlayerTurn;
typedef enum { MENU, GAME, GAME_OVER } GameState;

Cell grid[GRID_SIZE][GRID_SIZE];
PlayerTurn currentPlayerTurn = PLAYER_X_TURN;
bool gameOver = false;
Cell winner = EMPTY;
GameState gameState = MENU;
bool isTwoPlayer = false; // Flag to check if it's a two-player or single-player game

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

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic-Tac-Toe");

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
    int bestScore = -1000;
    int bestRow = -1;
    int bestCol = -1;

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == EMPTY)
            {
                grid[i][j] = PLAYER_O;
                int score = Minimax(grid, false);
                grid[i][j] = EMPTY;

                if (score > bestScore)
                {
                    bestScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }

    grid[bestRow][bestCol] = PLAYER_O;

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
                    int score = Minimax(board, false);
                    board[i][j] = EMPTY;
                    bestScore = (score > bestScore) ? score : bestScore;
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
                    int score = Minimax(board, true);
                    board[i][j] = EMPTY;
                    bestScore = (score < bestScore) ? score : bestScore;
                }
            }
        }
        return bestScore;
    }
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

//gcc -o main main.c -IC:\raylib\w64devkit\x86_64-w64-mingw32\include -LC:\raylib\w64devkit\x86_64-w64-mingw32\lib -lraylib -lopengl32 -lgdi32 -lwinmm//
>>>>>>> b51ba30d8474433a8f558a07bf84034b8ab0308d

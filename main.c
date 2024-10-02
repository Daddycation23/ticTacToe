#include "C:\raylib\raylib\src\raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define GRID_SIZE 3
#define CELL_SIZE (SCREEN_WIDTH / GRID_SIZE)

typedef enum { EMPTY, PLAYER_X, PLAYER_O } Cell;
typedef enum { PLAYER, AI } PlayerType;
typedef enum { MENU, GAME, GAME_OVER } GameState;

Cell grid[GRID_SIZE][GRID_SIZE];
PlayerType currentPlayer = PLAYER;
bool gameOver = false;
Cell winner = EMPTY;
GameState gameState = MENU;
bool isTwoPlayer = false;
Cell startingPlayer = EMPTY;

void InitGame();
void UpdateGame();
void DrawGame();
bool CheckWin(Cell player);
bool CheckDraw();
void AITurn();
void DrawMenu();
void DrawGameOver();
void ResetGame();

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
                        isTwoPlayer = false;
                        gameState = GAME;
                        InitGame();
                    }
                    else if (mousePos.y > SCREEN_HEIGHT / 2.0 + 20 && mousePos.y < SCREEN_HEIGHT / 2.0 + 60)
                    {
                        isTwoPlayer = true;
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

    // Randomly choose the starting player
    startingPlayer = (rand() % 2 == 0) ? PLAYER_X : PLAYER_O;
    currentPlayer = PLAYER;
    gameOver = false;
    winner = EMPTY;
}

void UpdateGame()
{
    if (gameOver) return;

    if (currentPlayer == PLAYER)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePos = GetMousePosition();
            int row = (int)(mousePos.y / CELL_SIZE);
            int col = (int)(mousePos.x / CELL_SIZE);

            if (grid[row][col] == EMPTY)
            {
                grid[row][col] = (startingPlayer == PLAYER_X) ? PLAYER_X : PLAYER_O;
                if (CheckWin((startingPlayer == PLAYER_X) ? PLAYER_X : PLAYER_O))
                {
                    gameOver = true;
                    winner = (startingPlayer == PLAYER_X) ? PLAYER_X : PLAYER_O;
                    gameState = GAME_OVER;
                }
                else if (CheckDraw())
                {
                    gameOver = true;
                    gameState = GAME_OVER;
                }
                else
                {
                    currentPlayer = isTwoPlayer ? PLAYER : AI;
                    startingPlayer = (startingPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
                }
            }
        }
    }
    else if (currentPlayer == AI)
    {
        AITurn();
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
            currentPlayer = PLAYER;
        }
    }
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
    if (isTwoPlayer && !gameOver)
    {
        const char *message = (startingPlayer == PLAYER_X) ? "Player X's turn" : "Player O's turn";
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
    const char *message = winner == PLAYER_X ? "Player X Wins!" : (winner == PLAYER_O ? "Player O Wins!" : "It's a Draw!");
    DrawText(message, SCREEN_WIDTH / 2 - MeasureText(message, 40) / 2, SCREEN_HEIGHT / 2 - 20, 40, BLACK);
    DrawRectangle(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 40, 200, 40, LIGHTGRAY);
    DrawText("Play again?", SCREEN_WIDTH / 2 - MeasureText("Play again?", 20) / 2, SCREEN_HEIGHT / 2 + 50, 20, BLACK);
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

void AITurn()
{
    int emptyCells[GRID_SIZE * GRID_SIZE][2];
    int emptyCount = 0;

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == EMPTY)
            {
                emptyCells[emptyCount][0] = i;
                emptyCells[emptyCount][1] = j;
                emptyCount++;
            }
        }
    }

    if (emptyCount > 0)
    {
        int randomIndex = rand() % emptyCount;
        int row = emptyCells[randomIndex][0];
        int col = emptyCells[randomIndex][1];
        grid[row][col] = PLAYER_O;
    }
}

//gcc -o main main.c -IC:\raylib\w64devkit\x86_64-w64-mingw32\include -LC:\raylib\w64devkit\x86_64-w64-mingw32\lib -lraylib -lopengl32 -lgdi32 -lwinmm//

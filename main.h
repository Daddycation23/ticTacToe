#ifndef MAIN_H
#define MAIN_H

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
#define TITLE_GRID_SIZE 3
#define MAX_SYMBOLS 100
#define SYMBOL_SPEED 0.02f  // Constant speed for falling symbols
#define ROTATION_SPEED 0.02f  // Speed of rotation

typedef enum { EMPTY, PLAYER_X, PLAYER_O } Cell;
typedef enum { PLAYER_X_TURN, PLAYER_O_TURN } PlayerTurn;
typedef enum { MENU, DIFFICULTY_SELECT, GAME, GAME_OVER } GameState;
typedef enum { EASY, MEDIUM, HARD } Difficulty;

typedef struct {
    char symbol;  // 'X', 'O' or ' '
    float alpha;  // For fade effect
    bool active;
} GridSymbol;

typedef struct {
    Vector2 position;
    char symbol;
    float rotation;  // Rotation angle
} FallingSymbol;

extern GridSymbol titleSymbols[TITLE_GRID_SIZE][TITLE_GRID_SIZE];
extern FallingSymbol symbols[MAX_SYMBOLS];

extern Difficulty currentDifficulty;
extern PlayerTurn currentPlayerTurn;
extern bool gameOver;
extern Cell winner;
extern GameState gameState;
extern bool isTwoPlayer;
extern float titleCellScales[TITLE_GRID_SIZE][TITLE_GRID_SIZE];
extern float titleRotations[TITLE_GRID_SIZE][TITLE_GRID_SIZE];
extern float titleAnimSpeed;
extern float buttonVibrationOffset;
extern float vibrationSpeed;
extern float vibrationAmount;

// Declare scroll variables
static float scrollY = 0.0f;
static const float scrollSpeed = 20.0f;

void InitGame();
void InitSymbols();
void UpdateSymbols();
void DrawSymbols();
void UpdateGame(Sound buttonClickSound, Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound);
void UpdateGameOver(Sound buttonClickSound);
bool HandlePlayerTurn(Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound);
void AITurn(Sound victorySound, Sound loseSound, Sound drawSound);
void DrawGame();
void DrawDifficultySelect(void);
bool CheckWin(Cell player);
bool CheckDraw();
void DrawMenu();
void DrawGameOver();

int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit);
int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE]);

void DrawDifficultySection(const char* difficulty, int* y, Color color, int padding, int textFontSize);
void DrawButton(Rectangle bounds, const char* text, int fontSize, bool isHovered);

Font customFont;

#endif // MAIN_H
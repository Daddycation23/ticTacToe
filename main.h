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
#include "naivebayes.h" 

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
#define JUMP_SPEED 0.02f  // Constant jump speed
#define JUMP_DELAY 0.7f  // Constant delay between jumps

typedef enum { EMPTY, PLAYER_X, PLAYER_O } Cell;
typedef enum { PLAYER_X_TURN, PLAYER_O_TURN } PlayerTurn;
typedef enum { MENU, DIFFICULTY_SELECT, MODEL_SELECT, GAME, GAME_OVER } GameState;
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

typedef struct {
    Vector2 position;
    Vector2 targetPosition;
    const char* word;
    bool isJumping;
    float jumpSpeed;
} TitleWord;

typedef enum {
    NAIVE_BAYES,
    DECISION_TREE,
} AIModel;

typedef struct {
    int playerWins;
    int aiWins; 
    int draws;
    int totalGames;
} ModeStats;

ModeStats mediumStats = {0, 0, 0, 0};
ModeStats hardStats = {0, 0, 0, 0};
ModeStats naiveBayesStats = {0, 0, 0, 0};
ModeStats decisionTreeStats = {0, 0, 0, 0};

Sound buttonClickSound;
Sound popSound;
Sound victorySound;
Sound loseSound;
Sound drawSound; 
Sound mainMenuSound;
Sound playSound;

extern GridSymbol titleSymbols[TITLE_GRID_SIZE][TITLE_GRID_SIZE];
extern FallingSymbol symbols[MAX_SYMBOLS];
extern TitleWord titleWords[5];  // "Tic", "-", "Tac", "-", "Toe"

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
extern AIModel currentModel;
extern NaiveBayesModel model;

extern int playerWins;
extern int aiWins;
extern int draws;
extern int totalGames;

void DrawSymbols();
void DrawTitleWords();
void DrawGame();
void DrawDifficultySelect(void);
void DrawModelSelect(void);
void DrawButton(Rectangle bounds, const char* text, int fontSize, bool isHovered);
void DrawMenu();
void DrawGameOver();

void InitGame();
void InitSymbols();
void InitTitleWords();
void UpdateSymbols();
void UpdateTitleWords();
void UpdateGame();
void UpdateGameOver();
bool HandlePlayerTurn();
bool CheckWin(Cell player);
bool CheckDraw();
int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit);
int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE]);

void AITurn();
void AITurnNaiveBayes();
void LoadNaiveBayesModel();
void AITurnDecisionTree();


#endif // MAIN_H
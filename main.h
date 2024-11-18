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
#define JUMP_SPEED 0.02f  // Constant jump speed
#define JUMP_DELAY 0.7f  // Constant delay between jumps
#define MAX_CONFETTI 200    // Maximum number of confetti particles

#define NUM_POSITIONS 9
#define NUM_OUTCOMES 2 // positive or negative
#define POSITIVE 0
#define NEGATIVE 1
#define RATIO 0.8

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

ModeStats* GetCurrentModeStats(void);

// Define model type such that it holds the probabilites of number of attributes for both outcomes respectively
typedef struct {
    double x_probs[NUM_POSITIONS][NUM_OUTCOMES];
    double o_probs[NUM_POSITIONS][NUM_OUTCOMES];
    double b_probs[NUM_POSITIONS][NUM_OUTCOMES];
    double class_probs[NUM_OUTCOMES];
} NaiveBayesModel;

// Define GetHint object to store best move row and col, and the counts of hints
struct GetHint
{
    int row;
    int col;
    int hintCountX;
    int hintCountO;
};

typedef struct {
    Vector2 position;   // Position of the particle
    Vector2 velocity;   // Velocity of the particle (speed)
    Color color;    // Color of the particle
    float size;     // Size of the particle
    bool active;    // Add active flag
    float alpha;    // Add alpha for fade effect
    float lifetime; // Add lifetime for particle duration
} Confetti;

// Declare variables as extern
extern ModeStats mediumStats;
extern ModeStats hardStats;
extern ModeStats naiveBayesStats;
extern ModeStats decisionTreeStats;

extern Sound buttonClickSound;
extern Sound popSound;
extern Sound victorySound;
extern Sound loseSound;
extern Sound drawSound;
extern Sound mainMenuSound;
extern Sound playSound;

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

extern int playerWins;
extern int aiWins;
extern int draws;
extern int totalGames;

extern Confetti confetti[MAX_CONFETTI];
extern bool showPartyAnimation;

// Function prototypes
void InitConfetti();
void UpdateConfetti();
void DrawConfetti();
void DrawSymbols();
void DrawTitleWords();
void DrawGame();
void DrawDifficultySelect(void);
void DrawModelSelect(void);
void DrawButton(Rectangle bounds, const char* text, int fontSize, bool isHovered);
void DrawMenu();
void DrawGameOver();
bool HandleButtonHover(Rectangle button, const char* text, int fontSize, bool* isHovered);

void InitGame();
void InitSymbols();
void InitTitleWords();
void UpdateSymbols();
void UpdateTitleWords();
void UpdateGame(Sound buttonClickSound, Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound, NaiveBayesModel *model);
void UpdateGameOver(Sound buttonClickSound);
bool HandlePlayerTurn(Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound);
void AITurn(Sound victorySound, Sound loseSound, Sound drawSound, NaiveBayesModel *model);
bool CheckWin(Cell player);
bool CheckDraw();
void getHint();
void clearHint();
int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit, int alpha, int beta);
int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE]);

// Naive Bayes function prototypes
void load_data(const char *filename, char boards[][NUM_POSITIONS + 1], int outcomes[], int *total_records);
void split_data(char boards[][NUM_POSITIONS + 1], int outcomes[], int total_records, char train_boards[][NUM_POSITIONS + 1], int train_outcomes[], char test_boards[][NUM_POSITIONS + 1], int test_outcomes[], int *train_size, int *test_size, float ratio);
void train_NBmodel(NaiveBayesModel *model, char boards[][NUM_POSITIONS + 1], int outcomes[], int size);
void save_NBmodel(const NaiveBayesModel *model, const char *filename);
void test_NBmodel(const char *filename, char mode[], char type[], NaiveBayesModel *model, char boards[][NUM_POSITIONS + 1], int outcomes[], int size);
double calculate_probability(NaiveBayesModel *model, const char board[], int outcome);
int predict_outcome(NaiveBayesModel *model, const char board[]);
int predict_move(NaiveBayesModel *model, Cell grid[GRID_SIZE][GRID_SIZE], int *bestRow, int *bestCol);

// Utility functions
int outcome_index(const char *outcome);
void divide(int dividend, int divisor, int *quo, int *rem);
void AITurnDecisionTree();

#endif // MAIN_H

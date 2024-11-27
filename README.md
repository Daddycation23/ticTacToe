# Tic-Tac-Toe Game

## Overview

This is a graphical Tic-Tac-Toe game implemented using the Raylib library. The game supports both single-player and two-player modes, with an AI opponent available in single-player mode. The game features a user-friendly interface with animations and sound effects to enhance the gaming experience.

## Features

- **Single Player Mode**: Play against an AI opponent with three difficulty levels: Easy, Medium, and Hard.
- **Two Player Mode**: Play against another human player on the same device.
- **Animated UI**: Includes animations for the title, buttons, and grid cells.
- **Sound Effects**: Includes sound effects for button clicks, game events, and background music.
- **Hover Effects**: Visual feedback when hovering over buttons and grid cells, with vibration effects for certain buttons.
- **Winning Animations**: Surprise players with confetti animations upon winning.
- **Hint System**: Helping players to perform the best move available.

## Game States

- **Menu**: The main menu where players can choose between single-player and two-player modes.
- **Difficulty Select**: Allows players to select the difficulty level for the AI opponent.
- **Model Select**: Allows players to select the AI model for the single-player Easy mode.
- **Game**: The main game screen where players take turns placing their symbols on the grid.
- **Game Over**: Displays the result of the game and provides options to retry or return to the menu.

## Controls

- **Mouse**: Use the mouse to navigate the menu, select options, and place symbols on the grid.
- **Buttons**: Click on buttons to start a game, select difficulty, or exit the game.

## Code Structure

- **main.c**: Contains the main game logic, including initialization, game loop, and rendering.
- **main.h**: Header file with declarations for global variables, constants, and function prototypes.

## Key Functions

### Game Logic Functions
- `void InitGame()`: Initializes the game state and grid.
- `void UpdateGame(Sound buttonClickSound, Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound, NaiveBayesModel *model)`: Updates the game state based on player input and game logic.
- `bool CheckWin(Cell player)`: Checks if a specified player has won the game.
- `bool CheckDraw()`: Checks if the game is a draw.
- `void AITurn(Sound victorySound, Sound loseSound, Sound drawSound, NaiveBayesModel *model)`: Handles the AI's turn in single-player mode.
- `void AITurnDecisionTree(Sound victorySound, Sound loseSound, Sound drawSound, DecisionTreeNode *TDmodel)`: Handles the AI's turn in single-player mode with integrated Decision Tree model.
- `bool HandlePlayerTurn(Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound)`: Manages the player's turn and checks for game outcomes.
- `void UpdateGameOver(Sound buttonClickSound)`: Manages the game over state, allowing players to retry or return to the menu.
- `void clearHint()`: Clear best move from previous turn.
- `void getHint()`: Get best move for current player.
- `ModeStats* GetCurrentModeStats(void)`: Tracks game statistics (wins, losses and draws) for each difficulty mode and AI model.

### UI and Animation Functions
- `void DrawButton(Rectangle bounds, const char* text, int fontSize, bool isHovered)`: Draws buttons with hover and vibration effects.
- `void InitSymbols()`: Initializes the falling symbols for the animated UI.
- `void UpdateSymbols()`: Updates the position and rotation of falling symbols.
- `void DrawSymbols()`: Draws the falling symbols on the screen.
- `void InitTitleWords()`: Initializes the title words for the animated title.
- `void UpdateTitleWords()`: Updates the animation state of the title words.
- `void DrawTitleWords()`: Draws the animated title words.
- `void InitConfetti()`: Initializes the confetti particles for the animated UI upon winning.
- `void UpdateConfetti()`: Updates the position and rotation of confetti particles.
- `void DrawConfetti()`: Draws the confetti particles on the screen.
- `void DrawGame()`: Renders the game grid, symbols, and UI elements.
- `void DrawMenu()`: Renders the menu with UI and animations.
- `void DrawGameOver()`: Renders the game over screen.
- `void DrawDifficultySelect()`: Renders the difficulty selection screen.
- `void DrawModelSelect()`: Renders the model selection screen when easy mode is chosen.
- `bool HandleButtonHover(Rectangle button, const char* text, int fontSize, bool* isHovered)`: Handles button hover effects.

### Data Processing Functions
- `void load_data(const char *filename, char boards[][NUM_POSITIONS + 1], int outcomes[], int *total_records)`: Loads data from file.
- `void split_data(boards, outcomes, total_records, train_boards, train_outcomes, test_boards, test_outcomes, &train_size, &test_size, RATIO)`: Shuffle and split dataset for training and testing of model.
- `int outcome_index(const char *outcome)`: Convert the string outcome ("positive" or "negative") into the corresponding numerical label (POSITIVE(0) or NEGATIVE(1)).

### AI Functions
- `int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit, int alpha, int beta)`: Implements the Minimax algorithm with Alpha-Beta Pruning and imperfectness for AI decision-making.
- `void AITurnDecisionTree()`: Implements the decision tree algorithm for AI decision-making.
- `int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE])`: Evaluates the board to determine the score for the AI.
- `void train_NBmodel(NaiveBayesModel *model, char boards[][NUM_POSITIONS + 1], int outcomes[], int size)`: Train model with Naive Bayes(NB) algorithm.
- `void save_NBmodel(const NaiveBayesModel *model, const char *filename)`: Save weights of NB model into a text file.
- `void test_NBmodel(const char *filename, char mode[], char type[], NaiveBayesModel *model, char boards[][NUM_POSITIONS + 1], int outcomes[], int size)`: Save prediction results of trained NB model into text file.
- `double calculate_probability(NaiveBayesModel *model, const char board[], int outcome)`: Calculate the posterior probability of a specified outcome based on the given board layout.
- `int predict_outcome(NaiveBayesModel *model, const char board[])`: Predict outcome of a given board layout.
- `int predict_move(NaiveBayesModel *model, Cell grid[GRID_SIZE][GRID_SIZE], int *bestRow, int *bestCol)`: Predict next best move based on given board layout.
- `void divide(int dividend, int divisor, int *quo, int *rem)`: Get quotient and remainder of a given integer.
- `NBmodel/plot_confusion_matrix.py`: Python file to plot the confusion matrix of the NB model on both training and testing dataset.
- `void growth_Tree(DecisionTreeNode *tree)`: Builds, trains, and evaluates a decision tree on a dataset, calculates accuracy and error rates, and writes results to files.
- `void load_dataset(const char *filename, DataRow dataset[], int *dataset_size)`: Loads the dataset from a file, parses it into features and labels, and stores it in an array.
- `void shuffle_dataset(DataRow dataset[], int size)`: Randomly shuffles the dataset to ensure random distribution of samples.
- `void decision_tree_split_dataset(DataRow dataset[], int dataset_size, DataRow train_set[], int *train_size, DataRow test_set[], int *test_size, float train_ratio)`: Splits the dataset into training and testing subsets based on a specified ratio.
- `DecisionTreeNode *build_tree(DataRow dataset[], int size, int depth)`: Constructs a decision tree by recursively splitting the dataset using the Gini index and applying depth or purity stopping conditions.
- `float evaluate_with_randomness(DecisionTreeNode *root, DataRow dataset[], int size, int confusion_matrix[2][2])`: Evaluates the decision tree's accuracy with randomized predictions and updates a confusion matrix.
- `int predict_with_randomness(DecisionTreeNode *node, int features[])`: Predicts a label using a decision tree with an optional randomness factor to flip predictions.
- `void display_confusion_matrix(int confusion_matrix[2][2], const char *filename, const char *dataset_type)`: Writes a confusion matrix and associated metrics to a file.
- `void write_accuracy_to_file(const char *filename, const char *dataset_type, float accuracy, int correct, int total)`: Writes the accuracy and classification results for training or testing datasets to a file.
- `void free_tree(DecisionTreeNode *node)`: Recursively frees memory allocated for the decision tree nodes.
- `float calculate_gini_index(DataRow dataset[], int size, int feature_index, int threshold)`: Calculates the Gini index to evaluate the quality of a potential split in the dataset.
- `void decision_tree_split_data(DataRow dataset[], int size, int feature_index, int threshold, DataRow left[], int *left_size, DataRow right[], int *right_size)`: Splits the dataset into left and right branches based on a feature index and threshold.
- `void dt_predict_best_move(DecisionTreeNode *tree, char board[3][3], char current_player, int *best_row, int *best_col)`: Predicts the best move for a player in a tic-tac-toe board using the decision tree model.
- `void print_tree(DecisionTreeNode *node, int depth)`: Recursively prints the structure of the decision tree, including nodes and leaf predictions.
- `void calculate_position_probabilities(DataRow dataset[], int dataset_size, const char *filename)`: Calculates and saves the probabilities of each symbol (`x`, `o`, `b`) at each position for positive and negative outcomes.
- `double calculate_error_rate(DecisionTreeNode *root, DataRow dataset[], int size, int confusion_matrix[2][2])`: Calculates the error rate of a decision tree by comparing its predictions to the actual labels.
- `DecisionTree_ML/confusionmatrix.py`: Python file to plot the confusion matrix of the Decision Tree model on both training and testing dataset.
- `DecisionTree_ML/decisiontree.h`: Header file that defines the structure, constants and function prototypes for building, training, evaluating, and utilizing a decision tree model including dataset handling prediction.

### Raylib Functions
- `void InitWindow(int width, int height, const char *title)`: Initialize window and OpenGL context.
- `void CloseWindow(void)`: Close window and unload OpenGL context.
- `bool WindowShouldClose(void)`: Check if application should close (KEY_ESCAPE pressed or windows close icon clicked).
- `void BeginDrawing(void)`: Setup canvas (framebuffer) to start drawing.
- `void EndDrawing(void)`: End canvas drawing and swap buffers (double buffering).
- `void DrawText(const char *text, int posX, int posY, int fontSize, Color color)`: Draw text (using default font).
- `void DrawTextPro(Font font, const char *text, Vector2 position, Vector2 origin, float rotation, float fontSize, float spacing, Color tint)`: Draw text using Font and pro parameters (rotation).
- `void DrawRectangle(int posX, int posY, int width, int height, Color color)`: Draw a color-filled rectangle.
- `void DrawRectangleRec(Rectangle rec, Color color)`: Draw a color-filled rectangle.
- `void DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color)`: Draw rectangle outline with extended parameters.
- `void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color)`: Draw a line.
- `Vector2 GetMousePosition(void)`: Get mouse position XY.
- `void SetMouseCursor(int cursor)`: Set mouse cursor.
- `bool IsMouseButtonPressed(int button)`: Check if a mouse button has been pressed once.
- `bool CheckCollisionPointRec(Vector2 point, Rectangle rec)`: Check if point is inside rectangle.
- `void PlaySound(Sound sound)`: Play a sound.
- `Sound LoadSound(const char *fileName)`: Loads sound from file. 
- `void UnloadSound(Sound sound)`: Unloads a sound file.
- `void SetSoundVolume(Sound sound, float volume)`: Set volume for a sound (1.0 is max level).
- `void StopSound(Sound sound)`: Stop playing a sound.
- `bool IsSoundPlaying(Sound sound)`: Check if a sound is currently playing.
- `void InitAudioDevice(void)`: Initialize audio device and context.
- `void CloseAudioDevice(void)`: Close the audio device and context.
- `Image LoadImage(const char *fileName)`: Load image from file into CPU memory (RAM).
- `void UnloadImage(Image image)`: Unload image from CPU memory (RAM).
- `void SetWindowIcon(Image image)`: Set icon for window (single image, RGBA 32bit)
- `void ClearBackground(Color color)`: Set background color (framebuffer clear color).
- `int GetRandomValue(int min, int max)`: Get a random value between min and max (both included).
- `float GetFrameTime(void)`: Get time in seconds for last frame drawn (delta time).
- `Font GetFontDefault(void)`: Get the default Font.
- `int MeasureText(const char *text, int fontSize)`: Measure string width for default font.
- `double GetTime(void)`: Get elapsed time in seconds since InitWindow().

### Standard Functions
- `int sprintf(char *str, const char *format, ...)`: Sends formatted output to a string.
- `int snprintf(char *__restrict__ __stream, size_t __n, const char *__restrict__ __format, ...)`: Writes formatted output to a string.
- `float sinf (float x)`: Computes the sine (specified in radians) of x.
- `void *memset(void *str, int c, size_t n)`: Returns a pointer to the memory area string.

## Installation

1. **Clone**: Just clone the repository, ensure the .a files are cloned. Clone the repository to your desired location using this command:
   ```bash
   git clone https://github.com/Daddycation23/ticTacToe
   ```
2. **Build**: Compile the program using a C compiler with Raylib linked.
   ```bash
   gcc -o main main.c DecisionTree_ML/*.c NBmodel/*.c GameFunctions/*.c -I. -I./DecisionTree_ML -I./NBmodel -I./GameFunctions -L./Libraries -L. -lraylib -lopengl32 -lgdi32 -lwinmm
   ```
3. **Run**: Execute the compiled program.
   ```bash
   ./main
   ```


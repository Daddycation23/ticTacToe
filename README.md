# Tic-Tac-Toe Game

## Overview

This is a graphical Tic-Tac-Toe game implemented using the Raylib library. The game supports both single-player and two-player modes, with an AI opponent available in single-player mode. The game features a user-friendly interface with animations and sound effects to enhance the gaming experience.

## Features

- **Single Player Mode**: Play against an AI opponent with three difficulty levels: Easy, Medium, and Hard.
- **Two Player Mode**: Play against another human player on the same device.
- **Animated UI**: Includes animations for the title, buttons, and grid cells.
- **Sound Effects**: Includes sound effects for button clicks, game events, and background music.
- **Hover Effects**: Visual feedback when hovering over buttons and grid cells, with vibration effects for certain buttons.

## Game States

- **Menu**: The main menu where players can choose between single-player and two-player modes.
- **Difficulty Select**: Allows players to select the difficulty level for the AI opponent.
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
- `void DrawGame()`: Renders the game grid, symbols, and UI elements.
- `void DrawMenu()`: Renders the menu with UI and animations.
- `void DrawGameOver()`: Renders the game over screen.
- `void DrawDifficultySelect()`: Renders the difficulty selection screen.
- `void DrawModelSelect()`: Renders the model selection screen when easy mode is chosen.
- `bool CheckWin(Cell player)`: Checks if a specified player has won the game.
- `bool CheckDraw()`: Checks if the game is a draw.
- `void AITurn(Sound victorySound, Sound loseSound, Sound drawSound, NaiveBayesModel *model)`: Handles the AI's turn in single-player mode.
- `bool HandlePlayerTurn(Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound)`: Manages the player's turn and checks for game outcomes.
- `void UpdateGameOver(Sound buttonClickSound)`: Manages the game over state, allowing players to retry or return to the menu.
- `void clearHint()`: Clear hint.
- `void getHint()`: Get hint for player

### UI and Animation Functions
- `void DrawButton(Rectangle bounds, const char* text, int fontSize, bool isHovered)`: Draws buttons with hover and vibration effects.
- `void InitSymbols()`: Initializes the falling symbols for the animated UI.
- `void UpdateSymbols()`: Updates the position and rotation of falling symbols.
- `void DrawSymbols()`: Draws the falling symbols on the screen.
- `void InitTitleWords()`: Initializes the title words for the animated title.
- `void UpdateTitleWords()`: Updates the animation state of the title words.
- `void DrawTitleWords()`: Draws the animated title words.

### AI Functions
- `int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit, int alpha, int beta)`: Implements the Minimax algorithm for AI decision-making.
- `void AITurnDecisionTree()`: Implements the decision tree algorithm for AI decision-making.
- `int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE])`: Evaluates the board to determine the score for the AI.

### Raylib Functions
- `void InitWindow(int width, int height, const char *title)`: Initialize window and OpenGL context.
- `void CloseWindow(void)`: Close window and unload OpenGL context.
- `void BeginDrawing(void)`: Setup canvas (framebuffer) to start drawing.
- `void EndDrawing(void)`: End canvas drawing and swap buffers (double buffering).
- `void DrawText(const char *text, int posX, int posY, int fontSize, Color color)`: Draw text (using default font).
- `void DrawRectangleRec(Rectangle rec, Color color)`: Draw a color-filled rectangle.
- `void DrawRectangleLinesEx(Rectangle rec, float lineThick, Color color)`: Draw rectangle outline with extended parameters.
- `Vector2 GetMousePosition(void)`: Get mouse position XY.
- `bool IsMouseButtonPressed(int button)`: Check if a mouse button has been pressed once.
- `bool CheckCollisionPointRec(Vector2 point, Rectangle rec)`: Check if point is inside rectangle.
- `void PlaySound(Sound sound)`: Play a sound.
- `Sound LoadSound(const char *fileName)`: Loads sound from file.
- `void UnloadSound(Sound sound)`: Unloads a sound file. 
- `void InitAudioDevice(void)`: Initialize audio device and context.
- `void CloseAudioDevice(void)`: Close the audio device and context.
- `Image LoadImage(const char *fileName)`: Load image from file into CPU memory (RAM).
- `void SetWindowIcon(Image image)`: Set icon for window (single image, RGBA 32bit)

## Installation

1. **Clone**: Just clone the repository, ensure the .a files are cloned. Clone the repository to your desired location using this command:
   ```bash
   git clone https://github.com/Daddycation23/ticTacToe
   ```
2. **Build**: Compile the program using a C compiler with Raylib linked.
   ```bash
   gcc -o main main.c NBmodel/data_processing.c NBmodel/NBmodel.c -I. -L. -lraylib -lopengl32 -lgdi32 -lwinmm
   ```
3. **Run**: Execute the compiled program.
   ```bash
   ./main
   ```


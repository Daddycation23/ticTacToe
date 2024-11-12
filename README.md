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
- `InitGame()`: Initializes the game state and grid.
- `UpdateGame()`: Updates the game state based on player input and game logic.
- `DrawGame()`: Renders the game grid, symbols, and UI elements.
- `CheckWin(Cell player)`: Checks if a specified player has won the game.
- `CheckDraw()`: Checks if the game is a draw.
- `AITurn(Sound victorySound, Sound loseSound, Sound drawSound)`: Handles the AI's turn in single-player mode.
- `HandlePlayerTurn(Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound)`: Manages the player's turn and checks for game outcomes.
- `UpdateGameOver(Sound buttonClickSound)`: Manages the game over state, allowing players to retry or return to the menu.

### UI and Animation Functions
- `DrawButton(Rectangle bounds, const char* text, int fontSize, bool isHovered)`: Draws buttons with optional hover and vibration effects.
- `InitSymbols()`: Initializes the falling symbols for the animated UI.
- `UpdateSymbols()`: Updates the position and rotation of falling symbols.
- `DrawSymbols()`: Draws the falling symbols on the screen.
- `InitTitleWords()`: Initializes the title words for the animated title.
- `UpdateTitleWords()`: Updates the animation state of the title words.
- `DrawTitleWords()`: Draws the animated title words.

### AI Functions
- `Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit)`: Implements the Minimax algorithm for AI decision-making.
- `EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE])`: Evaluates the board to determine the score for the AI.

### Raylib Functions
- `InitWindow()`: Initializes the game window.
- `CloseWindow()`: Closes the game window.
- `BeginDrawing()`: Begins the drawing process.
- `EndDrawing()`: Ends the drawing process.
- `DrawText()`: Draws text on the screen.
- `DrawRectangleRec()`: Draws a rectangle on the screen.
- `DrawRectangleLinesEx()`: Draws the outline of a rectangle.
- `GetMousePosition()`: Retrieves the current mouse position.
- `IsMouseButtonPressed()`: Checks if a mouse button is pressed.
- `CheckCollisionPointRec()`: Checks if a point is within a rectangle.
- `PlaySound()`: Plays a sound.
- `LoadSound()`: Loads a sound file.
- `UnloadSound()`: Unloads a sound file.
- `InitAudioDevice()`: Initializes the audio device.
- `CloseAudioDevice()`: Closes the audio device.

## Installation

1. **Clone**: Just clone the repository, ensure the .a files are cloned. Clone the repository to your desired location using this command:
   ```bash
   git clone https://github.com/Daddycation23/ticTacToe/tree/alt
   ```
2. **Build**: Compile the program using a C compiler with Raylib linked.
   ```bash
   gcc -o main main.c -I. -L. -lraylib -lopengl32 -lgdi32 -lwinmm
   ```
3. **Run**: Execute the compiled program.
   ```bash
   ./main
   ```

## Future Improvements

- Add network multiplayer support to play with friends online.
- Enhance AI with more advanced strategies and learning capabilities.
- Add more visual themes and customization options for the game board and pieces.
- Implement a scoring system to track wins, losses, and draws over multiple games.

## License

??

## Contributing

Contributions are welcome! If you have suggestions or improvements, feel free to fork the repository and submit a pull request.

## Acknowledgments

- Thanks to [Raylib](https://www.raylib.com/) for providing an easy-to-use library for game development.
- Inspired by classic Tic-Tac-Toe games and the desire to create a fun, interactive experience.

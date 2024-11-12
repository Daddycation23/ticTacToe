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

- `InitGame()`: Initializes the game state and grid.
- `UpdateGame()`: Updates the game state based on player input and game logic.
- `DrawGame()`: Renders the game grid, symbols, and UI elements.
- `CheckWin()`: Checks if a player has won the game.
- `CheckDraw()`: Checks if the game is a draw.
- `AITurn()`: Handles the AI's turn in single-player mode.
- `DrawButton()`: Draws buttons with optional hover and vibration effects.

## Installation

1. **Dependencies**: Ensure you have the Raylib library installed on your system. You can download it from [Raylib's official website](https://www.raylib.com/).
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

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! If you have suggestions or improvements, feel free to fork the repository and submit a pull request.

## Acknowledgments

- Thanks to [Raylib](https://www.raylib.com/) for providing an easy-to-use library for game development.
- Inspired by classic Tic-Tac-Toe games and the desire to create a fun, interactive experience.

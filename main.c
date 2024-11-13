#include "main.h"

// Define the global arrays
GridSymbol titleSymbols[TITLE_GRID_SIZE][TITLE_GRID_SIZE];
FallingSymbol symbols[MAX_SYMBOLS];
TitleWord titleWords[5];  // "Tic", "-", "Tac", "-", "Toe"

// Initialize the title words
void InitTitleWords() {
    const char* words[] = {"Tic", "-", "Tac", "-", "Toe"};
    int startX = SCREEN_WIDTH / 2 - MeasureText("Tic-Tac-Toe", 40) / 2;
    int startY = SCREEN_HEIGHT / 5 + TITLE_GRID_SIZE * 50 + 20;
    int spacing = 10;  // Space between words and hyphens

    for (int i = 0; i < 5; i++) {
        titleWords[i].word = words[i];
        titleWords[i].position = (Vector2){ startX, startY };
        titleWords[i].targetPosition = (Vector2){ startX, startY - 20 };
        titleWords[i].isJumping = false;
        titleWords[i].jumpSpeed = JUMP_SPEED;
        startX += MeasureText(words[i], 40) + spacing;
    }
}

// Update the title words
void UpdateTitleWords() {
    static int currentWord = 0;
    static float jumpDelay = 0.0f;

    jumpDelay += GetFrameTime();
    if (jumpDelay > JUMP_DELAY) {  // Delay between each word's jump
        if (!titleWords[currentWord].isJumping) {
            titleWords[currentWord].isJumping = true;
            jumpDelay = 0.0f;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (titleWords[i].isJumping) {
            titleWords[i].position.y -= titleWords[i].jumpSpeed;
            if (titleWords[i].position.y <= titleWords[i].targetPosition.y) {
                titleWords[i].jumpSpeed = -titleWords[i].jumpSpeed;  // Reverse direction
            }
            if (titleWords[i].position.y >= SCREEN_HEIGHT / 5 + TITLE_GRID_SIZE * 50 + 20) {
                titleWords[i].position.y = SCREEN_HEIGHT / 5 + TITLE_GRID_SIZE * 50 + 20;
                titleWords[i].isJumping = false;
                titleWords[i].jumpSpeed = JUMP_SPEED;
                currentWord = (currentWord + 1) % 5;  // Move to the next word
            }
        }
    }
}

// Draw the title words
void DrawTitleWords() {
    for (int i = 0; i < 5; i++) {
        DrawText(titleWords[i].word, titleWords[i].position.x, titleWords[i].position.y, 40, BLACK);
    }
}

// Initialize the symbols
void InitSymbols() {
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        symbols[i].position = (Vector2){ GetRandomValue(0, SCREEN_WIDTH), GetRandomValue(-SCREEN_HEIGHT, 0) };
        symbols[i].symbol = GetRandomValue(0, 1) ? 'X' : 'O';
        symbols[i].rotation = GetRandomValue(0, 360);  // Random initial rotation
    }
}

// Update the symbols
void UpdateSymbols() {
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        symbols[i].position.y += SYMBOL_SPEED;
        symbols[i].rotation += ROTATION_SPEED;  // Update rotation
        if (symbols[i].position.y > SCREEN_HEIGHT) {
            symbols[i].position.y = GetRandomValue(-SCREEN_HEIGHT, 0);
            symbols[i].position.x = GetRandomValue(0, SCREEN_WIDTH);
            symbols[i].symbol = GetRandomValue(0, 1) ? 'X' : 'O';
            symbols[i].rotation = GetRandomValue(0, 360);  // Reset rotation
        }
    }
}
// Draw the symbols
void DrawSymbols() {
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        Vector2 origin = {10, 10};  // Center of rotation
        DrawTextPro(GetFontDefault(), &symbols[i].symbol, symbols[i].position, origin, symbols[i].rotation, 20, 1, symbols[i].symbol == 'X' ? BLUE : RED);
    }
}

Difficulty currentDifficulty = MEDIUM; // Initialize difficulty to a value, doesn't have to be medium
Cell grid[GRID_SIZE][GRID_SIZE]; // Initialize the grid with empty cells
PlayerTurn currentPlayerTurn = PLAYER_X_TURN; // Initialize the current player turn to Player X
bool gameOver = false; // Initialize the game over flag to false
Cell winner = EMPTY; // Initialize the winner to empty
GameState gameState = MENU; // Initialize the game state to menu
bool isTwoPlayer = false; // Flag to check if it's a two-player or single-player game

float titleCellScales[TITLE_GRID_SIZE][TITLE_GRID_SIZE] = {0};
float titleRotations[TITLE_GRID_SIZE][TITLE_GRID_SIZE] = {0};
float titleAnimSpeed = 2.0f;
float buttonVibrationOffset = 0.0f;
float vibrationSpeed = 15.0f;
float vibrationAmount = 2.0f;

// Main function
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic-Tac-Toe");
    InitAudioDevice();  // Initialize audio device

    // Load the icon image
    Image icon = LoadImage("assets\\icon.png");  // Make sure the file path is correct
    SetWindowIcon(icon);  // Set the window icon
    UnloadImage(icon);  // Unload the image after setting the icon

    Sound buttonClickSound = LoadSound("assets\\ButtonClicked.mp3");  // Load the button click sound
    Sound popSound = LoadSound("assets\\Pop.mp3");  // Load the pop sound
    Sound victorySound = LoadSound("assets\\FFVictory.mp3");  // Load the victory sound
    Sound loseSound = LoadSound("assets\\MarioLose.mp3");  // Load the lose sound
    Sound drawSound = LoadSound("assets\\Draw.mp3");  // Load the draw sound
    Sound mainMenuSound = LoadSound("assets\\MainMenu.mp3");  // Load the main menu sound
    Sound playSound = LoadSound("assets\\Play.mp3");  // Load the play sound

    InitSymbols();  // Initialize the falling symbols
    InitTitleWords();  // Initialize the title words

    while (!WindowShouldClose())
    {
        if (gameState == MENU || gameState == DIFFICULTY_SELECT) {
            if (!IsSoundPlaying(mainMenuSound)) {
                PlaySound(mainMenuSound);  // Play main menu sound
            }
            StopSound(playSound);  // Ensure play sound is stopped
        } else if (gameState == GAME) {
            if (!IsSoundPlaying(playSound)) {
                PlaySound(playSound);  // Play play sound
            }
            StopSound(mainMenuSound);  // Ensure main menu sound is stopped
        } else {
            StopSound(mainMenuSound);  // Stop main menu sound when leaving these states
            StopSound(playSound);  // Stop play sound when leaving the game state
        }

        if (gameState == MENU || gameState == DIFFICULTY_SELECT) {
            UpdateSymbols();  // Update the falling symbols
            UpdateTitleWords();  // Update the title words
        }

        if (gameState == MENU) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                // Single Player button
                if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
                    mousePos.y >= SCREEN_HEIGHT/2 + 60 && mousePos.y <= SCREEN_HEIGHT/2 + 100) {
                    PlaySound(buttonClickSound);  // Play sound on button click
                    isTwoPlayer = false;
                    gameState = DIFFICULTY_SELECT;  // go to difficulty selection instead of game
                }
                // Two Player button
                else if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
                        mousePos.y >= SCREEN_HEIGHT/2 + 120 && mousePos.y <= SCREEN_HEIGHT/2 + 160) {
                    PlaySound(buttonClickSound);  // Play sound on button click
                    isTwoPlayer = true;
                    gameState = GAME;
                    InitGame();
                }
                // Exit button
                else if (mousePos.x >= SCREEN_WIDTH/2 - 100 && mousePos.x <= SCREEN_WIDTH/2 + 100 &&
                        mousePos.y >= SCREEN_HEIGHT/2 + 180 && mousePos.y <= SCREEN_HEIGHT/2 + 220) {
                    PlaySound(buttonClickSound);  // Play sound on button click
                    break;  // Exit the game loop
                }
            }
        }
        else if (gameState == GAME) 
        {
            UpdateGame(buttonClickSound, popSound, victorySound, loseSound, drawSound);
        }
        else if (gameState == GAME_OVER)
        {
            UpdateGameOver(buttonClickSound);
        }
        else if (gameState == DIFFICULTY_SELECT) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();

                // Back button
                if (mousePos.x >= 20 && mousePos.x <= SCREEN_WIDTH/6 && mousePos.y >= 10 && mousePos.y <= 40) {
                    PlaySound(buttonClickSound);  // Play sound on button click
                    gameState = MENU;
                }

                if (mousePos.x >= SCREEN_WIDTH/2 - BUTTON_WIDTH/2 && 
                    mousePos.x <= SCREEN_WIDTH/2 + BUTTON_WIDTH/2) {
                    // easy button
                    if (mousePos.y >= SCREEN_HEIGHT/2 && 
                        mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT) {
                        PlaySound(buttonClickSound);  // Play sound on button click
                        currentDifficulty = EASY;
                        gameState = GAME;
                        InitGame();
                    }
                    // medium button
                    else if (mousePos.y >= SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20 && 
                             mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT * 2 + 20) {
                        PlaySound(buttonClickSound);  // Play sound on button click
                        currentDifficulty = MEDIUM;
                        gameState = GAME;
                        InitGame();
                    }
                    // hard button
                    else if (mousePos.y >= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 && 
                             mousePos.y <= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 + BUTTON_HEIGHT) {
                        PlaySound(buttonClickSound);  // Play sound on button click
                        currentDifficulty = HARD;
                        gameState = GAME;
                        InitGame();
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch(gameState) {
            case MENU:
                DrawSymbols();  // Draw the falling symbols
                DrawTitleWords();  // Draw the jumping title words
                DrawMenu();  // Draw the menu
                break;
            case DIFFICULTY_SELECT:
                DrawSymbols();  // Draw the falling symbols
                DrawDifficultySelect();  // Draw the difficulty selection
                break;
            case GAME:
                DrawGame();  // Draw the game
                break;
            case GAME_OVER:
                DrawGame();  // Draw the game
                DrawGameOver();  // Draw the game over screen
                break;
        }

        EndDrawing();  // End drawing
    }

    UnloadSound(buttonClickSound);  // Unload the button click sound
    UnloadSound(popSound);  // Unload the pop sound
    UnloadSound(victorySound);  // Unload the victory sound
    UnloadSound(loseSound);  // Unload the lose sound
    UnloadSound(drawSound);  // Unload the draw sound
    UnloadSound(mainMenuSound);  // Unload the main menu sound
    UnloadSound(playSound);  // Unload the play sound
    CloseAudioDevice();  // Close the audio device
    CloseWindow();  // Close the window
    return 0;
}

// Update the game
void UpdateGame(Sound buttonClickSound, Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound)
{
    if (gameOver) return;

    // Quit button click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
            mousePos.y >= 10 && mousePos.y <= 40)
        {
            PlaySound(buttonClickSound);  // Play sound on button click
            gameState = MENU;
            return;
        }
    }

    // Handle game moves
    if (currentPlayerTurn == PLAYER_X_TURN)
    {
        if (HandlePlayerTurn(popSound, victorySound, loseSound, drawSound)) {
            PlaySound(popSound);  // Play sound when player moves
        }
    }
    else if (currentPlayerTurn == PLAYER_O_TURN)
    {
        if (isTwoPlayer)
        {
            if (HandlePlayerTurn(popSound, victorySound, loseSound, drawSound)) {
                PlaySound(popSound);  // Play sound when player moves
            }
        }
        else
        {
            AITurn(victorySound, loseSound, drawSound);
        }
    }
}

// Handle the player's turn
bool HandlePlayerTurn(Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        int row = (int)(mousePos.y / CELL_SIZE);
        int col = (int)(mousePos.x / CELL_SIZE);

        if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE)
        {
            if (grid[row][col] == EMPTY)
            {
                grid[row][col] = (currentPlayerTurn == PLAYER_X_TURN) ? PLAYER_X : PLAYER_O;
                if (CheckWin(grid[row][col]))
                {
                    gameOver = true;
                    winner = grid[row][col];
                    gameState = GAME_OVER;
                    
                    // Play sound immediately when a winner is detected
                    if (!isTwoPlayer) {
                        if (winner == PLAYER_X) {
                            PlaySound(victorySound);  // Play victory sound for Player X
                        } else if (winner == PLAYER_O) {
                            PlaySound(loseSound);  // Play lose sound for Player O
                        }
                    } else {
                        PlaySound(victorySound);  // Play victory sound for any winner in two-player mode
                    }
                }
                else if (CheckDraw())  // Check for a draw
                {
                    gameOver = true;
                    gameState = GAME_OVER;
                    winner = EMPTY;  // No winner in a draw
                    PlaySound(drawSound);  // Play draw sound
                }
                else
                {
                    currentPlayerTurn = (currentPlayerTurn == PLAYER_X_TURN) ? PLAYER_O_TURN : PLAYER_X_TURN;
                }
                return true;  // Move was made
            }
        }
    }
    return false;  // No move was made
}

// AI's turn
void AITurn(Sound victorySound, Sound loseSound, Sound drawSound)
{
    int bestScore = -1000;
    int bestRow = -1;
    int bestCol = -1;

    // Easy mode: use Minimax with limited depth search of 2
    if (currentDifficulty == EASY) {
        int depthLimit = 1; // Set a depth limit for easy difficulty
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] == EMPTY) {
                    grid[i][j] = PLAYER_O;
                    int score = Minimax(grid, false, 0, depthLimit);
                    grid[i][j] = EMPTY;

                    if (score > bestScore) {
                        bestScore = score;
                        bestRow = i;
                        bestCol = j;
                    }
                }
            }
        }
    }
    // Medium mode: use Minimax with limited depth search of 4
    else if (currentDifficulty == MEDIUM)
    {
        int depthLimit = 4; // Set a depth limit for medium difficulty
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] == EMPTY) {
                    grid[i][j] = PLAYER_O;
                    int score = Minimax(grid, false, 0, depthLimit);
                    grid[i][j] = EMPTY;

                    if (score > bestScore) {
                        bestScore = score;
                        bestRow = i;
                        bestCol = j;
                    }
                }
            }
        }
    }
    // Hard mode: full Minimax search
    else if (currentDifficulty == HARD)
    {
        int depthLimit = 9; // Full depth for hard mode
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] == EMPTY) {
                    grid[i][j] = PLAYER_O;
                    int score = Minimax(grid, false, 0, depthLimit);
                    grid[i][j] = EMPTY;

                    if (score > bestScore) {
                        bestScore = score;
                        bestRow = i;
                        bestCol = j;
                    }
                }
            }
        }
    }

    // Ensure a move is made
    if (bestRow != -1 && bestCol != -1) {
        grid[bestRow][bestCol] = PLAYER_O;
    }

    if (CheckWin(PLAYER_O)) {
        gameOver = true;
        winner = PLAYER_O;
        gameState = GAME_OVER;
        
        // Play sound immediately when a winner is detected
        if (!isTwoPlayer) {
            PlaySound(loseSound);  // Play lose sound for Player O
        } else {
            PlaySound(victorySound);  // Play victory sound for any winner in two-player mode
        }

    } 
    else if (CheckDraw()) {
        gameOver = true;
        gameState = GAME_OVER;
        PlaySound(drawSound);  // Play draw sound
    } 
    else {
        currentPlayerTurn = PLAYER_X_TURN;
    }
}

// Check if the player has won  
bool CheckWin(Cell player)
{
    // check rows and columns
    for (int i = 0; i < GRID_SIZE; i++)
    {
        if (grid[i][0] == player && grid[i][1] == player && grid[i][2] == player) return true; // Row check
        if (grid[0][i] == player && grid[1][i] == player && grid[2][i] == player) return true; // Column check
    }

    // Check diagonals
    if (grid[0][0] == player && grid[1][1] == player && grid[2][2] == player) return true; // Main diagonal
    if (grid[0][2] == player && grid[1][1] == player && grid[2][0] == player) return true; // Anti diagonal

    return false; // No win found
}

// Check if the game is a draw
bool CheckDraw()
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == EMPTY) return false; // If there's an empty cell, it's not a draw
        }
    }
    return true; // All cells are filled
}

// Draw the game
void DrawGame()
{
    Vector2 mousePos = GetMousePosition();
    
    // Quit button hover 
    bool isQuitHovered = (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
                         mousePos.y >= 10 && mousePos.y <= 40);
    
    // Only set cursor for quit button if we're not in game over state
    if (!gameOver && isQuitHovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else if (!gameOver) {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    // The grid and pieces
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            Rectangle cell = {(float)(j * CELL_SIZE), (float)(i * CELL_SIZE), (float)CELL_SIZE, (float)CELL_SIZE};
            
            // Check if the mouse is hovering over the cell and the cell is empty
            bool isHovered = !gameOver && grid[i][j] == EMPTY && CheckCollisionPointRec(mousePos, cell);
            
            // Draw the cell with a darker color if hovered
            DrawRectangleRec(cell, isHovered ? DARKGRAY : LIGHTGRAY);

            if (grid[i][j] == PLAYER_X)
            {
                const char* text = "X";
                float fontSize = 100;
                float textWidth = MeasureText(text, fontSize);
                float textHeight = fontSize * 0.75f; // Approximate height of the text
                float textX = cell.x + (CELL_SIZE - textWidth) / 2;
                float textY = cell.y + (CELL_SIZE - textHeight) / 2;
                DrawText(text, textX, textY, fontSize, BLUE);
            }
            else if (grid[i][j] == PLAYER_O)
            {
                const char* text = "O";
                float fontSize = 100;
                float textWidth = MeasureText(text, fontSize);
                float textHeight = fontSize * 0.75f; // Approximate height of the text
                float textX = cell.x + (CELL_SIZE - textWidth) / 2;
                float textY = cell.y + (CELL_SIZE - textHeight) / 2;
                DrawText(text, textX, textY, fontSize, RED);
            }
        }
    }

    // Grid lines
    for (int i = 1; i < GRID_SIZE; i++)
    {
        DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT, BLACK);
        DrawLine(0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE, BLACK);
    }

    // Quit button
    Rectangle quitBtn = {
        SCREEN_WIDTH - 80, 10,  // position
        70, 30                  // size
    };
    
    // Use the existing mousePos variable instead of declaring a new one
    // Update isQuitHovered using CheckCollisionPointRec
    isQuitHovered = CheckCollisionPointRec(mousePos, quitBtn);
    
    DrawButton(quitBtn, "Quit", 20, !gameOver && isQuitHovered);

    // Turn indicator
    if (!gameOver) {
        const char* turnText;
        if (currentPlayerTurn == PLAYER_X_TURN) {
            turnText = "Player X's Turn";
            DrawText(turnText, SCREEN_WIDTH/2 - MeasureText(turnText, 30)/2, 20, 30, BLUE);
        } 
        else {
            if (isTwoPlayer) {
                turnText = "Player O's Turn";
            } else {
                turnText = "AI's Turn";
            }
            DrawText(turnText, SCREEN_WIDTH/2 - MeasureText(turnText, 30)/2, 20, 30, RED);
        }
    }
}

// Draw the menu
void DrawMenu() {
    const int titleFontSize = 40;
    const int buttonFontSize = 20;
    
    // Title
    const char* title = "";
    const int cellSize = 50;  // larger cells for better visibility
    const int gridWidth = TITLE_GRID_SIZE * cellSize;
    const int gridHeight = TITLE_GRID_SIZE * cellSize;
    const int startX = SCREEN_WIDTH/2 - gridWidth/2;
    const int startY = SCREEN_HEIGHT/5;

    // Cell animations
    for(int i = 0; i < TITLE_GRID_SIZE; i++) {
        for(int j = 0; j < TITLE_GRID_SIZE; j++) {
            Rectangle cell = {
                startX + j * cellSize,
                startY + i * cellSize,
                cellSize,
                cellSize
            };
            
            // Draw just the grid lines
            DrawRectangleLinesEx(cell, 2, BLACK);

            // Handle the X and O symbols
            if (!titleSymbols[i][j].active && GetRandomValue(0, 100) < 2) {
                titleSymbols[i][j].symbol = GetRandomValue(0, 1) ? 'X' : 'O';
                titleSymbols[i][j].alpha = 0;
                titleSymbols[i][j].active = true;
            }

            if (titleSymbols[i][j].active) {
                titleSymbols[i][j].alpha += GetFrameTime() * 2;
                if (titleSymbols[i][j].alpha > 1.0f) {
                    titleSymbols[i][j].alpha = 0;
                    titleSymbols[i][j].active = false;
                }

                Color symbolColor = titleSymbols[i][j].symbol == 'X' ? BLUE : RED;
                symbolColor.a = (unsigned char)(titleSymbols[i][j].alpha * 255);
                
                Vector2 textPos = {
                    cell.x + (cellSize - MeasureText(&titleSymbols[i][j].symbol, 40))/2,
                    cell.y + (cellSize - 40)/2
                };
                DrawText(&titleSymbols[i][j].symbol, textPos.x, textPos.y, 40, symbolColor);
            }
        }
    }

    // Draw title text below the grid
    DrawText(title, 
        SCREEN_WIDTH/2 - MeasureText(title, titleFontSize)/2,
        startY + gridHeight + 20,
        titleFontSize,
        BLACK);
    
    // Button rectangles
    Rectangle singlePlayerBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Rectangle twoPlayerBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

    Rectangle exitBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 3,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Vector2 mousePos = GetMousePosition();

    // Check hover states
    bool singlePlayerHover = CheckCollisionPointRec(mousePos, singlePlayerBtn);
    bool twoPlayerHover = CheckCollisionPointRec(mousePos, twoPlayerBtn);
    bool exitHover = CheckCollisionPointRec(mousePos, exitBtn);

    // Draw buttons with hover effects
    DrawButton(singlePlayerBtn, "Single Player", buttonFontSize, singlePlayerHover);
    DrawButton(twoPlayerBtn, "Two Players", buttonFontSize, twoPlayerHover);
    DrawButton(exitBtn, "Exit", buttonFontSize, exitHover);

    // Set cursor based on any button hover
    SetMouseCursor((singlePlayerHover || twoPlayerHover || exitHover) ? 
        MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

void DrawGameOver() {
    const int titleFontSize = 40;
    const int buttonFontSize = 20;
    
    // Draw semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 100});
    
    // Result Text
    const char* resultText;
    Color resultColor;
    
    if (winner == PLAYER_X) {
        resultText = isTwoPlayer ? "Player X Wins!" : "You win!";
        resultColor = BLUE;
    } else if (winner == PLAYER_O) {
        resultText = isTwoPlayer ? "Player O Wins!" : "U so noob!";
        resultColor = RED;
    } else {
        resultText = "It's a Draw!";
        resultColor = DARKGRAY;
    }
    
    // Draw result text with background
    int textWidth = MeasureText(resultText, titleFontSize);
    DrawRectangle(
        SCREEN_WIDTH/2 - textWidth/2 - 10,
        SCREEN_HEIGHT/3 - 10,
        textWidth + 20,
        titleFontSize + 20,
        WHITE
    );
    DrawText(resultText, 
        SCREEN_WIDTH/2 - textWidth/2,
        SCREEN_HEIGHT/3,
        titleFontSize,
        resultColor
    );
    
    // Retry Button
    Rectangle retryBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + 40, // Position above the menu button
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    // Back to Menu Button
    Rectangle menuBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + 100,  // Position below the retry button
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
          
    Vector2 mousePos = GetMousePosition();
    bool isHoveringMenu = CheckCollisionPointRec(mousePos, menuBtn);
    bool isHoveringRetry = CheckCollisionPointRec(mousePos, retryBtn);
    
    // Draw buttons with hover effect
    DrawButton(retryBtn, "Retry", buttonFontSize, isHoveringRetry);
    DrawButton(menuBtn, "Back to Menu", buttonFontSize, isHoveringMenu);
    
    // Set cursor
    SetMouseCursor((isHoveringMenu || isHoveringRetry) ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

// Add the function definition
void DrawButton(Rectangle bounds, const char* text, int fontSize, bool isHovered) {
    Rectangle vibrationBounds = bounds;
    
    // Apply vibration effect to all buttons when hovered
    if (isHovered) {
        buttonVibrationOffset = sinf(GetTime() * vibrationSpeed) * vibrationAmount;
        vibrationBounds.x += buttonVibrationOffset;
    }

    // Draw the button background
    DrawRectangleRec(vibrationBounds, isHovered ? GRAY : LIGHTGRAY); // Draw the button background with a gray color if hovered

    // Draw the button outline
    DrawRectangleLinesEx(vibrationBounds, 2, BLACK); // Draw the button outline with a black color

    // Draw the button text
    DrawText(text,
        vibrationBounds.x + (vibrationBounds.width - MeasureText(text, fontSize))/2, // Center the text horizontally
        vibrationBounds.y + (vibrationBounds.height - fontSize)/2, // Center the text vertically
        fontSize,
        BLACK
    );
}

void InitGame() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = EMPTY;
        }
    }
    gameOver = false;
    winner = EMPTY;
    currentPlayerTurn = PLAYER_X_TURN;
}

void UpdateGameOver(Sound buttonClickSound) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        
        // Retry Button
        Rectangle retryBtn = {
            SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
            SCREEN_HEIGHT/2 + 40,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        
        // Back to Menu Button
        Rectangle menuBtn = {
            SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
            SCREEN_HEIGHT/2 + 100,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        
        if (CheckCollisionPointRec(mousePos, menuBtn)) {
            PlaySound(buttonClickSound);  // Play sound on button click
            gameState = MENU;
            InitGame();  // Reset the game state
        } else if (CheckCollisionPointRec(mousePos, retryBtn)) {
            PlaySound(buttonClickSound);  // Play sound on button click
            gameState = GAME;
            InitGame();  // Reset the game state for a new game
        }
    }
}

void DrawDifficultySelect() {
    const int titleFontSize = 40;
    const int buttonFontSize = 20;
    
    // Title
    const char* title = "Select Difficulty";
    DrawText(title, 
        SCREEN_WIDTH/2 - MeasureText(title, titleFontSize)/2, 
        SCREEN_HEIGHT/3,
        titleFontSize,
        BLACK);
    
    // Button rectangles
    Rectangle easyBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Rectangle mediumBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Rectangle hardBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

    // Add back button at top left
    Rectangle backBtn = {
        20,                // Left margin
        10,                // Top margin  
        SCREEN_WIDTH/6,    // Width (100px at 600px screen width)
        30                 // Height
    };

    Vector2 mousePos = GetMousePosition();
    
    // Check hover states
    bool easyHover = CheckCollisionPointRec(mousePos, easyBtn);
    bool mediumHover = CheckCollisionPointRec(mousePos, mediumBtn);
    bool hardHover = CheckCollisionPointRec(mousePos, hardBtn);
    bool backHover = CheckCollisionPointRec(mousePos, backBtn);

    // Draw buttons with hover effects
    DrawButton(easyBtn, "Easy", buttonFontSize, easyHover);
    DrawButton(mediumBtn, "Medium", buttonFontSize, mediumHover);
    DrawButton(hardBtn, "Hard", buttonFontSize, hardHover);
    DrawButton(backBtn, "Back", buttonFontSize, backHover);

    // Set cursor based on any button hover
    SetMouseCursor((easyHover || mediumHover || hardHover || backHover) ? 
        MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit) {
    if (depth >= depthLimit) return 0; // Return 0 if depth limit is reached

    int score = EvaluateBoard(board);
    if (score == 10) return score - depth; // O (AI) is the maximizing player
    if (score == -10) return score + depth; // X (human) is the minimizing player
    if (CheckDraw()) return 0; // Draw

    if (isMaximizing) {
        int bestScore = -1000; // Initialize the best score to a very low value
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) { // Iterate through each cell in the grid
                if (board[i][j] == EMPTY) { // If the cell is empty
                    board[i][j] = PLAYER_O; // Set the cell to PLAYER_O
                    bestScore = fmax(bestScore, Minimax(board, false, depth + 1, depthLimit)); // Update the best score
                    board[i][j] = EMPTY; // Reset the cell to EMPTY
                }
            }
        }
        return bestScore;
    } else {
        int bestScore = 1000; // Initialize the best score to a very high value
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) { // Iterate through each cell in the grid
                if (board[i][j] == EMPTY) { // If the cell is empty
                    board[i][j] = PLAYER_X; // Set the cell to PLAYER_X
                    bestScore = fmin(bestScore, Minimax(board, true, depth + 1, depthLimit)); // Update the best score
                    board[i][j] = EMPTY; // Reset the cell to EMPTY
                }
            }
        }
        return bestScore; // Return the best score
    }
}

// Evaluate the board
int EvaluateBoard(Cell board[GRID_SIZE][GRID_SIZE]) {
    // Check rows and columns for a win
    for (int row = 0; row < GRID_SIZE; row++) {
        if (board[row][0] == board[row][1] && board[row][0] == board[row][2]) {
            if (board[row][0] == PLAYER_O) return 10;
            else if (board[row][0] == PLAYER_X) return -10;
        }
    }
    for (int col = 0; col < GRID_SIZE; col++) {
        if (board[0][col] == board[1][col] && board[0][col] == board[2][col]) {
            if (board[0][col] == PLAYER_O) return 10;
            else if (board[0][col] == PLAYER_X) return -10;
        }
    }

    // Check diagonals for a win
    if (board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
        if (board[0][0] == PLAYER_O) return 10;
        else if (board[0][0] == PLAYER_X) return -10;
    }
    if (board[0][2] == board[1][1] && board[0][2] == board[2][0]) {
        if (board[0][2] == PLAYER_O) return 10;
        else if (board[0][2] == PLAYER_X) return -10;
    }

    return 0; // No winner
}

// gcc -o main main.c -I. -L. -lraylib -lopengl32 -lgdi32 -lwinmm

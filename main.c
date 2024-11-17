#include "main.h"

// Define the global arrays
GridSymbol titleSymbols[TITLE_GRID_SIZE][TITLE_GRID_SIZE];
FallingSymbol symbols[MAX_SYMBOLS];
TitleWord titleWords[5];
Difficulty currentDifficulty = MEDIUM; // Initialize difficulty to a value, doesn't have to be medium
Cell grid[GRID_SIZE][GRID_SIZE]; // Initialize the grid with empty cells
PlayerTurn currentPlayerTurn = PLAYER_X_TURN; // Initialize the current player turn to Player X
bool gameOver = false; // Initialize the game over flag to false
Cell winner = EMPTY; // Initialize the winner to empty
GameState gameState = MENU; // Initialize the game state to menu
bool isTwoPlayer = false; // Flag to check if it's a two-player or single-player game
float titleCellScales[TITLE_GRID_SIZE][TITLE_GRID_SIZE] = {0};
float titleRotations[TITLE_GRID_SIZE][TITLE_GRID_SIZE] = {0};
float titleAnimSpeed = 2.0f; // Animation speed for title cells
float buttonVibrationOffset = 0.0f; // Vibration offset for buttons
float vibrationSpeed = 15.0f; // Speed of vibration, increase this to intensify the vibration
float vibrationAmount = 2.0f; // Amount of vibration
AIModel currentModel = NAIVE_BAYES; // Default to Naive Bayes
int totalGames = 0;
int aiWins = 0;
struct GetHint hint;

// Define variables
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

// Main function
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic-Tac-Toe");
    InitAudioDevice();  // Initialize audio device

    // Load the icon image
    Image icon = LoadImage("assets\\icon.png");  // Make sure the file path is correct
    SetWindowIcon(icon);  // Set the window icon
    UnloadImage(icon);  // Unload the image after setting the icon

    buttonClickSound = LoadSound("assets\\ButtonClicked.mp3");  // Load the button click sound
    popSound = LoadSound("assets\\Pop.mp3");  // Load the pop sound
    victorySound = LoadSound("assets\\FFVictory.mp3");  // Load the victory sound
    loseSound = LoadSound("assets\\MarioLose.mp3");  // Load the lose sound
    drawSound = LoadSound("assets\\Draw.mp3");  // Load the draw sound
    mainMenuSound = LoadSound("assets\\MainMenu.mp3");  // Load the main menu sound
    playSound = LoadSound("assets\\Play.mp3");  // Load the play sound

    // After loading each sound, set its volume (between 0.0f to 1.0f)
    SetSoundVolume(buttonClickSound, 0.3f);  // 30% volume
    SetSoundVolume(popSound, 0.3f);
    SetSoundVolume(victorySound, 0.3f);
    SetSoundVolume(loseSound, 0.3f);
    SetSoundVolume(drawSound, 0.3f);
    SetSoundVolume(mainMenuSound, 0.2f);  // Background music slightly lower
    SetSoundVolume(playSound, 0.2f);      // Background music slightly lower

    InitSymbols();  // Initialize the falling symbols
    InitTitleWords();  // Initialize the title words

    // Naive Bayes Machine Learning for easy mode
    char boards[1000][NUM_POSITIONS + 1];       // Array to store attrbutes of tic-tac-toe.data dataset
    int outcomes[1000];                         // Array to store outcomes of tic-tac-toe.data dataset
    int total_records = 0;                      // Count for number of lines in dataset

    // Load data
    load_data("tic-tac-toe.data", boards, outcomes, &total_records);

    // Split data
    char train_boards[800][NUM_POSITIONS + 1];  // Array for attributes of training dataset
    int train_outcomes[800];                    // Array for outcomes of training dataset
    char test_boards[200][NUM_POSITIONS + 1];   // Array for attributes of testing dataset
    int test_outcomes[200];                     // Array for outcomes of testing dataset
    int train_size = 0, test_size = 0;          // Count number of lines in training and testing dataset respectively

    split_data(boards, outcomes, total_records, train_boards, train_outcomes, test_boards, test_outcomes, &train_size, &test_size, RATIO);

    // Train model
    NaiveBayesModel NBmodel;
    train_NBmodel(&NBmodel, train_boards, train_outcomes, train_size);

    // Save model weights to a file
    save_NBmodel(&NBmodel, "NBmodel/NBmodel_weights.txt");

    // Test model
    test_NBmodel(&NBmodel, test_boards, test_outcomes, test_size);
    // End of Machine Learning

    while (!WindowShouldClose())
    {
        if (gameState == MENU || gameState == DIFFICULTY_SELECT || gameState == MODEL_SELECT) {
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

        if (gameState == MENU || gameState == DIFFICULTY_SELECT || gameState == MODEL_SELECT) {
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
            UpdateGame(buttonClickSound, popSound, victorySound, loseSound, drawSound, &NBmodel);
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
                        gameState = MODEL_SELECT;   // go to model selection instead of game
                        InitGame();
                    }
                    // medium button
                    else if (mousePos.y >= SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20 && 
                             mousePos.y <= SCREEN_HEIGHT/2 + BUTTON_HEIGHT * 2 + 20) {
                        PlaySound(buttonClickSound);  // Play sound on button click
                        currentDifficulty = MEDIUM; // go to imperfect minimax
                        gameState = GAME;
                        InitGame();
                    }
                    // hard button
                    else if (mousePos.y >= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 && 
                             mousePos.y <= SCREEN_HEIGHT/2 + (BUTTON_HEIGHT + 20) * 2 + BUTTON_HEIGHT) {
                        PlaySound(buttonClickSound);  // Play sound on button click
                        currentDifficulty = HARD; // go to perfect minimax
                        gameState = GAME;
                        InitGame();
                    }
                }
            }
        }
        else if (gameState == MODEL_SELECT) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();

                // Back button
                if (mousePos.x >= 20 && mousePos.x <= SCREEN_WIDTH/6 && mousePos.y >= 10 && mousePos.y <= 40) {
                    PlaySound(buttonClickSound);  // Play sound on button click
                    gameState = DIFFICULTY_SELECT;
                }

                // Naive Bayes button
                Rectangle nbBtn = {
                    SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
                    SCREEN_HEIGHT/2,
                    BUTTON_WIDTH,
                    BUTTON_HEIGHT
                };

                // Decision Tree button
                Rectangle dtBtn = {
                    SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
                    SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20,
                    BUTTON_WIDTH,
                    BUTTON_HEIGHT
                };
                
                if (CheckCollisionPointRec(mousePos, nbBtn)) {
                    PlaySound(buttonClickSound);
                    currentModel = NAIVE_BAYES;
                    gameState = GAME;
                    InitGame();
                }
                else if (CheckCollisionPointRec(mousePos, dtBtn)) {
                    PlaySound(buttonClickSound);
                    currentModel = DECISION_TREE;
                    gameState = GAME;
                    InitGame();
                }
            }
        }

        BeginDrawing();  // Begin drawing
        ClearBackground(RAYWHITE); // Clear the background to white

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
            case MODEL_SELECT:
                DrawSymbols();  // Draw the falling symbols
                DrawModelSelect(); // Draw the model selection
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

// Initialize the symbols
void InitSymbols() {
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        symbols[i].position = (Vector2){ GetRandomValue(0, SCREEN_WIDTH), GetRandomValue(-SCREEN_HEIGHT, 0) };
        symbols[i].symbol = GetRandomValue(0, 1) ? 'X' : 'O';
        symbols[i].rotation = GetRandomValue(0, 360);  // Random initial rotation
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

// Drawing of the Game Designs
// Draw the title words
void DrawTitleWords() {
    for (int i = 0; i < 5; i++) {
        DrawText(titleWords[i].word, titleWords[i].position.x, titleWords[i].position.y, 40, BLACK);
    }
}

// Draw the symbols
void DrawSymbols() {
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        Vector2 origin = {10, 10};  // Center of rotation
        DrawTextPro(GetFontDefault(), &symbols[i].symbol, symbols[i].position, origin, symbols[i].rotation, 20, 1, symbols[i].symbol == 'X' ? BLUE : RED);
    }
}

// Draw the game
void DrawGame()
{
    Vector2 mousePos = GetMousePosition();
    // Hint button hover
    bool isHintHovered = (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
                         mousePos.y >= 50 && mousePos.y <= 80);
    // Quit button hover 
    bool isQuitHovered = (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
                         mousePos.y >= 10 && mousePos.y <= 40);
    
    // Only set cursor for button if we're not in game over state
    if (!gameOver && isQuitHovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else if (!gameOver && isHintHovered) {
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

    // Hint button
    Rectangle hintBtn = {
        SCREEN_WIDTH - 80, 50,  // position
        70, 30                  // size
    };

    // Use the existing mousePos variable instead of declaring a new one
    // Update isHintHovered using CheckCollisionPointRec
    isHintHovered = CheckCollisionPointRec(mousePos, hintBtn);
    DrawButton(hintBtn, "Hint", 20, !gameOver && isHintHovered);    // draw the hint button

    // Quit button
    Rectangle quitBtn = {
        SCREEN_WIDTH - 80, 10,  // position
        70, 30                  // size
    };
    
    // Use the existing mousePos variable instead of declaring a new one
    // Update isQuitHovered using CheckCollisionPointRec
    isQuitHovered = CheckCollisionPointRec(mousePos, quitBtn);
    DrawButton(quitBtn, "Quit", 20, !gameOver && isQuitHovered);    // draw the quit button

    if (!gameOver) {
        // only display stats for single player mode
        if (!isTwoPlayer) {
            char statsText[100];
            ModeStats* currentStats = GetCurrentModeStats();

            sprintf(statsText, "Player: %d | AI: %d | Draws: %d", 
                    currentStats->playerWins, 
                    currentStats->aiWins, 
                    currentStats->draws);

            // draw stats in middle above turn display
            DrawText(statsText, SCREEN_WIDTH/2 - MeasureText(statsText, 20)/2, 10, 20, BLACK);
        }

        // turn display indicator
        int yPos = isTwoPlayer ? 20 : 40;  // shift up for 2 player mode

        if (currentPlayerTurn == PLAYER_X_TURN) {
            const char* turnText = "Player X's Turn";
            DrawText(turnText, SCREEN_WIDTH/2 - MeasureText(turnText, 30)/2, yPos, 30, BLUE);
        } else {
            const char* turnText = "Player O's Turn";
            DrawText(turnText, SCREEN_WIDTH/2 - MeasureText(turnText, 30)/2, yPos, 30, RED);
        }
    }
}

// Draw the menu
void DrawMenu() {
    const int titleFontSize = 40;
    const int buttonFontSize = 20;
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
                titleSymbols[i][j].alpha = 0; // reset to transparent
                titleSymbols[i][j].active = true;
            }

            if (titleSymbols[i][j].active) {
                titleSymbols[i][j].alpha += GetFrameTime() * 2;
                if (titleSymbols[i][j].alpha > 1.0f) {
                    titleSymbols[i][j].alpha = 0; // reset to transparent
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

// Draw the game over screen
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
        resultText = isTwoPlayer ? "Player O Wins!" : "You lose!";
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
    bool isMenuHovered = CheckCollisionPointRec(mousePos, menuBtn);
    bool isRetryHovered = CheckCollisionPointRec(mousePos, retryBtn);
    
    // Draw buttons with hover effect
    DrawButton(retryBtn, "Retry", buttonFontSize, isRetryHovered);
    DrawButton(menuBtn, "Back to Menu", buttonFontSize, isMenuHovered);
    
    // Set cursor
    SetMouseCursor((isMenuHovered || isRetryHovered) ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

// draw the buttons with hover effect
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

// Draw the difficulty selection screen
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

// Draw the AI model selection screen
void DrawModelSelect() {
    const char* title = "Select AI Model";
    DrawText(title, 
        SCREEN_WIDTH/2 - MeasureText(title, 40)/2,
        SCREEN_HEIGHT/3,
        40,
        BLACK);
        
    Rectangle nbBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    Rectangle dtBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT/2 + BUTTON_HEIGHT + 20,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };
    
    // back button at top left
    Rectangle backBtn = {
        20,                // Left margin
        10,                // Top margin  
        SCREEN_WIDTH/6,    // Width (100px at 600px screen width)
        30                 // Height
    };

    Vector2 mousePos = GetMousePosition();
    bool nbHover = CheckCollisionPointRec(mousePos, nbBtn);
    bool dtHover = CheckCollisionPointRec(mousePos, dtBtn);
    bool backHover = CheckCollisionPointRec(mousePos, backBtn);

    DrawButton(nbBtn, "Naive Bayes", 20, nbHover);
    DrawButton(dtBtn, "Decision Tree", 20, dtHover);
    DrawButton(backBtn, "Back", 20, backHover);
    
    SetMouseCursor((nbHover || dtHover || backHover) ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

// Game Functions
// initialize the game
void InitGame() {
    // Stop all sounds that might be playing
    StopSound(victorySound);
    StopSound(loseSound); 
    StopSound(drawSound);

    // Initialize the grid to EMPTY in a single loop
    memset(grid, EMPTY, sizeof(grid));
    gameOver = false;
    winner = EMPTY;
    currentPlayerTurn = PLAYER_X_TURN;
}

ModeStats* GetCurrentModeStats() {
    if (currentDifficulty == EASY) {
        return (currentModel == NAIVE_BAYES) ? &naiveBayesStats : &decisionTreeStats;
    }
    return (currentDifficulty == MEDIUM) ? &mediumStats : &hardStats;
}

// Update the game
void UpdateGame(Sound buttonClickSound, Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound, NaiveBayesModel *model)
{
    if (gameOver) return;  // Exit if game is already over
    
    // Handle quit button click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
            mousePos.y >= 10 && mousePos.y <= 40)
        {
            PlaySound(buttonClickSound);
            gameState = MENU;
            return;
        }
    }

    // Handle moves based on whose turn it is
    if (currentPlayerTurn == PLAYER_X_TURN)
    {
        // Handle human player X's turn
        if (HandlePlayerTurn(popSound, victorySound, loseSound, drawSound)) {
            PlaySound(popSound);
        }
    }
    else if (currentPlayerTurn == PLAYER_O_TURN)
    {
        if (isTwoPlayer)
        {
            // In 2 player mode, handle human player O's turn
            if (HandlePlayerTurn(popSound, victorySound, loseSound, drawSound)) {
                PlaySound(popSound);
            }
        }
        else
        {
            // In single player mode, handle AI's turn based on difficulty
            switch(currentDifficulty) {
                case EASY:
                    // Use ML models (Naive Bayes or Decision Tree) for EASY mode
                    if (currentModel == NAIVE_BAYES) {
                        AITurn(victorySound, loseSound, drawSound, model);  // Naive Bayes
                    } else {
                        AITurnDecisionTree();  // Decision Tree
                    }
                    break;
                    
                case MEDIUM:
                    // Use limited depth Minimax for MEDIUM
                    AITurn(victorySound, loseSound, drawSound, model);  // This uses depthLimit = 4
                    break;
                    
                case HARD:
                    // Use full depth Minimax for HARD
                    AITurn(victorySound, loseSound, drawSound, model);  // This uses depthLimit = 9
                    break;
            }
        }
    }
}

// Handle the player's turn
bool HandlePlayerTurn(Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound)
{   
    clearHint();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        int row = (int)(mousePos.y / CELL_SIZE); 
        int col = (int)(mousePos.x / CELL_SIZE);
        // Handle hint button click
        if (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
            mousePos.y >= 50 && mousePos.y <= 80)
        {
            PlaySound(buttonClickSound);
            getHint();
            row = hint.row; // overwrites the mouse position
            col = hint.col; // overwrites the mouse position
        }

        // When updating stats, use the current mode's counter:
        ModeStats* currentStats = GetCurrentModeStats();

        // check win after a grid is selected
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
                            currentStats->playerWins++; // Increment player wins
                            currentStats->totalGames++; // Increment total games
                            PlaySound(victorySound);  // Play victory sound for Player X
                        } 
                        else {
                            currentStats->aiWins++; // Increment AI wins
                            currentStats->totalGames++; // Increment total games
                            PlaySound(loseSound);  // Play lose sound for Player O
                        }
                    } 
                    else {
                        PlaySound(victorySound);  // Play victory sound for any winner in two-player mode
                    }
                }
                else if (CheckDraw()) { // Check for a draw
                    gameOver = true;
                    gameState = GAME_OVER;
                    winner = EMPTY;  // No winner in a draw
                    currentStats->draws++;  // Increment draws scores
                    currentStats->totalGames++; // Increment total games
                    PlaySound(drawSound);  // Play draw sound
                }
                else {
                    currentPlayerTurn = (currentPlayerTurn == PLAYER_X_TURN) ? PLAYER_O_TURN : PLAYER_X_TURN; // change player turn
                }
                return true;  // Move was made
            }
        }
    }
    return false;  // No move was made
}

// AI's turn using MiniMax algorithms and Machine Learning models
void AITurn(Sound victorySound, Sound loseSound, Sound drawSound, NaiveBayesModel *model)
{
    int bestScore = -1000;
    int bestRow = -1;
    int bestCol = -1;

    // Ensure this function only applies in medium and hard modes
    if (currentDifficulty == EASY) {
        if (currentModel == NAIVE_BAYES) {
            predict_move(model, grid, &bestRow, &bestCol);
        } 
        else {
            AITurnDecisionTree();  // New function to implement
        }
    }

    // Medium mode: use Minimax with limited depth search of 4
    if (currentDifficulty == MEDIUM) {
        int depthLimit = 4; // Set a depth limit for medium difficulty
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] == EMPTY) {
                    grid[i][j] = PLAYER_O;
                    int score = Minimax(grid, false, 0, depthLimit, -1000, 1000);
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
    else if (currentDifficulty == HARD) {
        int depthLimit = 9; // Full depth for hard mode
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (grid[i][j] == EMPTY) {
                    grid[i][j] = PLAYER_O;
                    int score = Minimax(grid, false, 0, depthLimit, -1000, 1000);
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

    // Get the current mode's stats
    ModeStats* currentStats = GetCurrentModeStats();

    if (CheckWin(PLAYER_O)) {
        gameOver = true;
        winner = PLAYER_O;
        gameState = GAME_OVER;
        
        // Update the stats counter
        currentStats->aiWins++;
        currentStats->totalGames++;

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
        winner = EMPTY;
        
        // Update the stats counter
        currentStats->draws++;
        currentStats->totalGames++;

        PlaySound(drawSound);  // Play draw sound
    } 
    else {
        currentPlayerTurn = PLAYER_X_TURN;
    }
}

// Decision Tree AI Turn function
void AITurnDecisionTree() {
    int row, col;
    do {
        row = GetRandomValue(0, GRID_SIZE - 1);
        col = GetRandomValue(0, GRID_SIZE - 1);
    } while (grid[row][col] != EMPTY);
    
    grid[row][col] = PLAYER_O;

    // Get current stats for Easy mode
    ModeStats* currentStats = &decisionTreeStats;
    
    if (CheckWin(PLAYER_O)) {
        gameOver = true;
        winner = PLAYER_O;
        gameState = GAME_OVER;
        currentStats->aiWins++; // Increment AI wins
        currentStats->totalGames++; // Increment total games

        PlaySound(loseSound);
    } else if (CheckDraw()) {
        gameOver = true;
        gameState = GAME_OVER;
        winner = EMPTY;
        currentStats->draws++; // Increment draws score
        currentStats->totalGames++; // Increment total games

        PlaySound(drawSound);
    } else {
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

// update the game when its over
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

// Clear Hint 
void clearHint() {
    hint.row = -1;
    hint.col = -1;
}
// Get Hint for Player
void getHint() {
    int player = (currentPlayerTurn == PLAYER_X_TURN) ? PLAYER_X : PLAYER_O;
    int bestScore = -1000;
    int bestRow = -1;
    int bestCol = -1;
    int depthLimit = 9; // Full depth
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == EMPTY) {
                grid[i][j] = PLAYER_O;
                int score = Minimax(grid, false, 0, depthLimit, -1000, 1000);
                grid[i][j] = EMPTY;

                if (score > bestScore) {
                    bestScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }
    // save best move
    if (bestRow != -1 && bestCol != -1) {
        hint.row = bestRow;
        hint.col = bestCol;
    }
}

// minimax algorithm, factorial design
int Minimax(Cell board[GRID_SIZE][GRID_SIZE], bool isMaximizing, int depth, int depthLimit, int alpha, int beta) {
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
                    bestScore = fmax(bestScore, Minimax(board, false, depth + 1, depthLimit, alpha, beta)); // Update the best score
                    board[i][j] = EMPTY; // Reset the cell to EMPTY
                    alpha = fmax(alpha, bestScore); // Update alpha (maximize)
                    if (beta <= alpha) break; // Beta cut-off (prune the branch, stopping the recursion)
                    
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
                    bestScore = fmin(bestScore, Minimax(board, true, depth + 1, depthLimit, alpha, beta)); // Update the best score
                    board[i][j] = EMPTY; // Reset the cell to EMPTY
                    beta = fmin(beta, bestScore); // Update beta (minimize)
                    if (beta <= alpha) break; // Alpha cut-off (prune the branch, stopping the recursion)
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

// gcc -o main main.c NBmodel/data_processing.c NBmodel/NBmodel.c -I. -L. -lraylib -lopengl32 -lgdi32 -lwinmm
// ./main
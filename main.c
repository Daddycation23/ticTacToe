#include "main.h"
#include "DecisionTree_ML/decisiontree.h"


// Define the global variables
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
int aiWins = 0; // Set aiWins to 0
int totalGames = 0; // Set the total number of games to 0
Confetti confetti[MAX_CONFETTI]; // Set the maximum number of confetti particles
bool showPartyAnimation = false; // Flag to check if the party animation should be shown
bool allInactive = true; // Flag to check if all confetti particles are inactive
struct GetHint hint = { -1, -1, 0, 0}; // Declare hint object to store best move and hint counts for player 
int winningCells[3][2] = {{-1,-1}, {-1,-1}, {-1,-1}}; // Store winning cell coordinates

// Initialize the ModeStats structs
ModeStats mediumStats = {0, 0, 0, 0};
ModeStats hardStats = {0, 0, 0, 0};
ModeStats naiveBayesStats = {0, 0, 0, 0};
ModeStats decisionTreeStats = {0, 0, 0, 0};

// Initialize the sound variables
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
    SetSoundVolume(buttonClickSound, 0.4f);  // 40% volume
    SetSoundVolume(popSound, 0.4f);
    SetSoundVolume(victorySound, 0.4f);
    SetSoundVolume(loseSound, 0.4f);
    SetSoundVolume(drawSound, 0.4f);
    SetSoundVolume(mainMenuSound, 0.4f);
    SetSoundVolume(playSound, 0.4f);

    InitSymbols();  // Initialize the falling symbols
    InitTitleWords();  // Initialize the title words
    InitConfetti(); // Initialize the confetti

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
    char mode[] = "w";
    char type[] = "Training";
    test_NBmodel("NBmodel/NBmodel_confusion_matrix.txt", mode, type, &NBmodel, train_boards, train_outcomes, train_size);
    strcpy(mode, "a");
    strcpy(type, "Testing");
    test_NBmodel("NBmodel/NBmodel_confusion_matrix.txt", mode, type, &NBmodel, test_boards, test_outcomes, test_size);
    // End of Machine Learning

    DecisionTreeNode TDmodel;
    growth_Tree(&TDmodel); 

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
            UpdateGame(buttonClickSound, popSound, victorySound, loseSound, drawSound, &NBmodel, &TDmodel);
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
        if (gameState!=GAME && gameState!=GAME_OVER)
        {
            // resets hintCount when not in game
            hint.hintCountX = 0;
            hint.hintCountO = 0;
        }
        
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
                if (showPartyAnimation == true) {   // If the party animation is active
                    UpdateConfetti();   // Update the confetti
                    DrawConfetti(); // Draw the confetti
                }
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

// Initialize the confetti
void InitConfetti() {
    for (int i = 0; i < MAX_CONFETTI; i++) {
        // Start all particles from bottom right corner with some variation
        confetti[i].position = (Vector2){
            SCREEN_WIDTH - GetRandomValue(30, 70),  // More variation in start position
            SCREEN_HEIGHT - GetRandomValue(30, 70)
        };
        
        // Wider spray pattern (160° to 280° for almost full semicircle)
        float angle = GetRandomValue(160, 280) * DEG2RAD;  // Increased angle range
        float speed = GetRandomValue(600, 1200) / 100.0f;  // Increased speed range
        confetti[i].velocity = (Vector2){
            cos(angle) * speed,
            sin(angle) * speed
        };
        
        // Festive colors for party popper
        switch(GetRandomValue(0, 4)) {
            case 0: confetti[i].color = (Color){255, 50, 50, 255};   // Red
                break;
            case 1: confetti[i].color = (Color){50, 255, 50, 255};   // Green
                break;
            case 2: confetti[i].color = (Color){50, 50, 255, 255};   // Blue
                break;
            case 3: confetti[i].color = (Color){255, 255, 50, 255};  // Yellow
                break;
            case 4: confetti[i].color = (Color){255, 50, 255, 255};  // Pink
                break;
        }
        
        confetti[i].size = GetRandomValue(2, 4);
        confetti[i].active = true;
        confetti[i].alpha = 1.0f;
        confetti[i].lifetime = GetRandomValue(150, 200) / 100.0f;
    }
}

// Update the confetti animation
void UpdateConfetti() {
    for (int i = 0; i < MAX_CONFETTI; i++) {
        if (confetti[i].active) {
            allInactive = false;    // Reset the flag
            
            // Update position with drag effect
            confetti[i].velocity.x *= 0.99f;
            confetti[i].velocity.y *= 0.99f;
            
            // Increased movement multiplier for wider spread
            confetti[i].position.x += confetti[i].velocity.x * 0.6f;  // Increased from 0.4f
            confetti[i].position.y += confetti[i].velocity.y * 0.6f;  // Increased from 0.4f
            
            // Reduced gravity for more horizontal movement
            confetti[i].velocity.y += 0.02f;
            
            // Increased random movement for more spread
            confetti[i].velocity.x += GetRandomValue(-20, 20) / 100.0f;  // Increased range
            confetti[i].velocity.y += GetRandomValue(-20, 20) / 100.0f;  // Increased range
            
            // Slower fade out
            confetti[i].alpha -= 0.002f;
            confetti[i].lifetime -= 0.002f;
            
            // Increased bounds for off-screen check to allow more spread
            if (confetti[i].alpha <= 0 || 
                confetti[i].lifetime <= 0 ||
                confetti[i].position.y > SCREEN_HEIGHT + 50 ||  // Increased bounds
                confetti[i].position.x < -50 ||                 // Increased bounds
                confetti[i].position.x > SCREEN_WIDTH + 50) {   // Increased bounds
                confetti[i].active = false;
            }
        }
    }
    
    if (allInactive) {
        showPartyAnimation = false; // Stop the party animation
    }
}

// Draw the confetti
void DrawConfetti() {
    for (int i = 0; i < MAX_CONFETTI; i++) {
        if (confetti[i].active) {
            Color particleColor = confetti[i].color;
            particleColor.a = (unsigned char)(confetti[i].alpha * 255);
            
            // Longer trails for more visible effect
            Vector2 direction = {
                -confetti[i].velocity.x * 0.15f,  // Increased from 0.1f
                -confetti[i].velocity.y * 0.15f   // Increased from 0.1f
            };
            
            // Draw main particle
            DrawCircle(
                confetti[i].position.x,
                confetti[i].position.y,
                confetti[i].size,
                particleColor
            );
            
            // Longer trails with more segments
            for (int trail = 1; trail <= 7; trail++) {  // Increased from 5 to 7 segments
                float trailAlpha = confetti[i].alpha * (1.0f - (trail * 0.14f));  // Adjusted fade
                Vector2 trailPos = {
                    confetti[i].position.x + direction.x * trail,
                    confetti[i].position.y + direction.y * trail
                };
                
                DrawCircle(
                    trailPos.x,
                    trailPos.y,
                    confetti[i].size * (1.0f - (trail * 0.12f)),  // Adjusted size reduction
                    ColorAlpha(particleColor, trailAlpha * 255)
                );
            }
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
void DrawGame() {
    bool isHintHovered = false;
    Vector2 mousePos = GetMousePosition(); 
    
    // The grid and pieces
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            Rectangle cell = {(float)(j * CELL_SIZE), (float)(i * CELL_SIZE), (float)CELL_SIZE, (float)CELL_SIZE};
            
            // Check if this cell is part of the winning combination
            bool isWinningCell = false;
            if (gameOver && winner != EMPTY) {
                for (int k = 0; k < 3; k++) {
                    if (winningCells[k][0] == i && winningCells[k][1] == j) {
                        isWinningCell = true;
                        break;
                    }
                }
            }
            
            // Check if the mouse is hovering over the cell and the cell is empty
            bool isHovered = !gameOver && grid[i][j] == EMPTY && CheckCollisionPointRec(mousePos, cell);
            
            // Draw the cell with appropriate color
            Color cellColor;
            if (isWinningCell) {
                if (!isTwoPlayer && winner == PLAYER_O) {
                    cellColor = (Color){ 255, 200, 200, 255 }; // Light red highlight for AI wins
                } else {
                    cellColor = (Color){ 144, 238, 144, 255 }; // Light green highlight for player wins
                }
            } else {
                cellColor = isHovered ? DARKGRAY : LIGHTGRAY;
            }
            
            DrawRectangleRec(cell, cellColor);

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

    // Hint button position
    Rectangle hintBtn = {
        SCREEN_WIDTH - 80, 10,  // moved to top right
        70, 30
    };
    // Hint button counts left for player
    const char *hintText = "Hint: ";
    char hintTextFinal[10];
    // hintCount for player X
    snprintf(hintTextFinal, sizeof(hintTextFinal), "%s%d", hintText, (2 - hint.hintCountX)); // hint button text
    if (currentPlayerTurn==PLAYER_X_TURN){
        if (hint.hintCountX < 2) // hint button active when count < 2
        {
            isHintHovered = (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 && mousePos.y >= 10 && mousePos.y <= 40);
            DrawButton(hintBtn, hintTextFinal, 20, !gameOver && isHintHovered);
        } else 
        {
            DrawButton(hintBtn, hintTextFinal, 20, !gameOver && false);
        }    
    }
    // hintCount for player O
    snprintf(hintTextFinal, sizeof(hintTextFinal), "%s%d", hintText, (2 - hint.hintCountO)); // hint button text
    if (currentPlayerTurn==PLAYER_O_TURN)
    {
        if (hint.hintCountO < 2) // hint button active when count < 2
        {
            isHintHovered = (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 && mousePos.y >= 10 && mousePos.y <= 40);
            DrawButton(hintBtn, hintTextFinal, 20, !gameOver && isHintHovered);    
        } else 
        {
            DrawButton(hintBtn, hintTextFinal, 20, !gameOver && false);
        }
    } 
    // Quit button position
    Rectangle quitBtn = {
        20, 10,  // moved to top left
        70, 30
    };
    bool isQuitHovered = (mousePos.x >= 20 && mousePos.x <= 90 && mousePos.y >= 10 && mousePos.y <= 40);
    DrawButton(quitBtn, "Quit", 20, !gameOver && isQuitHovered);

    // Only set cursor for button if we're not in game over state
    if (!gameOver && isQuitHovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else if (!gameOver && isHintHovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else if (!gameOver) {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

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
            const char* turnText = isTwoPlayer ? "Player X's Turn" : "Your Turn";
            DrawText(turnText, SCREEN_WIDTH/2 - MeasureText(turnText, 30)/2, yPos, 30, BLUE);
        } else {
            const char* turnText = isTwoPlayer ? "Player O's Turn" : "AI's Turn";
            DrawText(turnText, SCREEN_WIDTH/2 - MeasureText(turnText, 30)/2, yPos, 30, RED);
        }
    }
}

bool HandleButtonHover(Rectangle button, const char* text, int fontSize, bool* isHovered) {
    Vector2 mousePos = GetMousePosition();
    *isHovered = CheckCollisionPointRec(mousePos, button);
    DrawButton(button, text, fontSize, *isHovered);
    return *isHovered;
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
    
    // Check hover states
    bool singlePlayerHover = false;
    bool twoPlayerHover = false;
    bool exitHover = false;

    HandleButtonHover(singlePlayerBtn, "Single Player", buttonFontSize, &singlePlayerHover);
    HandleButtonHover(twoPlayerBtn, "Two Players", buttonFontSize, &twoPlayerHover);
    HandleButtonHover(exitBtn, "Exit", buttonFontSize, &exitHover);

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

    // Check hover states
    bool easyHover = false;
    bool mediumHover = false;
    bool hardHover = false;
    bool backHover = false;

    // Draw buttons with hover effects
    HandleButtonHover(easyBtn, "Easy", buttonFontSize, &easyHover);
    HandleButtonHover(mediumBtn, "Medium", buttonFontSize, &mediumHover);
    HandleButtonHover(hardBtn, "Hard", buttonFontSize, &hardHover);
    HandleButtonHover(backBtn, "Back", buttonFontSize, &backHover);

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

    // Check hover states
    bool nbHover = false;
    bool dtHover = false;
    bool backHover = false;

    // Draw buttons with hover effects
    HandleButtonHover(nbBtn, "Naive Bayes", 20, &nbHover);
    HandleButtonHover(dtBtn, "Decision Tree", 20, &dtHover);
    HandleButtonHover(backBtn, "Back", 20, &backHover);
    
    // Set cursor based on any button hover
    SetMouseCursor((nbHover || dtHover || backHover) ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}

// Game Functions
// initialize the game
void InitGame() {
    // resets hintCount when retry
    hint.hintCountO = 0;
    hint.hintCountX = 0;
    showPartyAnimation = false; // Reset party animation

    // Stop all sounds that might be playing
    StopSound(victorySound);
    StopSound(loseSound); 
    StopSound(drawSound);

    // Initialize the grid to EMPTY in a single loop
    memset(grid, EMPTY, sizeof(grid));
    gameOver = false;
    winner = EMPTY;
    
    // Randomize starting player for both single and two player modes
    RandomizeStartingPlayer();
    
    
    // Reset winning cells
    for (int i = 0; i < 3; i++) {
        winningCells[i][0] = -1;
        winningCells[i][1] = -1;
    }
}

// function to point to the current game mode stats
ModeStats* GetCurrentModeStats() {
    if (currentDifficulty == EASY) {
        return (currentModel == NAIVE_BAYES) ? &naiveBayesStats : &decisionTreeStats;
    }
    return (currentDifficulty == MEDIUM) ? &mediumStats : &hardStats;
}

// Update the game
void UpdateGame(Sound buttonClickSound, Sound popSound, Sound victorySound, Sound loseSound, Sound drawSound, NaiveBayesModel *model, DecisionTreeNode *TDmodel)
{
    if (gameOver) return;
    
    // Update quit button position check
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.x >= 20 && mousePos.x <= 90 &&
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
                        AITurnDecisionTree(victorySound, loseSound, drawSound, TDmodel);  // Decision Tree
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
        
        // when hint button is clicked, get best move for the player and update hintCount. If hintCount is 2, button doesnt work
        if (mousePos.x >= SCREEN_WIDTH - 80 && mousePos.x <= SCREEN_WIDTH - 10 &&
            mousePos.y >= 10 && mousePos.y <= 40 && (hint.hintCountX < 2 || hint.hintCountO < 2))
        {
            // Check player turn and update the hint count when hint button is clicked
            if (currentPlayerTurn == PLAYER_X_TURN && hint.hintCountX < 2)
            {
                PlaySound(buttonClickSound);
                hint.hintCountX+=1;
                getHint();
                row = hint.row;
                col = hint.col;
            } else if (currentPlayerTurn == PLAYER_O_TURN && hint.hintCountO < 2)
            {
                PlaySound(buttonClickSound);
                hint.hintCountO+=1;
                getHint();
                row = hint.row;
                col = hint.col;
            } else {
                return false; // No move was made
            }
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
                    if (isTwoPlayer) {
                        showPartyAnimation = true; // Show confetti for any winner in two-player mode
                        InitConfetti(); // trigger confetti animation
                        PlaySound(victorySound); // Play victory sound for any winner in two-player mode
                    }
                    else if (!isTwoPlayer && winner == PLAYER_X) {
                        showPartyAnimation = true;  // Show party animation only when human player wins
                        InitConfetti(); // trigger confetti animation
                        currentStats->playerWins++; // Increment player wins
                        currentStats->totalGames++; // Increment total games
                        PlaySound(victorySound); // Play victory sound for Player X
                    } 
                    else {
                        showPartyAnimation = false; // No confetti for AI wins
                        currentStats->aiWins++; // Increment AI wins
                        currentStats->totalGames++; // Increment total games
                        PlaySound(loseSound); // Play lose sound for Player O
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
        predict_move(model, grid, &bestRow, &bestCol);
    }

    // Medium mode: use Minimax with limited depth search of 4
    else if (currentDifficulty == MEDIUM) {
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
void AITurnDecisionTree(Sound victorySound, Sound loseSound, Sound drawSound, DecisionTreeNode *TDmodel) {
    int bestScore = -1000;       // Initialize best score for evaluating moves
    int bestRow = -1;            // Initialize best row for AI move
    int bestCol = -1;            // Initialize best column for AI move
    int row, col;                // Variables for random fallback move
    double best_prob = 0.0;      // Probability of the best move
    char board[3][3];            // Buffer array for board layout

    // Convert the current grid state into a compatible format for the decision tree
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i][j] == EMPTY) {
                board[i][j] = 'b';  // Convert EMPTY cells to 'b' (blank)
            } else if (grid[i][j] == PLAYER_X) {
                board[i][j] = 'x';  // Convert PLAYER_X cells to 'x'
            } else if (grid[i][j] == PLAYER_O) {
                board[i][j] = 'o';  // Convert PLAYER_O cells to 'o'
            }
        }
    }

    // Print the decision tree structure for debugging
    print_tree(TDmodel, 2);

    // Use the decision tree to predict the best move for the AI
    dt_predict_best_move(TDmodel, board, PLAYER_O, &bestRow, &bestCol);

    // Fallback logic: Choose a random empty cell if the decision tree fails
    do {
        row = GetRandomValue(0, GRID_SIZE - 1);  // Generate random row index
        col = GetRandomValue(0, GRID_SIZE - 1);  // Generate random column index
    } while (grid[row][col] != EMPTY);           // Ensure the chosen cell is empty

    // Place the AI's move on the grid at the predicted or random position
    grid[bestRow][bestCol] = PLAYER_O;

    // Get the current stats for Easy mode
    ModeStats* currentStats = &decisionTreeStats;

    // Check if the AI's move results in a win
    if (CheckWin(PLAYER_O)) {
        gameOver = true;               // Mark the game as over
        winner = PLAYER_O;             // Set the winner to PLAYER_O
        gameState = GAME_OVER;         // Transition to GAME_OVER state
        currentStats->aiWins++;        // Increment AI win count
        currentStats->totalGames++;    // Increment total games count

        PlaySound(loseSound);          // Play losing sound for the player
    }
    // Check if the game results in a draw
    else if (CheckDraw()) {
        gameOver = true;               // Mark the game as over
        gameState = GAME_OVER;         // Transition to GAME_OVER state
        winner = EMPTY;                // Set the winner to NONE (draw)
        currentStats->draws++;         // Increment draw count
        currentStats->totalGames++;    // Increment total games count

        PlaySound(drawSound);          // Play draw sound
    }
    // If the game continues, pass the turn to the player
    else {
        currentPlayerTurn = PLAYER_X_TURN; // Set the turn to PLAYER_X
    }
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


void RandomizeStartingPlayer() { //Randomize starting player
    // 50% chance for each player to start
    if (GetRandomValue(0, 1) == 0) {
        currentPlayerTurn = PLAYER_X_TURN;  // Human starts
    } else {
        currentPlayerTurn = PLAYER_O_TURN;  // AI starts
    }
}

// gcc -o main main.c DecisionTree_ML/decisiontree.c NBmodel/data_processing.c NBmodel/NBmodel.c -I. -L. -lraylib -lopengl32 -lgdi32 -lwinmm
// ./main
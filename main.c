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



bool HandleButtonHover(Rectangle button, const char* text, int fontSize, bool* isHovered) {
    Vector2 mousePos = GetMousePosition();
    *isHovered = CheckCollisionPointRec(mousePos, button);
    DrawButton(button, text, fontSize, *isHovered);
    return *isHovered;
}

// function to point to the current game mode stats
ModeStats* GetCurrentModeStats() {
    if (currentDifficulty == EASY) {
        return (currentModel == NAIVE_BAYES) ? &naiveBayesStats : &decisionTreeStats;
    }
    return (currentDifficulty == MEDIUM) ? &mediumStats : &hardStats;
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
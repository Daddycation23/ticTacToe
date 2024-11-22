#include "../main.h"

extern Confetti confetti[MAX_CONFETTI];
extern bool showPartyAnimation;
extern bool gameOver;
extern Cell grid[GRID_SIZE][GRID_SIZE];
extern int winningCells[3][2];
extern struct GetHint hint;
extern bool isTwoPlayer;
extern Cell winner;
extern ModeStats* currentStats;
extern PlayerTurn currentPlayerTurn;

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
#include "../main.h"

extern struct GetHint hint;
extern Cell grid[GRID_SIZE][GRID_SIZE];
extern int winningCells[3][2];
extern bool showPartyAnimation;
extern bool gameOver;
extern Cell winner;

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
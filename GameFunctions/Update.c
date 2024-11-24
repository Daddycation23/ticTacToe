#include "../main.h"

extern TitleWord titleWords[5];
extern FallingSymbol symbols[MAX_SYMBOLS];
extern Confetti confetti[MAX_CONFETTI];
extern bool showPartyAnimation;
extern bool gameOver;
extern bool allInactive;
extern int currentWord;
extern int currentModel;
extern Difficulty currentDifficulty;
extern PlayerTurn currentPlayerTurn;
extern bool isTwoPlayer;

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
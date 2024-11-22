#include "../main.h"

extern Cell grid[GRID_SIZE][GRID_SIZE];
extern struct GetHint hint;

// Handle the button hover
bool HandleButtonHover(Rectangle button, const char* text, int fontSize, bool* isHovered) {
    Vector2 mousePos = GetMousePosition();
    *isHovered = CheckCollisionPointRec(mousePos, button);
    DrawButton(button, text, fontSize, *isHovered);
    return *isHovered;
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
            // Get player turn and update the hint count when hint button is clicked
            if (currentPlayerTurn == PLAYER_X_TURN && hint.hintCountX < 2)
            {
                PlaySound(buttonClickSound);
                hint.hintCountX+=1; // increment 
                getHint(); // Get best move 
                row = hint.row; // assign best move to be picked
                col = hint.col; // assign best move to be picked
            } else if (currentPlayerTurn == PLAYER_O_TURN && hint.hintCountO < 2)
            {
                PlaySound(buttonClickSound);
                hint.hintCountO+=1; // increment
                getHint(); // Get best move
                row = hint.row; // assign best move to be picked
                col = hint.col; // assign best move to be picked
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

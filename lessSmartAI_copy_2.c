// Add this helper function outside of DrawAIAnalysis
void DrawDifficultySection(const char* difficulty, DifficultyStats stats, int* y, Color color, int padding, int textFontSize) {
    char buffer[100];
    
    // Draw difficulty title
    DrawText(difficulty, padding, *y, textFontSize + 4, color);
    *y += textFontSize + padding/2;

    // Draw stats
    snprintf(buffer, sizeof(buffer), "Wins: %d", stats.wins);
    DrawText(buffer, padding * 2, *y, textFontSize, BLACK);
    *y += textFontSize + padding/3;

    snprintf(buffer, sizeof(buffer), "Losses: %d", stats.losses);
    DrawText(buffer, padding * 2, *y, textFontSize, BLACK);
    *y += textFontSize + padding/3;

    snprintf(buffer, sizeof(buffer), "Draws: %d", stats.draws);
    DrawText(buffer, padding * 2, *y, textFontSize, BLACK);
    *y += textFontSize + padding/3;

    float winRate = stats.totalGames > 0 ? 
        (float)stats.wins/stats.totalGames * 100 : 0;
    snprintf(buffer, sizeof(buffer), "Win Rate: %.1f%%", winRate);
    DrawText(buffer, padding * 2, *y, textFontSize, BLACK);
    *y += textFontSize + padding;
}

void DrawAIAnalysis() {
    const int titleFontSize = 30;  // Reduced from 40
    const int textFontSize = 16;   // Reduced from 20
    const int padding = 15;        // Reduced from 20
    int currentY = 30;             // Reduced from 50
    
    // Title
    const char* title = "AI Performance Analysis";
    DrawText(title, 
        SCREEN_WIDTH/2 - MeasureText(title, titleFontSize)/2,
        currentY,
        titleFontSize,
        BLACK);
    currentY += titleFontSize + padding;

    // Draw stats for each difficulty
    DrawDifficultySection("Easy Mode", easyStats, &currentY, GREEN, padding, textFontSize);
    DrawDifficultySection("Medium Mode", mediumStats, &currentY, ORANGE, padding, textFontSize);
    DrawDifficultySection("Hard Mode", hardStats, &currentY, RED, padding, textFontSize);

    // Back button
    Rectangle backBtn = {
        SCREEN_WIDTH/2 - BUTTON_WIDTH/2,
        SCREEN_HEIGHT - BUTTON_HEIGHT - padding/2,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

    // Button hover effect
    Vector2 mousePos = GetMousePosition();
    bool isHovering = (mousePos.x >= backBtn.x && 
                      mousePos.x <= backBtn.x + backBtn.width &&
                      mousePos.y >= backBtn.y && 
                      mousePos.y <= backBtn.y + backBtn.height);

    // Draw button
    DrawRectangleRec(backBtn, isHovering ? GRAY : LIGHTGRAY);
    DrawRectangleLinesEx(backBtn, 2, BLACK);

    // Draw button text
    const char* backText = "Back to Menu";
    DrawText(backText,
        SCREEN_WIDTH/2 - MeasureText(backText, textFontSize)/2,
        backBtn.y + (BUTTON_HEIGHT - textFontSize)/2,
        textFontSize,
        BLACK);

    // Set cursor
    SetMouseCursor(isHovering ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
} 
#include "theme.h"
#include "raylib.h"
#include <stdio.h> // Include stdio.h for printf

// Define the available themes
Theme themes[] = {
    { RAYWHITE, BLACK, BLUE, RED, LIGHTGRAY },        // Classic theme
    { DARKGRAY, LIGHTGRAY, SKYBLUE, ORANGE, GRAY },   // Dark theme
};

int currentThemeIndex = 0; // Track the current theme index
bool isDropdownOpen = false;  // Track if the dropdown is open
float dropdownToggleCooldown = 0.0f;  // Cooldown timer for toggling dropdown

// Initialize themes if needed
void InitThemes() {
    currentThemeIndex = 0; // Default to the first theme
}

// Apply the selected theme by setting background color
void ApplyTheme(Theme theme) {
    ClearBackground(theme.backgroundColor);
}

// Function to draw the theme selection dropdown menu
void DrawThemeDropdown() {
    const int buttonWidth = 155;  
    const int buttonHeight = 30;
    const int dropdownHeight = 30;
    const int padding = 5;

    Rectangle mainButton = { GetScreenWidth() - buttonWidth - 10, 10, buttonWidth, buttonHeight };
    Vector2 mousePos = GetMousePosition();

    // Debugging information: print dropdown state and mouse position
    // printf("isDropdownOpen: %d\n", isDropdownOpen);
    // printf("Mouse Position: x=%f, y=%f\n", mousePos.x, mousePos.y);

    // Update the cooldown timer
    if (dropdownToggleCooldown > 0) {
        dropdownToggleCooldown -= GetFrameTime();
    }

    // Check if main button is clicked to toggle dropdown (with cooldown)
    if (dropdownToggleCooldown <= 0 && CheckCollisionPointRec(mousePos, mainButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isDropdownOpen = !isDropdownOpen;  // Toggle dropdown
        dropdownToggleCooldown = 0.2f;  // Set cooldown to 0.2 seconds to prevent rapid toggling
    }

    // Draw the main "Select Theme" button with centered text and thicker border
    DrawRectangleRec(mainButton, LIGHTGRAY);
    DrawRectangleLinesEx(mainButton, 2, BLACK);  // Thicker border
    int textWidth = MeasureText("Select Theme", 20);
    DrawText("Select Theme", mainButton.x + (buttonWidth - textWidth) / 2, mainButton.y + padding, 20, BLACK);  // Centered text

    // Draw the dropdown options if open
    if (isDropdownOpen) {
        for (int i = 0; i < 2; i++) {
            Rectangle optionButton = { mainButton.x, mainButton.y + (i + 1) * dropdownHeight, buttonWidth, dropdownHeight };
            bool isOptionHovered = CheckCollisionPointRec(mousePos, optionButton);

            // Draw each option with hover effect and thicker border
            DrawRectangleRec(optionButton, isOptionHovered ? GRAY : LIGHTGRAY);
            DrawRectangleLinesEx(optionButton, 2, DARKGRAY);
            
            // Display theme names
            const char *themeText = (i == 0) ? "Classic" : "Dark";
            int optionTextWidth = MeasureText(themeText, 20);
            DrawText(themeText, optionButton.x + (buttonWidth - optionTextWidth) / 2, optionButton.y + padding, 20, BLACK);  // Centered text

            // Apply theme if option is clicked
            if (isOptionHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                currentThemeIndex = i;
                printf("Selected theme index: %d\n", currentThemeIndex); 
                isDropdownOpen = false;  // Close the dropdown after selection
            }
        }
    }
}

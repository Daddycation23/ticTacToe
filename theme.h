#ifndef THEME_H
#define THEME_H

#include "raylib.h"

// Define the structure for a Theme
typedef struct Theme {
    Color backgroundColor;
    Color primaryColor;  // Add this line
    Color xColor;
    Color oColor;
    Color gridColor;
} Theme;

// Declare the themes array and the current theme index as extern, so they can be accessed in other files
extern Theme themes[];
extern int currentThemeIndex;

// Declare the functions
void InitThemes();               // Initialize the themes
void ApplyTheme(Theme theme);    // Apply the selected theme
void DrawThemeDropdown();        // Draw the theme selection dropdown menu

#endif // THEME_H

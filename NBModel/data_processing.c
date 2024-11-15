#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"

// Function to load data from file into arrays
void load_data(const char *filename, char boards[][NUM_POSITIONS + 1], int outcomes[], int *total_records) {
    FILE *file_ptr = fopen(filename, "r");      // Open file of dataset to read
    if (file_ptr == NULL) {                     // Check if its an exisiting file, else will send an error
        perror("Failed to open file");
        exit(1);
    }

    char line[50];         // Array buffer for each line in dataset
    // This loop gets each line and store the individual attributes and the respective outcome in two different arrays, board & outcome
    while (fgets(line, sizeof(line), file_ptr)) {
        char board[NUM_POSITIONS + 1];                  // Array buffer for the 9 attributes per line
        char outcome[10];                               // Array buffer for outcome
        sscanf(line, "%c,%c,%c,%c,%c,%c,%c,%c,%c,%s",   // Splitting each line into the 9 different attributes and the outcome
               &board[0], &board[1], &board[2], &board[3],
               &board[4], &board[5], &board[6], &board[7],
               &board[8], outcome);
        strcpy(boards[*total_records], board);                  // Store attributes into board array
        outcomes[*total_records] = outcome_index(outcome);      // Store respectively outcome in outcome array
        (*total_records)++;     // count of total number of lines in dataset
    }
    fclose(file_ptr);       // Close file
}

// Function to split dataset into training and testing datasets
void split_data(char boards[][NUM_POSITIONS + 1], int outcomes[], int total_records, char train_boards[][NUM_POSITIONS + 1], int train_outcomes[], char test_boards[][NUM_POSITIONS + 1], int test_outcomes[], int *train_size, int *test_size, float ratio) {
    // Shuffle the dataset using Fisher-Yates algorithm 
    srand(time(NULL));
    for (int i = total_records-1; i > 0; i--) { 
        int j = rand() % (i + 1);   // Get unique randome number
 
        // Swap boards[i] and boards[j] 
        char temp_board[10]; 
        strcpy(temp_board, boards[i]);
        strcpy(boards[i], boards[j]); 
        strcpy(boards[j], temp_board);

        // Swap outcomes[i] and outcomes[j]
        char temp_outcome;
        temp_outcome = outcomes[i];
        outcomes[i] = outcomes[j];
        outcomes[j] = temp_outcome;
    }
    
    int target_train_size = (int)(ratio * total_records);       // Get number of lines for training dataaset, in this case 80% of total_records


    // Loop to seperate original dataset into training(80%) and testing(20%) dataset for machine learning
    for (int i = 0; i < total_records; i++) {
        if (*train_size < target_train_size) { // 80%
            strcpy(train_boards[*train_size], boards[i]);
            train_outcomes[*train_size] = outcomes[i];
            (*train_size)++;
        } else { // 20%
            strcpy(test_boards[*test_size], boards[i]);
            test_outcomes[*test_size] = outcomes[i];
            (*test_size)++;
        }
    }
}

// Utility function to convert the string outcome ("positive" or "negative") into the corresponding numerical label (POSITIVE(0) or NEGATIVE(1) ).
int outcome_index(const char *outcome) {
    return (strcmp(outcome, "positive") == 0) ? POSITIVE : NEGATIVE;
}
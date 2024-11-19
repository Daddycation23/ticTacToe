#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

// Function to train Naive Bayes model
void train_NBmodel(NaiveBayesModel *model, char boards[][NUM_POSITIONS + 1], int outcomes[], int size) {
    int positive_count = 0, negative_count = 0;            // Count for the occurrences of the two different outcomes "positive" and "negative" in the dataset

    // Initialize counts
    int x_counts[NUM_POSITIONS][NUM_OUTCOMES] = {0};        // Array for number of occurrences of 'x' in every position for both "positive" and "negative" outcome
    int o_counts[NUM_POSITIONS][NUM_OUTCOMES] = {0};        // Array for number of occurrences of 'o' in every position for both "positive" and "negative" outcome
    int b_counts[NUM_POSITIONS][NUM_OUTCOMES] = {0};        // Array for number of occurrences of 'b' in every position for both "positive" and "negative" outcome

    // Count occurences
    for (int i = 0; i < size; i++) {
        int outcome_idx = outcomes[i];
        if (outcome_idx == POSITIVE) positive_count++;      // Counting of number of times its a 'positive' or 'negative'
        else negative_count++;                              // outcome in the dataset

        // Loop to count how often 'x', 'o' and blank('b') appear in each position for each outcome
        for (int j = 0; j < NUM_POSITIONS; j++) {                       
            if (boards[i][j] == 'x') x_counts[j][outcome_idx]++;        // Count for 'x'
            else if (boards[i][j] == 'o') o_counts[j][outcome_idx]++;   // Count for 'o'
            else b_counts[j][outcome_idx]++;                            // Count for 'b'
        }
    }

    // Calculate prior probabilities
    model->class_probs[POSITIVE] = (double)positive_count / size;       // Probability of outcome being "positive" in the dataset
    model->class_probs[NEGATIVE] = (double)negative_count / size;       // Probability of outcome being "negative" in the dataset

    // Calculate conditional probabilities with Laplace smoothing
    for (int i = 0; i < NUM_POSITIONS; i++) {                                                       // Loop through each of the 9 position on the board
        model->x_probs[i][POSITIVE] = (double)(x_counts[i][POSITIVE] + 1) / (positive_count + 3);   // Calculate the probability of 'x' appearing in each position for all "positive" outcomes
        model->x_probs[i][NEGATIVE] = (double)(x_counts[i][NEGATIVE] + 1) / (negative_count + 3);   // Calculate the probability of 'x' appearing in each position for all "negative" outcomes

        model->o_probs[i][POSITIVE] = (double)(o_counts[i][POSITIVE] + 1) / (positive_count + 3);   // Calculate the probability of 'o' appearing in each position for all "positive" outcomes
        model->o_probs[i][NEGATIVE] = (double)(o_counts[i][NEGATIVE] + 1) / (negative_count + 3);   // Calculate the probability of 'o' appearing in each position for all "negative" outcomes
        
        model->b_probs[i][POSITIVE] = (double)(b_counts[i][POSITIVE] + 1) / (positive_count + 3);   // Calculate the probability of 'b' appearing in each position for all "positive" outcomes
        model->b_probs[i][NEGATIVE] = (double)(b_counts[i][NEGATIVE] + 1) / (negative_count + 3);   // Calculate the probability of 'b' appearing in each position for all "negative" outcomes
    }
}

// Function to save model weights
void save_NBmodel(const NaiveBayesModel *model, const char *filename) {
    FILE *file_ptr = fopen(filename, "w");      // Open text file to write
    if (file_ptr == NULL) {                     // Check if can open or create file, else will send an error
        perror("Failed to open file for saving model");
        return;
    }

    // Save the prior probabilites
    fprintf(file_ptr, "Class Probabilities:\n");
    fprintf(file_ptr, "P(Positive): %f\n", model->class_probs[POSITIVE]);
    fprintf(file_ptr, "P(Negative): %f\n\n", model->class_probs[NEGATIVE]);

    // Loop to save each conditional probabilites of each feature ['x', 'o' ,'b'] for each outcome
    for (int i = 0; i < NUM_POSITIONS; i++) {
        fprintf(file_ptr, "Position %d:\n", i + 1);
        fprintf(file_ptr, "P(x | Positive): %f\n", model->x_probs[i][POSITIVE]);
        fprintf(file_ptr, "P(x | Negative): %f\n", model->x_probs[i][NEGATIVE]);
        fprintf(file_ptr, "P(o | Positive): %f\n", model->o_probs[i][POSITIVE]);
        fprintf(file_ptr, "P(o | Negative): %f\n", model->o_probs[i][NEGATIVE]);
        fprintf(file_ptr, "P(b | Positive): %f\n", model->b_probs[i][POSITIVE]);
        fprintf(file_ptr, "P(b | Negative): %f\n\n", model->b_probs[i][NEGATIVE]);
    }

    fclose(file_ptr);       // Close file
    printf("Model weights saved to %s\n", filename);
}

// Function to test accuracy of model
void test_NBmodel(const char *filename, char mode[], char type[], NaiveBayesModel *model, char boards[][NUM_POSITIONS + 1], int outcomes[], int size) {
    int true_positive = 0;      // Count of true positives
    int false_positive = 0;     // Count of false positives
    int true_negative = 0;      // Count of true negatives
    int false_negative = 0;     // Count of false negatives
    int error_count = 0;        // Count of prediction errors

    // Loop to count for each of the 4 classes (TP, FP, TN, FN)
    for (int i = 0; i < size; i++) {
        int predicted_outcome = predict_outcome(model, boards[i]);
        if (outcomes[i] == POSITIVE && predicted_outcome == POSITIVE ) {
            true_positive++;
        }
        else if (outcomes[i] == POSITIVE && predicted_outcome == NEGATIVE ) {
            false_negative++;
            error_count++;
        }
        else if (outcomes[i] == NEGATIVE && predicted_outcome == NEGATIVE ) {
            true_negative++;
        }
        else {
            false_positive++;
            error_count++;
        };
    }

    // Calculate probability of error
    double prob_of_error = (double)error_count / size * 100;

    FILE *file_ptr = fopen(filename, mode);     // Open text file to write
    if (file_ptr == NULL) {                     // Check if can open or create file, else will send an error
        perror("Failed to open file");
        exit(1);
    }
    
    // Information to write in text file
    if (strcmp(type, "Testing") == 0) fprintf(file_ptr, "\n\n");        // If writing for Testing dataset, indent two newlines for easier readability

    fprintf(file_ptr, "%s Dataset:\n", type);                                                               // Type of dataset (Training/Testing)
    fprintf(file_ptr, "  Accuracy: %.2f%% (%d/%d)\n", 100 - prob_of_error, size - error_count, size);       // Prediction Accuracy of model on dataset
    fprintf(file_ptr, "  Error: %.2f%% (%d/%d)\n", prob_of_error, error_count, size);                       // Probability of error of model on dataset
    fprintf(file_ptr, "  Confusion Matrix:\n");                                                             // Confusion Matrix Values
    fprintf(file_ptr, "    True Positive: %d\n", true_positive);                                            // Number of True Positive predicted by model
    fprintf(file_ptr, "    False Positive: %d\n", false_positive);                                          // Number of False Positive predicted by model
    fprintf(file_ptr, "    True Negative: %d\n", true_negative);                                            // Number of True Negative predicted by model
    fprintf(file_ptr, "    False Negative: %d\n", false_negative);                                          // Number of False Negative predicted by model

    fclose(file_ptr);       // Close file
}

// Function to calculate the posterior probability of a specified outcome based on the given board layout.
double calculate_probability(NaiveBayesModel *model, const char board[], int outcome) {
    double probability = model->class_probs[outcome];       // P(C); Get prior probability of given/set outcome, "positive" or "negative"

    // Loop to iterate over each position in the board array to update probability by multiplying with the conditional probability of the feature in that position
    for (int i = 0; i < NUM_POSITIONS; i++) {
        if (board[i] == 'x') probability *= model->x_probs[i][outcome];         // P(X | C) for 'x'
        else if (board[i] == 'o') probability *= model->o_probs[i][outcome];    // P(X | C) for 'o'
        else probability *= model->b_probs[i][outcome];                         // P(X | C) for blank 'b'
    }
    return probability;
}

// Function to predict outcome of given board layout; either "positive" or "negative"
int predict_outcome(NaiveBayesModel *model, const char board[]) {
    double positive_prob = calculate_probability(model, board, POSITIVE);       // Calculate posterior probability for "positive" outcome
    double negative_prob = calculate_probability(model, board, NEGATIVE);       // Calculate posterior probability for "negative" outcome
    return (positive_prob > negative_prob) ? POSITIVE : NEGATIVE;               // Compare both posterior probability and return the outcome with the highest one
}

// Function to find next best move for the NBmodel based on the given board layout
int predict_move(NaiveBayesModel *model, Cell grid[GRID_SIZE][GRID_SIZE], int *bestRow, int *bestCol) {
    int best_move = -1;             // Index of the best move position
    double best_prob = 0.0;         // Probability of best move
    char board[NUM_POSITIONS + 1];  // Buffer array for board layout
    int k = 0;                      // Index of buffer array

    // Loop to translate the board layout in the GUI into an array for the model to read
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == EMPTY) {          // If position is empty, set respective position/index in buffer array to blank 'b'
                board[k] = 'b';
            }
            else if (grid[i][j] == PLAYER_O){   // If position has 'o', set respective position/index in buffer array to 'o'
                board[k] = 'o';
            }
            else {
                board[k] = 'x';                 // If position has 'x', set respective position/index in buffer array to 'x'
            }
            k++;
        }
    }

    // Loop over the board layout to compare which available position gives higher probability of winning
    for (int i = 0; i < NUM_POSITIONS; i++) {
        if (board[i] == 'b') {      // Check if position is available
            char temp_board[NUM_POSITIONS + 1];     // Buffer array to simulate a move on current board layout
            strcpy(temp_board, board);
            temp_board[i] = 'x'; // Assume AI is 'x' as 'x' is the winning perspective

            double positive_prob = calculate_probability(model, temp_board, POSITIVE);      // Calculate posterior probability of a "positive" outcome with the temporary board layout based on the simulated move
            
            // Check if latest calculated posterior probability is the highest so far, if yes, replace best_prob with tha, and best_move with the simulated move
            if (positive_prob > best_prob) {
                best_prob = positive_prob;
                best_move = i;
            }
        }
    }

    // Use divide function to translate best_move into the (x,y) position of the board layout for the model to place move on GUI.
    // For example, if best_move is 2, it will be translated as (0,2), meaning first row & third column on board
    divide(best_move, 3, bestRow, bestCol);

    return 0;
}

// Function to get quotient and remainder of an integer
void divide(int dividend, int divisor, int *quo, int *rem){
    *quo = dividend / divisor;      // Compute quotient
    *rem = dividend % divisor;      // Compute remainder
}

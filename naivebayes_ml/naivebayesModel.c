#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "naivebayes.h"

void train_model(NaiveBayesModel *model, char boards[][NUM_POSITIONS + 1], int outcomes[], int size) {
    int positive_count = 0, negative_count = 0;
    int x_counts[NUM_POSITIONS][NUM_OUTCOMES] = {0};
    int o_counts[NUM_POSITIONS][NUM_OUTCOMES] = {0};
    int b_counts[NUM_POSITIONS][NUM_OUTCOMES] = {0};

    for (int i = 0; i < size; i++) {
        int outcome_idx = outcomes[i];
        if (outcome_idx == POSITIVE) positive_count++;
        else negative_count++;

        for (int j = 0; j < NUM_POSITIONS; j++) {
            if (boards[i][j] == 'x') x_counts[j][outcome_idx]++;
            else if (boards[i][j] == 'o') o_counts[j][outcome_idx]++;
            else b_counts[j][outcome_idx]++;
        }
    }

    model->class_probs[POSITIVE] = (double)positive_count / size;
    model->class_probs[NEGATIVE] = (double)negative_count / size;

    for (int i = 0; i < NUM_POSITIONS; i++) {
        model->x_probs[i][POSITIVE] = (double)x_counts[i][POSITIVE] / positive_count;
        model->x_probs[i][NEGATIVE] = (double)x_counts[i][NEGATIVE] / negative_count;
        model->o_probs[i][POSITIVE] = (double)o_counts[i][POSITIVE] / positive_count;
        model->o_probs[i][NEGATIVE] = (double)o_counts[i][NEGATIVE] / negative_count;
        model->b_probs[i][POSITIVE] = (double)b_counts[i][POSITIVE] / positive_count;
        model->b_probs[i][NEGATIVE] = (double)b_counts[i][NEGATIVE] / negative_count;
    }
}

void save_model(const NaiveBayesModel *model, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for saving model");
        return;
    }

    fprintf(file, "Class Probabilities:\n");
    fprintf(file, "P(Positive): %f\n", model->class_probs[POSITIVE]);
    fprintf(file, "P(Negative): %f\n\n", model->class_probs[NEGATIVE]);

    for (int i = 0; i < NUM_POSITIONS; i++) {
        fprintf(file, "Position %d:\n", i + 1);
        fprintf(file, "P(x | Positive): %f\n", model->x_probs[i][POSITIVE]);
        fprintf(file, "P(x | Negative): %f\n", model->x_probs[i][NEGATIVE]);
        fprintf(file, "P(o | Positive): %f\n", model->o_probs[i][POSITIVE]);
        fprintf(file, "P(o | Negative): %f\n", model->o_probs[i][NEGATIVE]);
        fprintf(file, "P(b | Positive): %f\n", model->b_probs[i][POSITIVE]);
        fprintf(file, "P(b | Negative): %f\n\n", model->b_probs[i][NEGATIVE]);
    }

    fclose(file);
    printf("Model weights saved to %s\n", filename);
}

void test_model(NaiveBayesModel *model, char boards[][NUM_POSITIONS + 1], int outcomes[], int size) {
    int correct_predictions = 0;

    for (int i = 0; i < size; i++) {
        int predicted_outcome = predict_outcome(model, boards[i]);
        if (predicted_outcome == outcomes[i]) {
            correct_predictions++;
        }
    }

    double accuracy = (double)correct_predictions / size * 100.0;
    printf("Test Accuracy: %.2f%% (%d/%d correct predictions)\n", accuracy, correct_predictions, size);
}

double calculate_probability(NaiveBayesModel *model, const char board[], int outcome) {
    double probability = model->class_probs[outcome];
    for (int i = 0; i < NUM_POSITIONS; i++) {
        if (board[i] == 'x') probability *= model->x_probs[i][outcome];
        else if (board[i] == 'o') probability *= model->o_probs[i][outcome];
        else probability *= model->b_probs[i][outcome];
    }
    return probability;
}

int predict_outcome(NaiveBayesModel *model, const char board[]) {
    double positive_prob = calculate_probability(model, board, POSITIVE);
    double negative_prob = calculate_probability(model, board, NEGATIVE);
    return (positive_prob > negative_prob) ? POSITIVE : NEGATIVE;
}

int predict_move(NaiveBayesModel *model, const char board[]) {
    int best_move = -1;
    double best_prob = -1.0;

    for (int i = 0; i < NUM_POSITIONS; i++) {
        if (board[i] == 'b') {
            char temp_board[NUM_POSITIONS + 1];
            strcpy(temp_board, board);
            temp_board[i] = 'o'; // Assume AI is 'o'

            double positive_prob = calculate_probability(model, temp_board, POSITIVE);
            if (positive_prob > best_prob) {
                best_prob = positive_prob;
                best_move = i;
            }
        }
    }
    return best_move;
}
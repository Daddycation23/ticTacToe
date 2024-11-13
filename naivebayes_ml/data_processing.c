#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "naivebayes.h"

void load_data(const char *filename, char boards[][NUM_POSITIONS + 1], int outcomes[], int *total_records) {
    FILE *file_ptr = fopen(filename, "r");
    if (file_ptr == NULL) {
        perror("Failed to open file");
        exit(1);
    }

    char line[50];
    while (fgets(line, sizeof(line), file_ptr)) {
        char board[NUM_POSITIONS + 1];
        char outcome[10];
        sscanf(line, "%c,%c,%c,%c,%c,%c,%c,%c,%c,%s",
               &board[0], &board[1], &board[2], &board[3],
               &board[4], &board[5], &board[6], &board[7],
               &board[8], outcome);
        strcpy(boards[*total_records], board);
        outcomes[*total_records] = outcome_index(outcome);
        (*total_records)++;
    }
    fclose(file_ptr);
}

void split_data(char boards[][NUM_POSITIONS + 1], int outcomes[], int total_records, char train_boards[][NUM_POSITIONS + 1], int train_outcomes[], char test_boards[][NUM_POSITIONS + 1], int test_outcomes[], int *train_size, int *test_size, float ratio) {
    // Shuffle the dataset using Fisher-Yates algorithm 
    srand(time(NULL));
    for (int i = total_records-1; i > 0; i--) { 
        int j = rand() % (i + 1); 
 
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
    
    int target_train_size = (int)(ratio * total_records);

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

int outcome_index(const char *outcome) {
    return (strcmp(outcome, "positive") == 0) ? POSITIVE : NEGATIVE;
}
#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "main.h"

#define MAX_ROWS 958
#define NUM_FEATURES 9
#define POSITIVE 1
#define NEGATIVE 0
#define MAX_DEPTH 5 // Further reduce tree depth for fairness
#define RANDOMNESS_FACTOR 0.3 // Add randomness to predictions

typedef struct {
    int features[NUM_FEATURES];
    int label;
} DataRow;

// Function prototypes
void load_dataset(const char *filename, DataRow dataset[], int *dataset_size);
void decision_tree_split_dataset(DataRow dataset[], int dataset_size, DataRow train_set[], int *train_size, DataRow test_set[], int *test_size, float train_ratio);
void shuffle_dataset(DataRow dataset[], int size);
DecisionTreeNode *build_tree(DataRow dataset[], int size, int depth);
float calculate_gini_index(DataRow dataset[], int size, int feature_index, int threshold);
void decision_tree_split_data(DataRow dataset[], int size, int feature_index, int threshold, DataRow left[], int *left_size, DataRow right[], int *right_size);
int predict_with_randomness(DecisionTreeNode *node, int features[]);
float evaluate_with_randomness(DecisionTreeNode *root, DataRow dataset[], int size, int confusion_matrix[2][2]);
void display_confusion_matrix(int confusion_matrix[2][2], const char *filename, const char *dataset_type) ;
void free_tree(DecisionTreeNode *node);
void calculate_position_probabilities(DataRow dataset[], int dataset_size, const char *filename); // Added prototype
void dt_predict_best_move(DecisionTreeNode *tree, char board[3][3], char current_player, int *best_row, int *best_col);
void growth_Tree(DecisionTreeNode *TDmodel);
void print_tree(DecisionTreeNode *node, int depth) ;
#endif // DECISIONTREE_H

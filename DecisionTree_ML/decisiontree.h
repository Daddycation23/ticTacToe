#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "main.h"

// Define constants for the Decision Tree
#define MAX_ROWS 958          // Maximum number of rows in the dataset
#define NUM_FEATURES 9        // Number of features in the dataset
#define DT_POSITIVE 1         // Label for positive class
#define DT_NEGATIVE 0         // Label for negative class
#define MAX_DEPTH 5           // Maximum depth of the decision tree for fairness
#define RANDOMNESS_FACTOR 0.3 // Add randomness to predictions for simulation purposes

// Define the structure for a dataset row
typedef struct {
    int features[NUM_FEATURES]; // Array to store feature values for a data point
    int label;                  // Label for the data point (positive or negative)
} DataRow;

// Function prototypes for the Decision Tree
// Load the dataset from a file
void load_dataset(const char *filename, DataRow dataset[], int *dataset_size);
// Split the dataset into training and testing sets based on a train-test ratio
void decision_tree_split_dataset(DataRow dataset[], int dataset_size, DataRow train_set[], int *train_size, DataRow test_set[], int *test_size, float train_ratio);
// Shuffle the dataset for randomized data order
void shuffle_dataset(DataRow dataset[], int size);
// Build the decision tree recursively with a specified depth limit
DecisionTreeNode *build_tree(DataRow dataset[], int size, int depth);
// Calculate the Gini index for a potential split on a feature
float calculate_gini_index(DataRow dataset[], int size, int feature_index, int threshold);
// Split the dataset into left and right branches based on a feature and threshold
void decision_tree_split_data(DataRow dataset[], int size, int feature_index, int threshold, DataRow left[], int *left_size, DataRow right[], int *right_size);
// Make a prediction with added randomness in the decision tree
int predict_with_randomness(DecisionTreeNode *node, int features[]);
// Evaluate the decision tree on a dataset and update the confusion matrix
float evaluate_with_randomness(DecisionTreeNode *root, DataRow dataset[], int size, int confusion_matrix[2][2]);
// Display the confusion matrix and save it to a file
void display_confusion_matrix(int confusion_matrix[2][2], const char *filename, const char *dataset_type);
// Free the memory allocated for the decision tree
void free_tree(DecisionTreeNode *node);
// Calculate and save position probabilities for the dataset
void calculate_position_probabilities(DataRow dataset[], int dataset_size, const char *filename);
// Predict the best move in a Tic-Tac-Toe game using the decision tree
void dt_predict_best_move(DecisionTreeNode *tree, char board[3][3], char current_player, int *best_row, int *best_col);
// Grow the decision tree by training it on a dataset
void growth_Tree(DecisionTreeNode *TDmodel);
// Print the decision tree structure for debugging
void print_tree(DecisionTreeNode *node, int depth);
// Calculate the error rate of the decision tree on a dataset
double calculate_error_rate(DecisionTreeNode *root, DataRow dataset[], int size, int confusion_matrix[2][2]);
// Write the accuracy results to a file
void write_accuracy_to_file(const char *filename, const char *dataset_type, float accuracy, int correct, int total);

#endif // DECISIONTREE_H

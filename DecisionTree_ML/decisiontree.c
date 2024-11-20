#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "decisiontree.h"


void growth_Tree(DecisionTreeNode *tree) {
    DataRow dataset[MAX_ROWS];
    DataRow train_set[MAX_ROWS], test_set[MAX_ROWS];
    int dataset_size = 0, train_size = 0, test_size = 0;
    int train_confusion[2][2] = {0}, test_confusion[2][2] = {0};
    float train_accuracy = 0.0, test_accuracy = 0.0;
    double train_error_rate = 0.0, test_error_rate = 0.0; // Added variables for error rate
    int correct_train = 0, correct_test = 0;

    srand(time(NULL)); // Initialize random seed

    // Load dataset
    load_dataset("tic-tac-toe.data", dataset, &dataset_size);

    // Shuffle dataset
    shuffle_dataset(dataset, dataset_size);

    // Split dataset (80% training, 20% testing)
    decision_tree_split_dataset(dataset, dataset_size, train_set, &train_size, test_set, &test_size, 0.8);

    // Build decision tree
    tree = build_tree(train_set, train_size, 0);

    // Write weights to file
    calculate_position_probabilities(dataset, dataset_size, "DecisionTree_ML/DTweights.txt");

    // Clear the file before writing results
    FILE *file = fopen("DecisionTree_ML/DTconfusion_matrix.txt", "w");
    if (file) fclose(file);

    // Evaluate training data
    train_accuracy = evaluate_with_randomness(tree, train_set, train_size, train_confusion);
    correct_train = (int)(train_accuracy * train_size);
    display_confusion_matrix(train_confusion, "DecisionTree_ML/DTconfusion_matrix.txt", "Training");
    printf("Training Accuracy: %.2f%% (%d/%d)\n", train_accuracy * 100, correct_train, train_size);
    write_accuracy_to_file("DecisionTree_ML/DTconfusion_matrix.txt", "Training", train_accuracy, correct_train, train_size);

    // Calculate training error rate
    train_error_rate = calculate_error_rate(tree, train_set, train_size, train_confusion);
    printf("Training Error Rate: %.2f%%\n", train_error_rate);
    file = fopen("DecisionTree_ML/DTconfusion_matrix.txt", "a");
    if (file) {
        fprintf(file, "Training Error Rate: %.2f%%\n", train_error_rate);
        fclose(file);
    }

    // Evaluate testing data
    test_accuracy = evaluate_with_randomness(tree, test_set, test_size, test_confusion);
    correct_test = (int)(test_accuracy * test_size);
    display_confusion_matrix(test_confusion, "DecisionTree_ML/DTconfusion_matrix.txt", "Testing");
    printf("Testing Accuracy: %.2f%% (%d/%d)\n", test_accuracy * 100, correct_test, test_size);
    write_accuracy_to_file("DecisionTree_ML/DTconfusion_matrix.txt", "Testing", test_accuracy, correct_test, test_size);

    // Calculate testing error rate
    test_error_rate = calculate_error_rate(tree, test_set, test_size, test_confusion);
    printf("Testing Error Rate: %.2f%%\n", test_error_rate);
    file = fopen("DecisionTree_ML/DTconfusion_matrix.txt", "a");
    if (file) {
        fprintf(file, "Testing Error Rate: %.2f%%\n", test_error_rate);
        fclose(file);
    }
}

// Load dataset from file
void load_dataset(const char *filename, DataRow dataset[], int *dataset_size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }

    char line[256];
    *dataset_size = 0;

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        for (int i = 0; i < NUM_FEATURES; i++) {
            if (strcmp(token, "x") == 0) dataset[*dataset_size].features[i] = 1;
            else if (strcmp(token, "o") == 0) dataset[*dataset_size].features[i] = 2;
            else dataset[*dataset_size].features[i] = 0;
            token = strtok(NULL, ",");
        }
        dataset[*dataset_size].label = (strcmp(token, "positive\n") == 0) ? DT_POSITIVE : DT_NEGATIVE;
        (*dataset_size)++;
    }

    fclose(file);
}

// Shuffle dataset
void shuffle_dataset(DataRow dataset[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        DataRow temp = dataset[i];
        dataset[i] = dataset[j];
        dataset[j] = temp;
    }
}

// Split dataset into training and testing sets
void decision_tree_split_dataset(DataRow dataset[], int dataset_size, DataRow train_set[], int *train_size, DataRow test_set[], int *test_size, float train_ratio) {
    int train_limit = (int)(dataset_size * train_ratio);
    *train_size = 0;
    *test_size = 0;
    for (int i = 0; i < dataset_size; i++) {
        if (i < train_limit) {
            train_set[(*train_size)++] = dataset[i];
        } else {
            test_set[(*test_size)++] = dataset[i];
        }
    }
}

// Build decision tree with depth limit
DecisionTreeNode *build_tree(DataRow dataset[], int size, int depth) {
    int positives = 0, negatives = 0;
    for (int i = 0; i < size; i++) {
        if (dataset[i].label == DT_POSITIVE) positives++;
        else negatives++;
    }

    // Stop condition: max depth reached or pure node
    if (depth >= MAX_DEPTH || positives == 0 || negatives == 0) {
        DecisionTreeNode *leaf = (DecisionTreeNode *)malloc(sizeof(DecisionTreeNode));
        leaf->is_leaf = 1;
        leaf->prediction = (positives > negatives) ? DT_POSITIVE : DT_NEGATIVE;
        leaf->left = leaf->right = NULL;
        return leaf;
    }

    int best_feature = -1, best_threshold = -1;
    float best_gini = 1.0;
    DataRow left[MAX_ROWS], right[MAX_ROWS];
    int left_size = 0, right_size = 0;

    for (int feature_index = 0; feature_index < NUM_FEATURES; feature_index++) {
        for (int threshold = 0; threshold <= 2; threshold++) {
            float gini = calculate_gini_index(dataset, size, feature_index, threshold);
            if (gini < best_gini) {
                best_gini = gini;
                best_feature = feature_index;
                best_threshold = threshold;
            }
        }
    }

    decision_tree_split_data(dataset, size, best_feature, best_threshold, left, &left_size, right, &right_size);

    DecisionTreeNode *node = (DecisionTreeNode *)malloc(sizeof(DecisionTreeNode));
    node->is_leaf = 0;
    node->feature_index = best_feature;
    node->threshold = best_threshold;
    node->left = build_tree(left, left_size, depth + 1);
    node->right = build_tree(right, right_size, depth + 1);

    return node;
}

// Evaluate the decision tree with randomness
float evaluate_with_randomness(DecisionTreeNode *root, DataRow dataset[], int size, int confusion_matrix[2][2]) {
    int correct_predictions = 0;

    // Reset confusion matrix
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            confusion_matrix[i][j] = 0;
        }
    }

    // Populate the confusion matrix
    for (int i = 0; i < size; i++) {
        int prediction = predict_with_randomness(root, dataset[i].features);
        int actual = dataset[i].label;

        if (actual == DT_POSITIVE && prediction == DT_POSITIVE) {
            confusion_matrix[0][0]++; // True Positive
            correct_predictions++;
        } else if (actual == DT_NEGATIVE && prediction == DT_NEGATIVE) {
            confusion_matrix[1][1]++; // True Negative
            correct_predictions++;
        } else if (actual == DT_NEGATIVE && prediction == DT_POSITIVE) {
            confusion_matrix[1][0]++; // False Positive
        } else if (actual == DT_POSITIVE && prediction == DT_NEGATIVE) {
            confusion_matrix[0][1]++; // False Negative
        }
    }

    return (float)correct_predictions / size;
}

// Predict with added randomness
int predict_with_randomness(DecisionTreeNode *node, int features[]) {
    if (!node) {
        return DT_NEGATIVE; // Default to a safe fallback prediction
    }

    if (node->is_leaf) {
        // Add randomness: flip the prediction with a certain probability
        if ((float)rand() / RAND_MAX < RANDOMNESS_FACTOR) {
            return (node->prediction == DT_POSITIVE) ? DT_NEGATIVE : DT_POSITIVE;
        }
        return node->prediction;
    }

    if (features[node->feature_index] <= node->threshold) {
        return predict_with_randomness(node->left, features);
    } else {
        return predict_with_randomness(node->right, features);
    }
}

// Display the confusion matrix
void display_confusion_matrix(int confusion_matrix[2][2], const char *filename, const char *dataset_type) {
    FILE *file = fopen(filename, "a"); // Open in append mode
    if (!file) {
        perror("Failed to open confusion matrix file");
        return;
    }

    // Extract TP, TN, FP, FN from the confusion matrix
    int TP = confusion_matrix[0][0];
    int FP = confusion_matrix[1][0];
    int TN = confusion_matrix[1][1];
    int FN = confusion_matrix[0][1];

    // Print to console
    printf("\nDecision Tree %s Confusion Matrix:\n", dataset_type);
    printf("    True Positive (TP): %d\n", TP);
    printf("    False Positive (FP): %d\n", FP);
    printf("    True Negative (TN): %d\n", TN);
    printf("    False Negative (FN): %d\n", FN);
    printf("\nConfusion Matrix:\n");
    printf("                Predicted Positive    Predicted Negative\n");
    printf("Actual Positive        %10d%20d\n", TP, FN);
    printf("Actual Negative        %10d%20d\n", FP, TN);
    printf("---------------------------------------------------------\n");

    // Write to file
    fprintf(file, "\nDecision Tree %s Confusion Matrix:\n", dataset_type);
    fprintf(file, "    True Positive (TP): %d\n", TP);
    fprintf(file, "    False Positive (FP): %d\n", FP);
    fprintf(file, "    True Negative (TN): %d\n", TN);
    fprintf(file, "    False Negative (FN): %d\n", FN);
    fprintf(file, "\nConfusion Matrix:\n");
    fprintf(file, "                Predicted Positive    Predicted Negative\n");
    fprintf(file, "Actual Positive        %10d%20d\n", TP, FN);
    fprintf(file, "Actual Negative        %10d%20d\n", FP, TN);
    fprintf(file, "---------------------------------------------------------\n");

    fclose(file); // Close the file properly
}

void write_accuracy_to_file(const char *filename, const char *dataset_type, float accuracy, int correct, int total) {
    FILE *file = fopen(filename, "a"); // Open in append mode
    if (!file) {
        perror("Failed to open file for writing accuracy");
        return;
    }

    // Write the accuracy with the number of correctly classified samples
    fprintf(file, "%s Accuracy: %.2f%% (%d/%d)\n", dataset_type, accuracy * 100, correct, total);
    fclose(file); // Close the file
}

// Free the memory allocated for the decision tree
void free_tree(DecisionTreeNode *node) {
    if (node == NULL) return;

    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

// Calculate Gini Index
float calculate_gini_index(DataRow dataset[], int size, int feature_index, int threshold) {
    DataRow left[MAX_ROWS], right[MAX_ROWS];
    int left_size = 0, right_size = 0;

    decision_tree_split_data(dataset, size, feature_index, threshold, left, &left_size, right, &right_size);

    if (left_size == 0 || right_size == 0) return 1.0;

    float gini_left = 1.0, gini_right = 1.0;
    int positives_left = 0, positives_right = 0;

    for (int i = 0; i < left_size; i++) {
        if (left[i].label == DT_POSITIVE) positives_left++;
    }
    for (int i = 0; i < right_size; i++) {
        if (right[i].label == DT_POSITIVE) positives_right++;
    }

    float prob_left = (float)positives_left / left_size;
    float prob_right = (float)positives_right / right_size;
    gini_left = 1.0 - (prob_left * prob_left) - ((1.0 - prob_left) * (1.0 - prob_left));
    gini_right = 1.0 - (prob_right * prob_right) - ((1.0 - prob_right) * (1.0 - prob_right));

    return ((gini_left * left_size) + (gini_right * right_size)) / size;
}

// Split dataset into left and right branches
void decision_tree_split_data(DataRow dataset[], int size, int feature_index, int threshold, DataRow left[], int *left_size, DataRow right[], int *right_size) {
    *left_size = 0;
    *right_size = 0;

    for (int i = 0; i < size; i++) {
        if (dataset[i].features[feature_index] <= threshold) {
            left[(*left_size)++] = dataset[i];
        } else {
            right[(*right_size)++] = dataset[i];
        }
    }
}
void dt_predict_best_move(DecisionTreeNode *tree, char board[3][3], char current_player, int *best_row, int *best_col) {
    if (!tree) {
        printf("Error: Decision tree is not initialized!\n");
        return;
    }
    int features[NUM_FEATURES]; 
    int max_positive_prob = -1; 
    *best_row = -1;             
    *best_col = -1;            
    int attempts = 0;          

    // Convert the board to features
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 'x') features[i * 3 + j] = 1;
            else if (board[i][j] == 'o') features[i * 3 + j] = 2;
            else features[i * 3 + j] = 0;
        }
    }

    // Try to find the best move within 5 attempts
    for (attempts = 0; attempts < 5; attempts++) {
        int temp_row = -1, temp_col = -1;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == 'b') { // If the cell is empty
                    // Temporarily place the current player's symbol
                    features[i * 3 + j] = (current_player == 'x') ? 1 : 2;

                    // Predict the outcome using the trained Decision Tree
                    int prediction = predict_with_randomness(tree, features);

                    // Check if this move is the best
                    if (prediction == DT_POSITIVE && (max_positive_prob == -1 || prediction > max_positive_prob)) {
                        temp_row = i;
                        temp_col = j;
                        max_positive_prob = prediction;
                    }

                    // Reset the cell back to empty
                    features[i * 3 + j] = 0;
                }
            }
        }

        if (temp_row != -1 && temp_col != -1) {
            *best_row = temp_row;
            *best_col = temp_col;
            return; // Exit once a positive move is found
        }
    }

    // If no positive move is found within 5 attempts, pick a random empty slot
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 'b') { // If the cell is empty
                *best_row = i;
                *best_col = j;
                return;
            }
        }
    }
}

void print_tree(DecisionTreeNode *node, int depth) {
    if (!node) {
        return;
    }

    if (node->is_leaf) {
        // printf("%*sLeaf: Prediction = %d\n", depth * 4, "", node->prediction);
    } else {
        // printf("%*sNode: Feature = %d, Threshold = %d\n", depth * 4, "", node->feature_index, node->threshold);
        print_tree(node->left, depth + 1);
        print_tree(node->right, depth + 1);
    }
}

void calculate_position_probabilities(DataRow dataset[], int dataset_size, const char *filename) {
    int positive_count = 0, negative_count = 0;
    int position_count[NUM_FEATURES][3][2] = {0}; // [position][x/o/b][positive/negative]

    // Count occurrences of x, o, and b for each position
    for (int i = 0; i < dataset_size; i++) {
        if (dataset[i].label == DT_POSITIVE)
            positive_count++;
        else
            negative_count++;

        for (int j = 0; j < NUM_FEATURES; j++) {
            if (dataset[i].features[j] == 1) // x
                position_count[j][0][dataset[i].label]++;
            else if (dataset[i].features[j] == 2) // o
                position_count[j][1][dataset[i].label]++;
            else // b
                position_count[j][2][dataset[i].label]++;
        }
    }

    // Open the file to write probabilities in overwrite mode
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file to save weights");
        return;
    }

    // Write class probabilities
    fprintf(file, "Class Probabilities:\n");
    fprintf(file, "  Positive: P(Positive) = %.4f\n", (double)positive_count / dataset_size);
    fprintf(file, "  Negative: P(Negative) = %.4f\n", (double)negative_count / dataset_size);
    fprintf(file, "--------------------------------------------\n");

    // Write position probabilities
    for (int i = 0; i < NUM_FEATURES; i++) {
        fprintf(file, "Position %d:\n", i + 1);
        fprintf(file, "  Symbol | P(Symbol | Positive) | P(Symbol | Negative)\n");
        fprintf(file, "  -------|----------------------|----------------------\n");

        const char *symbols[] = {"x", "o", "b"};
        for (int j = 0; j < 3; j++) {
            double p_positive = (positive_count > 0) ? (double)position_count[i][j][DT_POSITIVE] / positive_count : 0.0;
            double p_negative = (negative_count > 0) ? (double)position_count[i][j][DT_NEGATIVE] / negative_count : 0.0;

            fprintf(file, "  %-6s | %-20.4f | %-20.4f\n", symbols[j], p_positive, p_negative);
        }
        fprintf(file, "--------------------------------------------\n");
    }

    fclose(file);
    printf("Weights updated and saved to %s\n", filename);
}

double calculate_error_rate(DecisionTreeNode *root, DataRow dataset[], int size, int confusion_matrix[2][2]) {
    int error_count = 0; // Count the number of errors

    for (int i = 0; i < size; i++) {
        int prediction = predict_with_randomness(root, dataset[i].features);
        int actual = dataset[i].label;

        if (prediction != actual) {
            error_count++;
        }
    }

    // Calculate and return error rate as a percentage
    return ((double)error_count / size) * 100;
}

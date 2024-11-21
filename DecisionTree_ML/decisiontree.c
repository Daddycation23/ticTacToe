#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "decisiontree.h"

// Function to build, train, and evaluate the decision tree
void growth_Tree(DecisionTreeNode *tree) {
    DataRow dataset[MAX_ROWS];                                     // Array to store the dataset
    DataRow train_set[MAX_ROWS], test_set[MAX_ROWS];               // Training and testing datasets
    int dataset_size = 0, train_size = 0, test_size = 0;           // Sizes of datasets
    int train_confusion[2][2] = {0}, test_confusion[2][2] = {0};   // Confusion matrices
    float train_accuracy = 0.0, test_accuracy = 0.0;               // Accuracy for training and testing
    double train_error_rate = 0.0, test_error_rate = 0.0;          // Error rates
    int correct_train = 0, correct_test = 0;                       // Correctly classified samples

    // Initialize random seed for shuffling
    srand(time(NULL)); 

    // Load the dataset from the file
    load_dataset("tic-tac-toe.data", dataset, &dataset_size);

    // Shuffle the dataset to ensure random distribution
    shuffle_dataset(dataset, dataset_size);

    // Split the dataset into training (80%) and testing (20%) sets
    decision_tree_split_dataset(dataset, dataset_size, train_set, &train_size, test_set, &test_size, 0.8);

    // Build the decision tree using the training data
    tree = build_tree(train_set, train_size, 0);

    // Write position probabilities (weights) to a file
    calculate_position_probabilities(dataset, dataset_size, "DecisionTree_ML/DTweights.txt");

    // Clear the output file before appending results
    FILE *file = fopen("DecisionTree_ML/DTconfusion_matrix.txt", "w");
    if (file) fclose(file);

    // Evaluate the decision tree on the training data
    train_accuracy = evaluate_with_randomness(tree, train_set, train_size, train_confusion);
    correct_train = (int)(train_accuracy * train_size);
    display_confusion_matrix(train_confusion, "DecisionTree_ML/DTconfusion_matrix.txt", "Training");
    // printf("Training Accuracy: %.2f%% (%d/%d)\n", train_accuracy * 100, correct_train, train_size);
    write_accuracy_to_file("DecisionTree_ML/DTconfusion_matrix.txt", "Training", train_accuracy, correct_train, train_size);

    // Calculate error rate for the training data
    train_error_rate = calculate_error_rate(tree, train_set, train_size, train_confusion);
    // printf("Training Error Rate: %.2f%%\n", train_error_rate);
    file = fopen("DecisionTree_ML/DTconfusion_matrix.txt", "a");
    if (file) {
        fprintf(file, "Training Error Rate: %.2f%%\n", train_error_rate);
        fclose(file);
    }

    // Evaluate the decision tree on the testing data
    test_accuracy = evaluate_with_randomness(tree, test_set, test_size, test_confusion);
    correct_test = (int)(test_accuracy * test_size);
    display_confusion_matrix(test_confusion, "DecisionTree_ML/DTconfusion_matrix.txt", "Testing");
    // printf("Testing Accuracy: %.2f%% (%d/%d)\n", test_accuracy * 100, correct_test, test_size);
    write_accuracy_to_file("DecisionTree_ML/DTconfusion_matrix.txt", "Testing", test_accuracy, correct_test, test_size);

    // Calculate error rate for the testing data
    test_error_rate = calculate_error_rate(tree, test_set, test_size, test_confusion);
    // printf("Testing Error Rate: %.2f%%\n", test_error_rate);
    file = fopen("DecisionTree_ML/DTconfusion_matrix.txt", "a");
    if (file) {
        fprintf(file, "Testing Error Rate: %.2f%%\n", test_error_rate);
        fclose(file);
    }
}

// Function to load the dataset from a file
void load_dataset(const char *filename, DataRow dataset[], int *dataset_size) {
    // Open the dataset file in read mode
    FILE *file = fopen(filename, "r");
    if (!file) {
        // Print an error message if the file cannot be opened
        perror("Failed to open file");
        exit(1); // Exit the program with an error code
    }
    char line[256];    // Buffer to store each line of the file
    *dataset_size = 0; // Initialize the dataset size to zero
    // Read the file line by line
    while (fgets(line, sizeof(line), file)) {
        // Split the current line into tokens using ',' as the delimiter
        char *token = strtok(line, ","); 
        for (int i = 0; i < NUM_FEATURES; i++) {
            // Map the token value to corresponding feature representation
            if (strcmp(token, "x") == 0) 
                dataset[*dataset_size].features[i] = 1; // Assign 1 for 'x'
            else if (strcmp(token, "o") == 0) 
                dataset[*dataset_size].features[i] = 2; // Assign 2 for 'o'
            else 
                dataset[*dataset_size].features[i] = 0; // Assign 0 for blank space
            // Move to the next token in the line
            token = strtok(NULL, ","); 
        }
        // Assign the label based on the last token in the line
        dataset[*dataset_size].label = (strcmp(token, "positive\n") == 0) ? DT_POSITIVE : DT_NEGATIVE;
        // Increment the dataset size after processing each line
        (*dataset_size)++;
    }
    // Close the file after reading is complete
    fclose(file);
}

// Function to shuffle the dataset
void shuffle_dataset(DataRow dataset[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1); // Generate random index
        DataRow temp = dataset[i]; // Swap elements
        dataset[i] = dataset[j];
        dataset[j] = temp;
    }
}

// Function to split dataset into training and testing sets
void decision_tree_split_dataset(DataRow dataset[], int dataset_size, DataRow train_set[], int *train_size, DataRow test_set[], int *test_size, float train_ratio) {
    int train_limit = (int)(dataset_size * train_ratio); // Calculate training data size
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

// Function to build the decision tree with depth limit
DecisionTreeNode *build_tree(DataRow dataset[], int size, int depth) {
    int positives = 0, negatives = 0;

    // Count positive and negative labels in the dataset
    for (int i = 0; i < size; i++) {
        if (dataset[i].label == DT_POSITIVE) 
            positives++; // Increment positive count for positive labels
        else 
            negatives++; // Increment negative count for negative labels
    }

    // Stop conditions: max depth reached or node is pure (only positives or negatives)
    if (depth >= MAX_DEPTH || positives == 0 || negatives == 0) {
        // Allocate memory for a leaf node
        DecisionTreeNode *leaf = (DecisionTreeNode *)malloc(sizeof(DecisionTreeNode));
        leaf->is_leaf = 1; // Mark the node as a leaf
        leaf->prediction = (positives > negatives) ? DT_POSITIVE : DT_NEGATIVE; // Predict the majority class
        leaf->left = leaf->right = NULL; // Leaf nodes have no children
        return leaf; // Return the leaf node
    }

    // Variables to track the best feature and threshold for splitting
    int best_feature = -1, best_threshold = -1;
    float best_gini = 1.0;                    // Initialize the best Gini impurity to the highest value
    DataRow left[MAX_ROWS], right[MAX_ROWS];  // Temporary arrays for storing split datasets
    int left_size = 0, right_size = 0;        // Sizes of left and right subsets

    // Iterate over all features and possible thresholds to find the best split
    for (int feature_index = 0; feature_index < NUM_FEATURES; feature_index++) {
        for (int threshold = 0; threshold <= 2; threshold++) {
            // Calculate the Gini impurity for the current split
            float gini = calculate_gini_index(dataset, size, feature_index, threshold);
            if (gini < best_gini) {
                // Update the best Gini impurity, feature, and threshold if this split is better
                best_gini = gini;
                best_feature = feature_index;
                best_threshold = threshold;
            }
        }
    }

    // Split the dataset into left and right subsets based on the best feature and threshold
    decision_tree_split_data(dataset, size, best_feature, best_threshold, left, &left_size, right, &right_size);

    // Allocate memory for the new decision tree node
    DecisionTreeNode *node = (DecisionTreeNode *)malloc(sizeof(DecisionTreeNode));
    node->is_leaf = 0;                  // Mark the node as an internal (non-leaf) node
    node->feature_index = best_feature; // Store the best feature for splitting
    node->threshold = best_threshold;   // Store the best threshold for splitting
    // Recursively build the left subtree using the left subset
    node->left = build_tree(left, left_size, depth + 1);
    // Recursively build the right subtree using the right subset
    node->right = build_tree(right, right_size, depth + 1);

    return node; // Return the newly created decision tree node
}

// Function to evaluate the decision tree with randomness and update the confusion matrix
float evaluate_with_randomness(DecisionTreeNode *root, DataRow dataset[], int size, int confusion_matrix[2][2]) {
    int correct_predictions = 0;          // Counter for correct predictions
    // Initialize confusion matrix to zero
    for (int i = 0; i < 2; i++) {        // Iterate through rows of the matrix
        for (int j = 0; j < 2; j++) {    // Iterate through columns of the matrix
            confusion_matrix[i][j] = 0;  // Set each cell to zero
        }
    }
    // Iterate through the dataset to populate the confusion matrix
    for (int i = 0; i < size; i++) {
        int prediction = predict_with_randomness(root, dataset[i].features); // Get prediction from the decision tree
        int actual = dataset[i].label; // Retrieve the actual label from the dataset

        if (actual == DT_POSITIVE && prediction == DT_POSITIVE) {
            confusion_matrix[0][0]++; // Increment True Positive (TP)
            correct_predictions++;    // Increment correct predictions count
        } else if (actual == DT_NEGATIVE && prediction == DT_NEGATIVE) {
            confusion_matrix[1][1]++; // Increment True Negative (TN)
            correct_predictions++;    // Increment correct predictions count
        } else if (actual == DT_NEGATIVE && prediction == DT_POSITIVE) {
            confusion_matrix[1][0]++; // Increment False Positive (FP)
        } else if (actual == DT_POSITIVE && prediction == DT_NEGATIVE) {
            confusion_matrix[0][1]++; // Increment False Negative (FN)
        }
    }
    // Return the accuracy as the ratio of correct predictions to the total dataset size
    return (float)correct_predictions / size; // Calculate accuracy
}

// Function to make predictions with randomness in the decision tree
int predict_with_randomness(DecisionTreeNode *node, int features[]) {
    if (!node) {
        return DT_NEGATIVE; // Default prediction if the node is NULL
    }
    if (node->is_leaf) {
        // Introduce randomness to the prediction
        if ((float)rand() / RAND_MAX < RANDOMNESS_FACTOR) { // Compare a random value to RANDOMNESS_FACTOR
            return (node->prediction == DT_POSITIVE) ? DT_NEGATIVE : DT_POSITIVE; // Flip the prediction randomly
        }
        return node->prediction; // Return the prediction stored in the leaf node
    }
    // Traverse the decision tree based on the feature threshold
    if (features[node->feature_index] <= node->threshold) { // Compare feature value with threshold
        return predict_with_randomness(node->left, features); // Traverse left subtree if the condition is met
    } else {
        return predict_with_randomness(node->right, features); // Traverse right subtree otherwise
    }
}

// Function to display and log the confusion matrix to a file
void display_confusion_matrix(int confusion_matrix[2][2], const char *filename, const char *dataset_type) {
    FILE *file = fopen(filename, "a"); // Open file in append mode
    if (!file) {
        perror("Failed to open confusion matrix file");
        return;
    }

    // Extract TP, TN, FP, FN from the confusion matrix
    int TP = confusion_matrix[0][0];
    int FP = confusion_matrix[1][0];
    int TN = confusion_matrix[1][1];
    int FN = confusion_matrix[0][1];

    // Print confusion matrix and metrics to console
    /*
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
    */

    // Write confusion matrix and metrics to file
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

// Function to write accuracy results to a file
void write_accuracy_to_file(const char *filename, const char *dataset_type, float accuracy, int correct, int total) {
    FILE *file = fopen(filename, "a"); // Open file in append mode to add data
    if (!file) { // Check if the file was opened successfully
        perror("Failed to open file for writing accuracy"); // Print error message if file open fails
        return; // Exit the function if file cannot be opened
    }

    // Write the dataset type, accuracy percentage, and correct classification counts to the file
    fprintf(file, "%s Accuracy: %.2f%% (%d/%d)\n", dataset_type, accuracy * 100, correct, total);
    fclose(file); // Close the file to save changes
}

// Function to free the memory allocated for the decision tree
void free_tree(DecisionTreeNode *node) {
    if (node == NULL) return; // Base case: If the node is NULL, nothing to free, so return
    // Recursively free memory for the left subtree
    free_tree(node->left);
    // Recursively free memory for the right subtree
    free_tree(node->right);
    free(node); // Free the current node's memory
}

// Function to calculate the Gini index for a potential split
float calculate_gini_index(DataRow dataset[], int size, int feature_index, int threshold) {
    DataRow left[MAX_ROWS], right[MAX_ROWS]; // Temporary arrays to store left and right branches
    int left_size = 0, right_size = 0; // Initialize sizes of left and right branches

    // Split the dataset into left and right branches based on the feature and threshold
    decision_tree_split_data(dataset, size, feature_index, threshold, left, &left_size, right, &right_size);

    // If either branch is empty, return the worst Gini index (1.0) to discourage this split
    if (left_size == 0 || right_size == 0) return 1.0;
    // Initialize Gini indices for the left and right branches
    float gini_left = 1.0, gini_right = 1.0;
    int positives_left = 0, positives_right = 0; // Counters for positive labels in each branch
    // Count positive labels in the left branch
    for (int i = 0; i < left_size; i++) {
        if (left[i].label == DT_POSITIVE) positives_left++;
    }
    // Count positive labels in the right branch
    for (int i = 0; i < right_size; i++) {
        if (right[i].label == DT_POSITIVE) positives_right++;
    }

    // Calculate the probability of positive labels in the left branch
    float prob_left = (float)positives_left / left_size;
    // Calculate the Gini index for the left branch
    gini_left = 1.0 - (prob_left * prob_left) - ((1.0 - prob_left) * (1.0 - prob_left));
    // Calculate the probability of positive labels in the right branch
    float prob_right = (float)positives_right / right_size;
    // Calculate the Gini index for the right branch
    gini_right = 1.0 - (prob_right * prob_right) - ((1.0 - prob_right) * (1.0 - prob_right));

    // Return the weighted average of the Gini indices for both branches
    return ((gini_left * left_size) + (gini_right * right_size)) / size;
}

// Function to split the dataset into left and right branches
void decision_tree_split_data(DataRow dataset[], int size, int feature_index, int threshold, DataRow left[], int *left_size, DataRow right[], int *right_size) {
    *left_size = 0; // Initialize the size of the left branch to zero
    *right_size = 0; // Initialize the size of the right branch to zero

    // Iterate through the dataset to classify each data point into the left or right branch
    for (int i = 0; i < size; i++) {
        if (dataset[i].features[feature_index] <= threshold) { // Check if the feature value is less than or equal to the threshold
            left[(*left_size)++] = dataset[i]; // Add the data point to the left branch and increment its size
        } else { // Otherwise, add the data point to the right branch
            right[(*right_size)++] = dataset[i]; // Add the data point to the right branch and increment its size
        }
    }
}

// Function to predict the best move for the current player based on the decision tree
void dt_predict_best_move(DecisionTreeNode *tree, char board[3][3], char current_player, int *best_row, int *best_col) {
    if (!tree) {                // Check if the decision tree is not initialized
        printf("Error: Decision tree is not initialized!\n"); // Print error message
        return;                 // Exit the function
    }

    int features[NUM_FEATURES]; // Array to store the board features as numerical values
    int max_positive_prob = -1; // Variable to track the highest probability for a positive outcome
    *best_row = -1;             // Initialize the best_row variable to an invalid value
    *best_col = -1;             // Initialize the best_col variable to an invalid value
    int attempts = 0;           // Counter to limit the number of attempts to find the best move

    // Convert the 3x3 board into a feature array
    for (int i = 0; i < 3; i++) { // Loop through each row
        for (int j = 0; j < 3; j++) { // Loop through each column
            if (board[i][j] == 'x') features[i * 3 + j] = 1; // Map 'x' to 1
            else if (board[i][j] == 'o') features[i * 3 + j] = 2; // Map 'o' to 2
            else features[i * 3 + j] = 0; // Map empty cells ('b') to 0
        }
    }

    // Attempt to find the best move within a maximum of 5 iterations
    for (attempts = 0; attempts < 5; attempts++) {
        int temp_row = -1, temp_col = -1; // Temporary variables to store the coordinates of the current best move

        // Iterate over all cells of the board
        for (int i = 0; i < 3; i++) { // Loop through rows
            for (int j = 0; j < 3; j++) { // Loop through columns
                if (board[i][j] == 'b') { // Check if the current cell is empty
                    // Temporarily set the current player's move in the feature array
                    features[i * 3 + j] = (current_player == 'x') ? 1 : 2; // Map 'x' to 1 and 'o' to 2

                    // Use the decision tree to predict the outcome of this move
                    int prediction = predict_with_randomness(tree, features);

                    // If the prediction is positive and better than the current best, update the best move
                    if (prediction == DT_POSITIVE && (max_positive_prob == -1 || prediction > max_positive_prob)) {
                        temp_row = i; // Update the row of the best move
                        temp_col = j; // Update the column of the best move
                        max_positive_prob = prediction; // Update the highest positive probability
                    }

                    // Reset the feature array for the current cell back to empty
                    features[i * 3 + j] = 0;
                }
            }
        }

        // If a valid positive move is found, update best_row and best_col and exit the loop
        if (temp_row != -1 && temp_col != -1) {
            *best_row = temp_row; // Set the best move's row
            *best_col = temp_col; // Set the best move's column
            return; // Exit the function
        }
    }

    // If no positive move is found after 5 attempts, choose any random empty cell
    for (int i = 0; i < 3; i++) { // Loop through rows
        for (int j = 0; j < 3; j++) { // Loop through columns
            if (board[i][j] == 'b') { // Check if the cell is empty
                *best_row = i; // Assign the row of the random empty cell
                *best_col = j; // Assign the column of the random empty cell
                return; // Exit the function
            }
        }
    }
}

// Function to recursively print the structure of the decision tree
void print_tree(DecisionTreeNode *node, int depth) {
    if (!node) {
        // Base case: If the node is NULL, return
        return;
    }

    if (node->is_leaf) {
        // Print leaf node details 
        // printf("%*sLeaf: Prediction = %d\n", depth * 4, "", node->prediction);
    } else {
        // Print internal node details 
        // printf("%*sNode: Feature = %d, Threshold = %d\n", depth * 4, "", node->feature_index, node->threshold);
        print_tree(node->left, depth + 1); // Recur for the left child
        print_tree(node->right, depth + 1); // Recur for the right child
    }
}

// Function to calculate and save position probabilities for the dataset
void calculate_position_probabilities(DataRow dataset[], int dataset_size, const char *filename) {
    int positive_count = 0, negative_count = 0; // Counters for the number of positive and negative samples
    int position_count[NUM_FEATURES][3][2] = {0}; // Array to store counts of symbols ('x', 'o', empty) for each position and class

    // Count occurrences of each symbol ('x', 'o', empty) in every position for each class
    for (int i = 0; i < dataset_size; i++) {
        if (dataset[i].label == DT_POSITIVE) positive_count++; // Increment positive count if label is positive
        else negative_count++; // Increment negative count otherwise

        // Iterate over each feature (board position)
        for (int j = 0; j < NUM_FEATURES; j++) {
            if (dataset[i].features[j] == 1) position_count[j][0][dataset[i].label]++; // Count 'x'
            else if (dataset[i].features[j] == 2) position_count[j][1][dataset[i].label]++; // Count 'o'
            else position_count[j][2][dataset[i].label]++; // Count empty spaces
        }
    }

    // Open the file to save calculated probabilities
    FILE *file = fopen(filename, "w");
    if (!file) { // Check if the file was successfully opened
        perror("Failed to open file to save weights"); 
        return; // Exit the function
    }

    // Write the class probabilities to the file
    fprintf(file, "Class Probabilities:\n");
    fprintf(file, "  Positive: P(Positive) = %.4f\n", (double)positive_count / dataset_size); // Probability of positive class
    fprintf(file, "  Negative: P(Negative) = %.4f\n", (double)negative_count / dataset_size); // Probability of negative class
    fprintf(file, "--------------------------------------------\n");

    // Write position-wise probabilities to the file
    for (int i = 0; i < NUM_FEATURES; i++) { // Loop through each board position
        fprintf(file, "Position %d:\n", i + 1); // Position label (1-indexed)
        fprintf(file, "  Symbol | P(Symbol | Positive) | P(Symbol | Negative)\n");
        fprintf(file, "  -------|----------------------|----------------------\n");

        const char *symbols[] = {"x", "o", "b"}; // Define symbols corresponding to feature values
        for (int j = 0; j < 3; j++) { // Loop through symbols
            double p_positive = (positive_count > 0) ? (double)position_count[i][j][DT_POSITIVE] / positive_count : 0.0; // Probability of symbol given positive class
            double p_negative = (negative_count > 0) ? (double)position_count[i][j][DT_NEGATIVE] / negative_count : 0.0; // Probability of symbol given negative class
            fprintf(file, "  %-6s | %-20.4f | %-20.4f\n", symbols[j], p_positive, p_negative); // Write probabilities to file
        }
        fprintf(file, "--------------------------------------------\n"); // Separator line for readability
    }

    fclose(file); // Close the file after writing
    printf("Weights updated and saved to %s\n", filename); // Notify user of success
}

// Function to calculate the error rate of the decision tree
double calculate_error_rate(DecisionTreeNode *root, DataRow dataset[], int size, int confusion_matrix[2][2]) {
    int error_count = 0; // Counter to track the number of incorrect predictions

    // Iterate over the dataset to compare predictions with actual labels
    for (int i = 0; i < size; i++) {
        int prediction = predict_with_randomness(root, dataset[i].features); // Get the predicted label from the decision tree
        int actual = dataset[i].label; // Get the actual label from the dataset

        if (prediction != actual) { // Check if the prediction is incorrect
            error_count++; // Increment the error count
        }
    }

    // Calculate and return the error rate as a percentage of total samples
    return ((double)error_count / size) * 100;
}

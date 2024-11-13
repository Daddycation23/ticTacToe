#include "naivebayes.h"

int main() {
    char boards[1000][NUM_POSITIONS + 1];
    int outcomes[1000];
    int total_records = 0;

    // Load data
    load_data("tic-tac-toe.data", boards, outcomes, &total_records);

    // Split data
    char train_boards[800][NUM_POSITIONS + 1];
    int train_outcomes[800];
    char test_boards[200][NUM_POSITIONS + 1];
    int test_outcomes[200];
    int train_size = 0, test_size = 0;

    split_data(boards, outcomes, total_records, train_boards, train_outcomes, test_boards, test_outcomes, &train_size, &test_size, RATIO);

    // Train model
    NaiveBayesModel model;
    train_model(&model, train_boards, train_outcomes, train_size);

    // Save model weights to a file
    save_model(&model, "model_weights.txt");

    // Test model
    test_model(&model, test_boards, test_outcomes, test_size);

    // Play game using trained model
    play_game(&model);

    return 0;
}
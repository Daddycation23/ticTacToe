#ifndef NAIVEBAYES_H
#define NAIVEBAYES_H
#define NUM_POSITIONS 9
#define NUM_OUTCOMES 2 // positive or negative
#define POSITIVE 0
#define NEGATIVE 1
#define RATIO 0.8

// Define model type such that it holds the probabilites of number of attributes for both outcomes respectively
typedef struct {
    double class_probs[2]; 
    double x_probs[NUM_POSITIONS][NUM_OUTCOMES];
    double o_probs[NUM_POSITIONS][NUM_OUTCOMES];
    double b_probs[NUM_POSITIONS][NUM_OUTCOMES];
    // double class_probs[NUM_OUTCOMES];
} NaiveBayesModel;

// Function prototypes
void load_data(const char *filename, char boards[][NUM_POSITIONS + 1], int outcomes[], int *total_records);
void split_data(char boards[][NUM_POSITIONS + 1], int outcomes[], int total_records, char train_boards[][NUM_POSITIONS + 1], int train_outcomes[], char test_boards[][NUM_POSITIONS + 1], int test_outcomes[], int *train_size, int *test_size, float ratio);
void train_model(NaiveBayesModel *model, char boards[][NUM_POSITIONS + 1], int outcomes[], int size);
void save_model(const NaiveBayesModel *model, const char *filename);
void test_model(NaiveBayesModel *model, char boards[][NUM_POSITIONS + 1], int outcomes[], int size);
double calculate_probability(NaiveBayesModel *model, const char board[], int outcome);
int predict_outcome(NaiveBayesModel *model, const char board[]);
int predict_move(NaiveBayesModel *model, const char board[]);
void play_game(NaiveBayesModel *model);
int check_winner(const char board[]);
void print_board(const char board[]);
void LoadNaiveBayesModel();
void load_model(NaiveBayesModel *model, FILE *file);

// Utility functions
int outcome_index(const char *outcome);

#endif // NAIVEBAYES_H
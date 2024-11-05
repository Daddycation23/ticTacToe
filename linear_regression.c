#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FEATURES 9
#define DATASET_SIZE 958
#define TRAINING_RATIO 0.8

typedef struct {
    float features[FEATURES];
    int outcome;
} DataPoint;

typedef struct {
    int tp, tn, fp, fn;
} ConfusionMatrix;

void LoadAndSplitDataset(DataPoint dataset[DATASET_SIZE], DataPoint trainingSet[], DataPoint testingSet[], int *trainingSize, int *testingSize);
void TrainLinearRegression(DataPoint trainingSet[], float weights[FEATURES + 1], int trainingSize, float learningRate, int epochs);
float PredictLinearRegression(float weights[FEATURES + 1], float features[FEATURES]);
void EvaluateLinearRegression(DataPoint dataset[], float weights[FEATURES + 1], int start, int end, ConfusionMatrix *cm);

int main() {
    DataPoint dataset[DATASET_SIZE];
    DataPoint trainingSet[(int)(DATASET_SIZE * TRAINING_RATIO)];
    DataPoint testingSet[DATASET_SIZE - (int)(DATASET_SIZE * TRAINING_RATIO)];
    int trainingSize, testingSize;

    LoadAndSplitDataset(dataset, trainingSet, testingSet, &trainingSize, &testingSize);

    float weights[FEATURES + 1] = {0}; // +1 for the bias term

    TrainLinearRegression(trainingSet, weights, trainingSize, 0.01, 1000);

    ConfusionMatrix trainingCM = {0, 0, 0, 0};
    ConfusionMatrix testingCM = {0, 0, 0, 0};

    EvaluateLinearRegression(trainingSet, weights, 0, trainingSize, &trainingCM);
    EvaluateLinearRegression(testingSet, weights, 0, testingSize, &testingCM);

    printf("Training Confusion Matrix:\n");
    printf("TP: %d, TN: %d, FP: %d, FN: %d\n", trainingCM.tp, trainingCM.tn, trainingCM.fp, trainingCM.fn);

    printf("Testing Confusion Matrix:\n");
    printf("TP: %d, TN: %d, FP: %d, FN: %d\n", testingCM.tp, testingCM.tn, testingCM.fp, testingCM.fn);

    float trainingAccuracy = (float)(trainingCM.tp + trainingCM.tn) / trainingSize * 100;
    float testingAccuracy = (float)(testingCM.tp + testingCM.tn) / testingSize * 100;

    printf("Training Accuracy: %.2f%%\n", trainingAccuracy);
    printf("Testing Accuracy: %.2f%%\n", testingAccuracy);

    return 0;
}

void LoadAndSplitDataset(DataPoint dataset[DATASET_SIZE], DataPoint trainingSet[], DataPoint testingSet[], int *trainingSize, int *testingSize) {
    FILE *file = fopen("tic-tac-toe.data", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    char line[100];
    int index = 0;
    while (fgets(line, sizeof(line), file) && index < DATASET_SIZE) {
        for (int i = 0; i < FEATURES; i++) {
            if (line[i] == 'x') dataset[index].features[i] = 1.0;
            else if (line[i] == 'o') dataset[index].features[i] = -1.0;
            else dataset[index].features[i] = 0.0;
        }
        dataset[index].outcome = (strcmp(&line[10], "positive\n") == 0) ? 1 : 0;
        index++;
    }

    fclose(file);

    *trainingSize = (int)(DATASET_SIZE * TRAINING_RATIO);
    *testingSize = DATASET_SIZE - *trainingSize;

    for (int i = 0; i < *trainingSize; i++) {
        trainingSet[i] = dataset[i];
    }

    for (int i = *trainingSize; i < DATASET_SIZE; i++) {
        testingSet[i - *trainingSize] = dataset[i];
    }
}

void TrainLinearRegression(DataPoint trainingSet[], float weights[FEATURES + 1], int trainingSize, float learningRate, int epochs) {
    for (int epoch = 0; epoch < epochs; epoch++) {
        for (int i = 0; i < trainingSize; i++) {
            float prediction = PredictLinearRegression(weights, trainingSet[i].features);
            for (int j = 0; j < FEATURES; j++) {
                weights[j] += learningRate * (trainingSet[i].outcome - prediction) * trainingSet[i].features[j];
            }
            weights[FEATURES] += learningRate * (trainingSet[i].outcome - prediction); // Update bias
        }
    }
}

float PredictLinearRegression(float weights[FEATURES + 1], float features[FEATURES]) {
    float result = weights[FEATURES]; // Start with bias
    for (int i = 0; i < FEATURES; i++) {
        result += weights[i] * features[i];
    }
    return result > 0.5 ? 1.0 : 0.0; // Threshold at 0.5
}

void EvaluateLinearRegression(DataPoint dataset[], float weights[FEATURES + 1], int start, int end, ConfusionMatrix *cm) {
    for (int i = start; i < end; i++) {
        float prediction = PredictLinearRegression(weights, dataset[i].features);
        if (prediction == 1.0 && dataset[i].outcome == 1) cm->tp++;
        else if (prediction == 0.0 && dataset[i].outcome == 0) cm->tn++;
        else if (prediction == 1.0 && dataset[i].outcome == 0) cm->fp++;
        else if (prediction == 0.0 && dataset[i].outcome == 1) cm->fn++;
    }
} 
import matplotlib.pyplot as plt
import numpy as np

# Function to plot confusion matrix
def plot_confusion_matrix(matrix, set_name):
    classes = ['Positive', 'Negative']

    fig, ax = plt.subplots()
    cax = ax.matshow(matrix, cmap="Blues")
    plt.colorbar(cax)

    for (i, j), val in np.ndenumerate(matrix):
        ax.text(j, i, f"{val}", ha='center', va='center', color='black')

    plt.xlabel("Predicted")
    plt.ylabel("Actual")
    plt.title(f"{set_name} Confusion Matrix")
    plt.xticks([0, 1], classes)
    plt.yticks([0, 1], classes)
    plt.show()

# Read confusion matrix values from file
def read_confusion_matrix(filename, set_name):
    with open(filename, "r") as file:
        lines = file.readlines()

    for i, line in enumerate(lines):
        if set_name in line:
            matrix = np.array([
                [int(val) for val in lines[i + 1].strip().split(",")],
                [int(val) for val in lines[i + 2].strip().split(",")]
            ])
            return matrix

    return None

# Plot both training and testing confusion matrices
train_matrix = read_confusion_matrix("confusion_matrix.txt", "Training")
test_matrix = read_confusion_matrix("confusion_matrix.txt", "Testing")

if train_matrix is not None:
    plot_confusion_matrix(train_matrix, "Training")

if test_matrix is not None:
    plot_confusion_matrix(test_matrix, "Testing")

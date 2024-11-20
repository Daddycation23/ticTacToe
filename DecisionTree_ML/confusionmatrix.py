import matplotlib.pyplot as plt
import numpy as np
import os

# Function to plot confusion matrix
def plot_combined_confusion_matrix(
    train_matrix, train_TP, train_TN, train_FP, train_FN,
    test_matrix, test_TP, test_TN, test_FP, test_FN
):
    classes = ['Positive', 'Negative']
    fig, axes = plt.subplots(1, 2, figsize=(12, 6))  # Create a single figure with 2 subplots

    # Plot Training Confusion Matrix
    cax1 = axes[0].matshow(train_matrix, cmap="Blues")
    fig.colorbar(cax1, ax=axes[0])
    axes[0].set_title("Training Confusion Matrix")
    axes[0].set_xticks([0, 1])
    axes[0].set_yticks([0, 1])
    axes[0].set_xticklabels(classes)
    axes[0].set_yticklabels(classes)
    for (i, j), val in np.ndenumerate(train_matrix):
        axes[0].text(j, i, f"{val}", ha='center', va='center', color='black')

    # Add Training TP, TN, FP, FN
    axes[0].set_xlabel(
        f"TP: {train_TP}, TN: {train_TN}\nFP: {train_FP}, FN: {train_FN}",
        fontsize=10
    )
    axes[0].set_ylabel("Actual")

    # Plot Testing Confusion Matrix
    cax2 = axes[1].matshow(test_matrix, cmap="Blues")
    fig.colorbar(cax2, ax=axes[1])
    axes[1].set_title("Testing Confusion Matrix")
    axes[1].set_xticks([0, 1])
    axes[1].set_yticks([0, 1])
    axes[1].set_xticklabels(classes)
    axes[1].set_yticklabels(classes)
    for (i, j), val in np.ndenumerate(test_matrix):
        axes[1].text(j, i, f"{val}", ha='center', va='center', color='black')

    # Add Testing TP, TN, FP, FN
    axes[1].set_xlabel(
        f"TP: {test_TP}, TN: {test_TN}\nFP: {test_FP}, FN: {test_FN}",
        fontsize=10
    )
    axes[1].set_ylabel("Actual")

    # Adjust layout and save the combined image
    plt.tight_layout()
    output_path = os.path.join(os.getcwd(), "DT_Confusion_Matrix.png")
    plt.savefig(output_path, bbox_inches='tight')
    print(f"Saved combined plot to {output_path}")


# Function to read confusion matrix and extract TP, TN, FP, FN
def read_confusion_matrix(filename, set_name):
    with open(filename, "r") as file:
        lines = file.readlines()

    # Locate the matrix for the specified set (Training or Testing)
    for i, line in enumerate(lines):
        if f"Decision Tree {set_name} Confusion Matrix" in line:
            # Extract TP, TN, FP, FN from the file
            TP = int(lines[i + 1].split(":")[-1].strip())
            FP = int(lines[i + 2].split(":")[-1].strip())
            TN = int(lines[i + 3].split(":")[-1].strip())
            FN = int(lines[i + 4].split(":")[-1].strip())

            # Extract the confusion matrix rows
            row1 = [int(val) for val in lines[i + 8].strip().split()[-2:]]
            row2 = [int(val) for val in lines[i + 9].strip().split()[-2:]]
            matrix = np.array([row1, row2])

            return matrix, TP, TN, FP, FN

    return None, None, None, None, None


# Main logic to read confusion matrix and plot
def main():
    # File containing the confusion matrix
    filename = "DTconfusion_matrix.txt"

    # Print the current working directory for debugging
    print("Current Working Directory:", os.getcwd())

    # Read the Training Confusion Matrix
    train_matrix, train_TP, train_TN, train_FP, train_FN = read_confusion_matrix(filename, "Training")
    if train_matrix is None:
        print("Training confusion matrix not found in the file.")
        return

    # Read the Testing Confusion Matrix
    test_matrix, test_TP, test_TN, test_FP, test_FN = read_confusion_matrix(filename, "Testing")
    if test_matrix is None:
        print("Testing confusion matrix not found in the file.")
        return

    # Plot the combined confusion matrix
    plot_combined_confusion_matrix(
        train_matrix, train_TP, train_TN, train_FP, train_FN,
        test_matrix, test_TP, test_TN, test_FP, test_FN
    )

    # Verify saved files in the current directory
    print("Files in current folder:", os.listdir(os.getcwd()))


# Run the main function
if __name__ == "__main__":
    main()

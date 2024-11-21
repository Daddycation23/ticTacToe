import matplotlib.pyplot as plt  # Import Matplotlib for plotting
import numpy as np               # Import NumPy for numerical operations
import os                        # Import OS module for file path handling

# Function to plot confusion matrix
def plot_combined_confusion_matrix(
    train_matrix, train_TP, train_TN, train_FP, train_FN,
    test_matrix, test_TP, test_TN, test_FP, test_FN
):
    # Define class labels
    classes = ['Positive', 'Negative']

    # Create a single figure with 2 subplots for training and testing matrices
    fig, axes = plt.subplots(1, 2, figsize=(12, 6))

    # Plot Training Confusion Matrix
    cax1 = axes[0].matshow(train_matrix, cmap="Blues")  # Display the matrix as a color-coded plot
    fig.colorbar(cax1, ax=axes[0])                      # Add color bar to the plot
    axes[0].set_title("Training Confusion Matrix")      # Set title for the training subplot
    axes[0].set_xticks([0, 1])                          # Set x-axis ticks
    axes[0].set_yticks([0, 1])                          # Set y-axis ticks
    axes[0].set_xticklabels(classes)                    # Label x-axis ticks
    axes[0].set_yticklabels(classes)                    # Label y-axis ticks

    # Annotate each cell with its value
    for (i, j), val in np.ndenumerate(train_matrix):
        axes[0].text(j, i, f"{val}", ha='center', va='center', color='black')

    # Add textual description of TP, TN, FP, FN for training data
    axes[0].set_xlabel(
        f"TP: {train_TP}, TN: {train_TN}\nFP: {train_FP}, FN: {train_FN}",
        fontsize=10
    )
    axes[0].set_ylabel("Actual")  # Label the y-axis

    # Plot Testing Confusion Matrix
    cax2 = axes[1].matshow(test_matrix, cmap="Blues")  # Display the matrix as a color-coded plot
    fig.colorbar(cax2, ax=axes[1])                     # Add color bar to the plot
    axes[1].set_title("Testing Confusion Matrix")      # Set title for the testing subplot
    axes[1].set_xticks([0, 1])                         # Set x-axis ticks
    axes[1].set_yticks([0, 1])                         # Set y-axis ticks
    axes[1].set_xticklabels(classes)                   # Label x-axis ticks
    axes[1].set_yticklabels(classes)                   # Label y-axis ticks

    # Annotate each cell with its value
    for (i, j), val in np.ndenumerate(test_matrix):
        axes[1].text(j, i, f"{val}", ha='center', va='center', color='black')

    # Add textual description of TP, TN, FP, FN for testing data
    axes[1].set_xlabel(
        f"TP: {test_TP}, TN: {test_TN}\nFP: {test_FP}, FN: {test_FN}",
        fontsize=10
    )
    axes[1].set_ylabel("Actual")  # Label the y-axis

    # Adjust layout to avoid overlap and save the combined image
    plt.tight_layout()
    output_path = os.path.join(os.getcwd(), "DT_Confusion_Matrix.png")  # Define output path
    plt.savefig(output_path, bbox_inches='tight')                       # Save the plot to a file
    print(f"Saved combined plot to {output_path}")                      # Notify user of the saved file


# Function to read confusion matrix and extract TP, TN, FP, FN
def read_confusion_matrix(filename, set_name):
    # Open the confusion matrix file
    with open(filename, "r") as file:
        lines = file.readlines()  # Read all lines from the file

    # Locate the matrix for the specified set (Training or Testing)
    for i, line in enumerate(lines):  # Iterate over each line in the file with its index
        if f"Decision Tree {set_name} Confusion Matrix" in line:  # Check if the current line matches the specified set name
            # Extract TP, TN, FP, FN from the respective lines below the header
            TP = int(lines[i + 1].split(":")[-1].strip())  # Extract True Positive (TP) value from the next line
            FP = int(lines[i + 2].split(":")[-1].strip())  # Extract False Positive (FP) value from the line after TP
            TN = int(lines[i + 3].split(":")[-1].strip())  # Extract True Negative (TN) value from the line after FP
            FN = int(lines[i + 4].split(":")[-1].strip())  # Extract False Negative (FN) value from the line after TN

            # Extract the rows of the confusion matrix
            row1 = [int(val) for val in lines[i + 8].strip().split()[-2:]]  # Parse the first row of the matrix (last two values)
            row2 = [int(val) for val in lines[i + 9].strip().split()[-2:]]  # Parse the second row of the matrix (last two values)
            matrix = np.array([row1, row2])  # Combine the two rows into a NumPy array representing the matrix

            return matrix, TP, TN, FP, FN  # Return the confusion matrix and the extracted metrics

    # Return None if the desired section is not found in the file
    return None, None, None, None, None

# Main logic to read confusion matrix and plot
def main():
    # File containing the confusion matrix
    filename = "DTconfusion_matrix.txt"

    # Print the current working directory for debugging
    print("Current Working Directory:", os.getcwd())

    # Read the Training Confusion Matrix
    train_matrix, train_TP, train_TN, train_FP, train_FN = read_confusion_matrix(filename, "Training")
    if train_matrix is None:  # Check if the matrix was successfully read
        print("Training confusion matrix not found in the file.")
        return

    # Read the Testing Confusion Matrix
    test_matrix, test_TP, test_TN, test_FP, test_FN = read_confusion_matrix(filename, "Testing")
    if test_matrix is None:  # Check if the matrix was successfully read
        print("Testing confusion matrix not found in the file.")
        return

    # Plot the combined confusion matrix for training and testing
    plot_combined_confusion_matrix(
        train_matrix, train_TP, train_TN, train_FP, train_FN,
        test_matrix, test_TP, test_TN, test_FP, test_FN
    )

    # Print the files present in the current directory for debugging
    print("Files in current folder:", os.listdir(os.getcwd()))

# Entry point of the script
if __name__ == "__main__":
    main()

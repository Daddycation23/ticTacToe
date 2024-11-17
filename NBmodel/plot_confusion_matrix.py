import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

with open("NBmodel/NBmodel_confusion_matrix.txt", 'r') as file:
    content = file.readlines()

# Initialize dictionary to store results
results = {
    "Training": {"Correct Predictions": 0, "Total Predictions": 0, "Accuracy": 0, "Error": 0, "True Positive": 0, "False Positive": 0, "True Negative": 0, "False Negative": 0},
    "Testing": {"Correct Predictions": 0, "Total Predictions": 0, "Accuracy": 0, "Error": 0, "True Positive": 0, "False Positive": 0, "True Negative": 0, "False Negative": 0},
}

# Flag to track whether is training or testing dataset
current_dataset = None

# Iterate through each line
for line in content:
    line = line.strip()  # Remove any extra whitespace or newline characters

    # Identify the dataset (Training or Testing) based on the labels
    if "Training Dataset:" in line:
        current_dataset = "Training"
    elif "Testing Dataset:" in line:
        current_dataset = "Testing"

    # Parse Accuracy and Error
    if "Accuracy:" in line:
        results[current_dataset]["Accuracy"] = float(line.split(":")[1].split("(")[0].strip().replace('%', ''))
        correct, total = map(int, line.split("(")[1].split(")")[0].split("/"))
        results[current_dataset]["Correct Predictions"] = correct
        results[current_dataset]["Total Predictions"] = total
    elif "Error:" in line:
        results[current_dataset]["Error"] = float(line.split(":")[1].split("(")[0].strip().replace('%', ''))

    # Parse confusion matrix values
    elif "True Positive:" in line:
        results[current_dataset]["True Positive"] = int(line.split(":")[1].strip())
    elif "False Positive:" in line:
        results[current_dataset]["False Positive"] = int(line.split(":")[1].strip())
    elif "True Negative:" in line:
        results[current_dataset]["True Negative"] = int(line.split(":")[1].strip())
    elif "False Negative:" in line:
        results[current_dataset]["False Negative"] = int(line.split(":")[1].strip())

# # Print results
# print("Results:")
# for dataset, metrics in results.items():
#     print(f"\n{dataset} Dataset:")
#     print(f"  Accuracy: {metrics['Accuracy']}% ({metrics['Correct Predictions']}/{metrics['Total Predictions']})")
#     print(f"  Error: {metrics['Error']}%")
#     print("  Confusion Matrix:")
#     print(f"    True Positive: {metrics['True Positive']}")
#     print(f"    False Positive: {metrics['False Positive']}")
#     print(f"    True Negative: {metrics['True Negative']}")
#     print(f"    False Negative: {metrics['False Negative']}")

# Confusion matrix values for prediction on Training dataset
training_cm = np.array([
    [results["Training"]["True Negative"], results["Training"]["False Positive"]],
    [results["Training"]["False Negative"], results["Training"]["True Positive"]]
])

# Confusion matrix values for prediction on Testing dataset
testing_cm = np.array([
    [results["Testing"]["True Negative"], results["Testing"]["False Positive"]],
    [results["Testing"]["False Negative"], results["Testing"]["True Positive"]]
])

# Labels for plots
labels = [
    ['TN', 'FP'],
    ['FN', 'TP']
]

# Set up plots
fig, axes = plt.subplots(1, 2, figsize=(14, 7))

# Plotting Training Confusion Matrix
sns.heatmap(training_cm, annot=False, fmt='d', cmap='Blues', xticklabels=['Predicted NO', 'Predicted YES'], yticklabels=['Actual NO', 'Actual YES'], ax=axes[0], cbar=False)
for i in range(training_cm.shape[0]):
    for j in range(training_cm.shape[1]):
        axes[0].text(j + 0.5, i + 0.5, f"{labels[i][j]} = {training_cm[i, j]}",
                     ha='center', va='center', color='black', fontsize=14)
axes[0].set_title('Training Dataset Confusion Matrix')
axes[0].set_xlabel(f"n = {results['Training']['Total Predictions']}")

# Plotting Testing Confusion Matrix
sns.heatmap(testing_cm, annot=False, fmt='d', cmap='Reds', xticklabels=['Predicted NO', 'Predicted YES'], yticklabels=['Actual NO', 'Actual YES'], ax=axes[1], cbar=False)
for i in range(testing_cm.shape[0]):
    for j in range(testing_cm.shape[1]):
        axes[1].text(j + 0.5, i + 0.5, f"{labels[i][j]} = {testing_cm[i, j]}",
                     ha='center', va='center', color='black', fontsize=14)
axes[1].set_title('Testing Dataset Confusion Matrix')
axes[1].set_xlabel(f"n = {results['Testing']['Total Predictions']}")

# Display plots
plt.tight_layout()
plt.show()
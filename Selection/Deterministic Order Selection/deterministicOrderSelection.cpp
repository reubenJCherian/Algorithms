#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <fstream>

using namespace std;

//Uses insertion sort to sort smaller sized vectors to find the k-th element
void insertion_sort(vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
        int key = arr[i];
        int j = i - 1;

        //Shift elements greater than key to the right
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }

        arr[j + 1] = key;
    }
}

//Function is used to find the median of each group of 5
int find_and_move_median(vector<int>& arr, int left, int right) {
    //Sorts the group using Insertion Sort
    insertion_sort(arr, left, right);
    
    //Calculates the median index
    int median_idx = left + (right - left) / 2;
    
    //The value at median_idx is the median of this group
    return arr[median_idx];
}

//Function used to partition around a specific pivot element
int partition(vector<int>& arr, int left, int right, int pivot_value) {
    //Finds the actual index of the pivot_value
    int pivot_idx;
    for (pivot_idx = left; pivot_idx <= right; ++pivot_idx) {
        if (arr[pivot_idx] == pivot_value) {
            break;
        }
    }
    
    //Moves the pivot to the end of the current sub-array for partitioning
    swap(arr[pivot_idx], arr[right]);
    
    int i = left - 1;
    for (int j = left; j < right; ++j) {
        if (arr[j] <= pivot_value) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    
    //Moves the pivot back into its final sorted position
    swap(arr[i + 1], arr[right]);

    // Returns the pivot's new index
    return i + 1;
}

//Function finds the k-th smallest element (where k is 1-based rank) given a vector "arr"
int deterministic_select(vector<int>& arr, int left, int right, int k) {
    // k is the 1-based rank we are looking for
    // Base case: If the sub-array size is 1, return the element
    if (left == right) {
        return arr[left];
    }
    
    //Step 1: Divides array into groups of 5 and finds medians
    
    //Total number of elements in the current sub-array
    int n = right - left + 1;
    
    //Calculates the number of groups
    int num_groups = (n + 4) / 5;
    
    //Vector stores the medians of each group
    vector<int> medians;
    medians.reserve(num_groups);

    //Iterates through the vector in groups of 5
    for (int i = 0; i < num_groups; ++i) {
        int group_start = left + i * 5;
        int group_end = min(group_start + 4, right);
        
        //Sorts the group and gets its median value
        int median_val = find_and_move_median(arr, group_start, group_end);
        medians.push_back(median_val);
    }
    
    //Step 2: Recursively finds the Median of Medians which will be used as the pivot value
    int mom_rank_1based = (medians.size() + 1) / 2;
    int mom_pivot;
    
    //If there is only one median, that's the MoM otherwise, recurse
    if (medians.size() == 1) {
        mom_pivot = medians[0];
    } else {
        //Recursively finds the median of the medians
        mom_pivot = deterministic_select(medians, 0, medians.size() - 1, mom_rank_1based);
    }

    //Step 3: Partition the original array around the MoM pivot
    int pivot_idx = partition(arr, left, right, mom_pivot);
    
    //Calculate the rank of the pivot element in the current sub-array (1-based)
    int pivot_rank = pivot_idx - left + 1;

    //Step 4: Check pivot rank and recurse
    
    if (k == pivot_rank) {
        //The pivot element is the k-th smallest element
        return arr[pivot_idx];
    } else if (k < pivot_rank) {
        //The k-th element is in the left partition
        return deterministic_select(arr, left, pivot_idx - 1, k);
    } else {
        //The k-th element is in the right partition. We look for the (k - pivot_rank)-th element in the right partition.
        return deterministic_select(arr, pivot_idx + 1, right, k - pivot_rank);
    }
}

// Function to read k and the data array from the specified input file.
bool read_input_from_file(int& k, vector<int>& data, string filename) {
    ifstream inputFile(filename);
    
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open the file '" << filename << "'. "
             << "Please ensure it exists in the same directory as the executable." << endl;
        return false;
    }

    char c;
    int num;
    
    //1. Consumes initial '{'
    if (!(inputFile >> c) || c != '{') {
        cerr << "Error: File input must start with '{'." << endl;
        return false;
    }

    //2. Reads k (the target rank)
    if (!(inputFile >> k)) {
        cerr << "Error: Failed to read k (the target rank) from file." << endl;
        return false;
    }

    //3. Consumes characters until the array's opening '{'
    do {
        if (!(inputFile >> c)) {
            cerr << "Error: Unexpected end of file after reading k." << endl;
            return false;
        }
    } while (c != '{');
    
    //4. Reads array elements until '}' is found
    while (inputFile >> num) {
        data.push_back(num);
        
        //Checks for separator or end of array
        if (!(inputFile >> c)) break; // End of file - unexpected
        if (c == '}') break;    // Found '}' - end of array
        if (c != ',') {
            cerr << "Error: Expected ',' or '}' after an array element in file." << endl;
            return false;
        }
        //If c == ',', loop continues to read the next number
    }
    
    //5. Consumes the final '}' (if the inner loop broke on the inner '}')
    if (c != '}') {
        if (!(inputFile >> c) || c != '}') {
            cerr << "Error: File ended unexpectedly or missing final '}'." << endl;
            return false;
        }
    }
    
    inputFile.close();
    return true;
}

// Main function to execute the deterministic selection algorithm
int main() {
    int k = -1;
    vector<int> data;
    string filename = "example.txt";

    cout << "--- Deterministic Selection (Median of Medians) File 1 ---\n";
    cout << "Attempting to read data from '" << filename << "'...\n";
    
    if (!read_input_from_file(k, data, filename)) {
        //Error message already printed in read_input_from_file
        cout << "\n--- EXAMPLE INPUT ---" << endl;
        cout << "To run successfully, create a file named 'input.txt' with content like:\n";
        cout << "{3, {7, 10, 4, 3, 20, 15, 8, 11, 19, 1}}" << endl;
    }

    //Validation
    if (data.empty()) {
        cerr << "Error: The data array read from file is empty." << endl;
    }
    
    int size = data.size();
    if (k <= 0 || k > size) {
        cerr << "Error: Invalid rank k (" << k << "). Must be between 1 and " << size << "." << endl;
    }
    
    //Execution
    cout << "Input successfully parsed.\n";
    cout << "Target k (rank): " << k << " (The " << k << "-th smallest element)" << endl;
    cout << "Original array: Recieved";

    
    //Runs the deterministic selection algorithm
    //Uses a copy as the algorithm modifies the array in place
    vector<int> working_data = data; 
    int result = deterministic_select(working_data, 0, size - 1, k);
    
    cout << "\nResult of Deterministic Select: " << result << endl;

    //Verification
    sort(data.begin(), data.end());
    int correct_result = data[k - 1];
    
    if (result == correct_result) {
        cout << "Verification successful." << endl;
    } else {
        cout << "Verification failed. Expected: " << correct_result << endl;
    }

    return 0;
}
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

//Uses Dynamic Programming to determine the maximum profit that can be extracted given certain weight restrictions of the things that will be carried in a knapsack
int knapsack(int capacity, const vector<pair<int, int>>& bricks) {
    int n = bricks.size();

    //DP table: dp[i][w] stores the maximum value using a subset of the first 'i' items with a total weight of exactly 'w'
    //Dimension: (n + 1) rows for items, (capacity + 1) columns for weight
    vector<vector<int>> dp(n + 1, vector<int>(capacity + 1, 0));

    //Iterates through each item
    for (int i = 1; i <= n; ++i) {
        //Current item's value and weight
        int current_value = bricks[i - 1].first;
        int current_weight = bricks[i - 1].second;

        //Iterates through each possible weight from 0 up to capacity
        for (int w = 1; w <= capacity; ++w) {
            
            //Case 1: The current item's weight is more than the current capacity 'w', so can't be included
            if (current_weight > w) {
                dp[i][w] = dp[i - 1][w];
            } 
            //Case 2: The current item can be included
            else {
                //Option A: DO NOT include the current item
                //The max value is the same as the max value from the previous (i-1) items
                int value_without_item = dp[i - 1][w];

                //Option B: INCLUDE the current item
                //The max value is the item's value PLUS the max value from the previous (i-1) items with the remaining capacity (w - current_weight)
                int value_with_item = current_value + dp[i - 1][w - current_weight];

                //Takes the maximum of the two options
                dp[i][w] = max(value_without_item, value_with_item);
            }
        }
    }

    //The result is the maximum value achieved using all 'n' items  with the full 'capacity' W
    return dp[n][capacity];
}

//Reads and parses the knapsack input from a text file, storing the results in the provided reference parameters
//.   filename: The name of the input file (e.g., "input.txt")
//.   capacity_ref: A reference to store the maximum knapsack weight
//.   bricks_ref A: reference to store the vector of {value, weight} pairs
bool read_input_from_file(
    const string& filename, 
    int& capacity_ref, 
    vector<pair<int, int>>& bricks_ref) {
    
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return false;
    }

    // Read the single line of input
    getline(file, line);
    file.close();

    // Clean up the string to remove structural braces and spaces
    string cleaned_line;
    for (char c : line) {
        if (c != '{' && c != '}' && c != ' ') {
            cleaned_line += c;
        }
    }
    
    // Use a stringstream to parse comma-separated values
    stringstream ss(cleaned_line);
    string segment;

    // The first segment is the capacity
    if (getline(ss, segment, ',')) {
        try {
            capacity_ref = stoi(segment);
        } catch (const invalid_argument& e) {
            cerr << "Error parsing capacity: " << segment << endl;
            return false;
        }
    } else {
        return false; // Failed to read capacity
    }

    // Process the remaining segments (value, weight, count)
    while (getline(ss, segment, ',')) {
        // Read the value (v)
        int v;
        try {
            v = stoi(segment);
        } catch (const invalid_argument& e) {
            cerr << "Error parsing brick value: " << segment << endl;
            continue;
        }
        
        // Read the weight (w)
        if (!getline(ss, segment, ',')) break;
        int w;
        try {
            w = stoi(segment);
        } catch (const invalid_argument& e) {
            cerr << "Error parsing brick weight: " << segment << endl;
            continue;
        }
        
        // Read and ignore the count (always '1' for 0/1 Knapsack)
        if (!getline(ss, segment, ',')) break;
        
        // Store the {value, weight} pair
        bricks_ref.push_back({v, w});
    }

    return true;
}

//Runs program above with a given example
int main() {
    const string FILENAME = "example.txt";
    int knapsack_capacity = 0;
    vector<pair<int, int>> brick_data; // Stores {value, weight}

    //1. Reads and parses the input from the file
    cout << "Attempting to read input from: " << FILENAME << endl;
    bool success = read_input_from_file(FILENAME, knapsack_capacity, brick_data);

    if (!success || knapsack_capacity <= 0 || brick_data.empty()) {
        cerr << "\nExiting due to invalid or missing input data." << endl;
        return 1;
    }

    //2. Calculate sthe maximum profit
    int max_profit = knapsack(knapsack_capacity, brick_data);

    //3. Outputs the result
    cout << "\n--- 0/1 Knapsack Solution ---" << endl;
    cout << "Knapsack Capacity: " << knapsack_capacity << endl;
    cout << "Number of Bricks: " << brick_data.size() << endl;
    cout << "---------------------------------------" << endl;
    cout << "The maximum total potential profit that can be stolen is: " << max_profit << endl;
    cout << "---------------------------------------" << endl;

    return 0;
}
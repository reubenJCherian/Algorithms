#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <fstream> 
#include <string>  
#include <sstream> // Added for string stream parsing
#include <cctype>  // Added for isdigit

using namespace std;

// Function to find the minimum number of scalar multiplications required
// for matrix chain multiplication.
// The dimensions array 'P' stores the dimensions of the matrices.
// P[i-1] x P[i] are the dimensions of matrix Mi.
long long matrixChainOrder(const vector<int>& P) {
    // 'n' is the number of elements in the dimension array P.
    // The number of matrices is n - 1.
    int n = P.size();
    if (n <= 2) {
        // 0 or 1 matrix (n=1 or n=2) requires 0 multiplications.
        return 0;
    }

    // dp[i][j] will store the minimum number of scalar multiplications
    // needed to compute the matrix chain product M_i * ... * M_j.
    // We use long long for costs to avoid overflow.
    vector<vector<long long>> dp(n, vector<long long>(n, 0));

    // L is the chain length (number of matrices being multiplied)
    // L goes from 2 up to n-1 (the total number of matrices)
    for (int L = 2; L < n; L++) {
        // i is the starting matrix index (from 1 to n - L)
        for (int i = 1; i <= n - L; i++) {
            // j is the ending matrix index
            int j = i + L - 1;

            // Initialize the current minimum cost to a very large number
            dp[i][j] = LLONG_MAX;

            // Try every possible split point k (i <= k < j)
            // (M_i ... M_k) * (M_{k+1} ... M_j)
            for (int k = i; k <= j - 1; k++) {
                // Cost = (Cost of M_i..M_k) + (Cost of M_{k+1}..M_j) + (Cost of final multiplication)
                
                // P[i-1] x P[k] are the dimensions of the resultant of the first part (M_i..M_k)
                // P[k] x P[j] are the dimensions of the resultant of the second part (M_{k+1}..M_j)
                long long current_cost = dp[i][k] + dp[k + 1][j] + (long long)P[i - 1] * P[k] * P[j];

                // Update the minimum cost for this subproblem
                dp[i][j] = min(dp[i][j], current_cost);
            }
        }
    }

    // The result is the minimum cost for multiplying the entire chain M_1...M_{n-1}
    return dp[1][n - 1];
}

int progRunner(const string filename) {
    
}

int main() {
    const string filename = "examaple.txt";
    ifstream inputFile(filename);

    cout << "--- Matrix Chain Multiplication Solver ---" << endl;
    cout << "Attempting to read dimensions from file: " << filename << endl;

    if (!inputFile.is_open()) {
        cerr << "Error: Could not open file " << filename << ". Please ensure the file exists in the same directory." << endl;
        return 1; 
    }

    // Read the entire file content into a single string
    string fileContent;
    fileContent.assign((istreambuf_iterator<char>(inputFile)),
                       (istreambuf_iterator<char>()));
    
    inputFile.close();

    // --- Data Cleaning and Pre-processing ---
    string cleanContent;
    for (char c : fileContent) {
        if (isdigit(c)) {
            cleanContent += c;
        } else if (c == ',') {
            // Replace commas with spaces to act as delimiters
            cleanContent += ' ';
        } else if (c == ' ' || c == '\n' || c == '\r') {
            // Preserve delimiters but ensure no double spaces
            if (!cleanContent.empty() && cleanContent.back() != ' ') {
                cleanContent += ' ';
            }
        }
        // Braces {}, tabs, and other characters are ignored
    }
    
    // Trim potential leading/trailing spaces
    size_t first = cleanContent.find_first_not_of(' ');
    size_t last = cleanContent.find_last_not_of(' ');
    if (string::npos != first && string::npos != last) {
        cleanContent = cleanContent.substr(first, (last - first + 1));
    } else {
        cleanContent = ""; 
    }
    
    // Check for empty content after cleaning
    if (cleanContent.empty()) {
        cerr << "Error: File '" << filename << "' is empty or contains only non-numeric delimiters. Cannot process." << endl;
        return 1;
    }
    
    // Use stringstream to parse the cleaned, space-separated numbers
    stringstream ss_parser(cleanContent);

    vector<int> P; // The dimension array (e.g., {5, 5, 10, 13, 10})
    int row_dim, col_dim;
    int matrixCount = 0;
    
    // Read the (row, col) pairs from the string stream
    while (ss_parser >> row_dim >> col_dim) {
        matrixCount++;
        if (P.empty()) {
            // First matrix: M1 (row x col) -> P = {row, col}
            P.push_back(row_dim);
            P.push_back(col_dim);
            cout << "M" << matrixCount << ": " << row_dim << "x" << col_dim;
        } else {
            // Subsequent matrices: Mi (row x col)
            // 'row' must match the last dimension (column) of the previous matrix.
            if (P.back() != row_dim) {
                cerr << "\nValidation Error in file " << filename << ": Matrix " << (matrixCount - 1) << " column count (" << P.back() 
                     << ") does not match Matrix " << matrixCount << " row count (" << row_dim << ")." << endl;
                cerr << "Chain is invalid. Exiting this run." << endl;
                return 1;
            }
            // Append only 'col' to P, as 'row' is a duplicate of P.back()
            P.push_back(col_dim);
            cout << ", M" << matrixCount << ": " << row_dim << "x" << col_dim;
        }
    }
    cout << endl; 

    // Final check for parsing failure/incomplete data
    if (P.size() < 2) {
        cerr << "\nError in file " << filename << ": Found incomplete matrix data or parsing failure. Ensure all matrices have (row, col) pairs." << endl;
        return 1;
    }

    // Call the solver function
    long long min_multiplications = matrixChainOrder(P);

    cout << "\nInput Dimensions (P array): [";
    for (size_t i = 0; i < P.size(); ++i) {
        cout << P[i] << (i == P.size() - 1 ? "" : ", ");
    }
    cout << "]" << endl;

    cout << "Total number of matrices processed: " << matrixCount << endl;
    cout << "Minimum number of single-register multiplications required: " << min_multiplications << endl;
    
    // Check against the known example result
    if (matrixCount == 4 && min_multiplications == 1550) {
        cout << "Result matches the expected example answer (1550) for the input file." << endl;
    }

    return 0; // Success

}

/** 
 * This algorithm will take two files as input
 * Each input files will have data that will form a matrix
 * The matricies are multiplied using Strassen's Algorithm 
 * The output will be the sum of all the entries in the resulting matrix
*/


#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <stdexcept> // For runtime_error

using namespace std;
using Matrix = vector<vector<int>>;

// --- File Reading Functions ---

/**
 * @brief Reads a matrix from a file. It is now tolerant of C-style array formatting 
 * (commas and curly braces) which are replaced by spaces before parsing.
 * Checks for square matrices.
 */
Matrix readMatrixFromFile(const string& filename) {
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << ". Please ensure the file exists." << endl;
        return {};
    }

    // Read the entire file content into a single string stream
    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    
    // --- Modified Parsing Logic: Replace C-style delimiters with spaces ---
    for (char& c : content) {
        // Replace all delimiters with a space for clean parsing
        if (c == '{' || c == '}' || c == ',') {
            c = ' ';
        }
    }
    // --- End of Modified Parsing Logic ---
    
    stringstream ss(content);
    vector<int> all_elements;
    int val;
    
    // Extract all integers from the cleaned content
    while (ss >> val) {
        all_elements.push_back(val);
    }
    
    if (all_elements.empty()) {
        cerr << "Error: Matrix in " << filename << " is empty or contains no valid numbers." << endl;
        return {};
    }
    
    size_t total_elements = all_elements.size();
    int n = (int)round(sqrt(total_elements));
    
    // Check if the total number of elements forms a perfect square matrix (n x n)
    if ((size_t)n * n != total_elements) {
        cerr << "Error: Matrix in " << filename << " has " << total_elements 
             << " elements, which cannot form a square matrix (N x N)." << endl;
        return {};
    }
    
    // Reconstruct the square matrix
    Matrix M(n, vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            M[i][j] = all_elements[i * n + j];
        }
    }
    
    return M;
}
// --- Strassen's Helper Functions (Padding/Unpadding) ---

/**
 * @brief Finds the smallest power of 2 greater than or equal to n.
 */
int nextPowerOf2(int n) {
    if (n <= 0) return 1;
    if ((n & (n - 1)) == 0) return n; // Check if already power of 2
    int power = 1;
    while (power < n) power *= 2;
    return power;
}

/**
 * @brief Pads a matrix with zeros up to a new size 'new_size'.
 */
Matrix padMatrix(const Matrix& M, int new_size) {
    int old_size = M.size();
    if (old_size == new_size) return M;
    
    Matrix padded_M(new_size, vector<int>(new_size, 0));
    for (int i = 0; i < old_size; ++i) {
        for (int j = 0; j < old_size; ++j) {
            padded_M[i][j] = M[i][j];
        }
    }
    return padded_M;
}

/**
 * @brief Removes zero padding from a matrix, returning the original size.
 */
Matrix unpadMatrix(const Matrix& M, int original_size) {
    Matrix unpadded_M(original_size, vector<int>(original_size));
    for (int i = 0; i < original_size; ++i) {
        for (int j = 0; j < original_size; ++j) {
            unpadded_M[i][j] = M[i][j];
        }
    }
    return unpadded_M;
}

// --- Basic Matrix Operations ---

//Function will multiply the matricies the brute force way where each row-column are multiplied and added
Matrix bruteForce(const Matrix &matrixA, const Matrix &matrixB) {
    int n = matrixA.size();

    //Creates the resulting matrix with the appropriate size
    Matrix matrixC(n, vector<int>(n, 0));

    for(int i=0; i<n; ++i) {
        for(int j=0; j<n; ++j) {
            // Use long long for the intermediate sum to avoid overflow
            long long sum = 0; 
            for(int k=0; k<n; ++k) {
                sum += (long long)matrixA[i][k] * matrixB[k][j];
            }
            matrixC[i][j] = (int)sum;
        }
    }

    return matrixC;
}

//Function will take in two matrices and add them together. The resulting matrix is returned
Matrix matrixAdd(const Matrix &matrixA, const Matrix &matrixB) {
    int size = matrixA.size();
    Matrix matrixC(size, vector<int>(size));

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrixC[i][j] = matrixA[i][j] + matrixB[i][j];
        }
    }

    return matrixC;
}

//Function will take in two matrices and subtract them. The resulting matrix is returned
Matrix matrixSubtract(const Matrix &matrixA, const Matrix &matrixB) {
    int size = matrixA.size();
    Matrix matrixC(size, vector<int>(size));

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrixC[i][j] = matrixA[i][j] - matrixB[i][j];
        }
    }

    return matrixC;
}

//Function splits input matrixM into 4 sub-matrices and stores it into the second parameter result
void split(const Matrix &matrixM, vector<Matrix> &result) {
    int size = matrixM.size();
    
    // Check for even size, required for split
    if (size % 2 != 0) {
        throw runtime_error("Error: Split called on non-even matrix size. Padding error occurred.");
    }
    
    int halfSize = size / 2;

    result.resize(4);
    for (int i = 0; i < 4; ++i) {
        result[i].assign(halfSize, vector<int>(halfSize));
    }

    const int A11 = 0, A12 = 1, A21 = 2, A22 = 3;

    for (int i = 0; i < halfSize; ++i) {
        for (int j = 0; j < halfSize; ++j) {
            // A11 (Top-Left)
            result[A11][i][j] = matrixM[i][j];
            // A12 (Top-Right)
            result[A12][i][j] = matrixM[i][j + halfSize];
            // A21 (Bottom-Left)
            result[A21][i][j] = matrixM[i + halfSize][j];
            // A22 (Bottom-Right)
            result[A22][i][j] = matrixM[i + halfSize][j + halfSize];
        }
    }
}

//Function joins back four sub-matricies that are inputted
Matrix join(const Matrix &C11, const Matrix &C12, const Matrix &C21, const Matrix &C22) {
    int halfSize = C11.size();
    int size = halfSize * 2;
    Matrix matrixC(size, vector<int>(size));

    for (int i = 0; i < halfSize; ++i) {
        for (int j = 0; j < halfSize; ++j) {
            matrixC[i][j] = C11[i][j];
            matrixC[i][j + halfSize] = C12[i][j];
            matrixC[i + halfSize][j] = C21[i][j];
            matrixC[i + halfSize][j + halfSize] = C22[i][j];
        }
    }
    
    return matrixC;
}

// --- Strassen's Recursive Core ---

//Multiplies 2 matrices using Strassen's Algorithm - This is the recursive part
Matrix strassenMultiplyRecursive(const Matrix &matrixA, const Matrix &matrixB) {
    int n = matrixA.size();

    // Base Case: Switch to standard brute force multiplication for small matrices
    if (n <= 32) { // Use a reasonable cutoff for efficiency
        return bruteForce(matrixA, matrixB);
    }

    // Split matrices matrixA and matrixB into four n/2 x n/2 submatrices
    vector<Matrix> A_parts(4), B_parts(4);
    split(matrixA, A_parts);
    split(matrixB, B_parts);

    const Matrix& A11 = A_parts[0];
    const Matrix& A12 = A_parts[1];
    const Matrix& A21 = A_parts[2];
    const Matrix& A22 = A_parts[3];

    const Matrix& B11 = B_parts[0];
    const Matrix& B12 = B_parts[1];
    const Matrix& B21 = B_parts[2];
    const Matrix& B22 = B_parts[3];

    // 10 intermediate matrices (S1 to S10)
    Matrix S1 = matrixSubtract(B12, B22);
    Matrix S2 = matrixAdd(A11, A12);
    Matrix S3 = matrixAdd(A21, A22);
    Matrix S4 = matrixSubtract(B21, B11);
    Matrix S5 = matrixAdd(A11, A22);
    Matrix S6 = matrixAdd(B11, B22);
    Matrix S7 = matrixSubtract(A12, A22);
    Matrix S8 = matrixAdd(B21, B22);
    Matrix S9 = matrixSubtract(A11, A21);
    Matrix S10 = matrixAdd(B11, B12);

    // 7 recursive calls to matrix multiplication (P1 to P7):
    Matrix P1 = strassenMultiplyRecursive(A11, S1);
    Matrix P2 = strassenMultiplyRecursive(S2, B22);
    Matrix P3 = strassenMultiplyRecursive(S3, B11);
    Matrix P4 = strassenMultiplyRecursive(A22, S4);
    Matrix P5 = strassenMultiplyRecursive(S5, S6);
    Matrix P6 = strassenMultiplyRecursive(S7, S8);
    Matrix P7 = strassenMultiplyRecursive(S9, S10);

    // Combining the products to get the four result quadrants (C11, C12, C21, C22):

    // C11 = P5 + P4 - P2 + P6
    Matrix C11 = matrixAdd(matrixSubtract(matrixAdd(P5, P4), P2), P6);

    // C12 = P1 + P2
    Matrix C12 = matrixAdd(P1, P2);

    // C21 = P3 + P4
    Matrix C21 = matrixAdd(P3, P4);

    // C22 = P5 + P1 - P3 - P7
    Matrix C22 = matrixSubtract(matrixSubtract(matrixAdd(P5, P1), P3), P7);

    // Join the four quadrants into the final result matrixC
    return join(C11, C12, C21, C22);
}

// --- Strassen's Top-Level Function with Padding ---

/**
 * @brief Top-level function that handles padding before calling the recursive Strassen's core.
 */
Matrix strassenMultiply(const Matrix &matrixA, const Matrix &matrixB) {
    int n = matrixA.size();
    
    // 1. Determine the necessary size (next power of 2)
    int target_size = nextPowerOf2(n);
    
    // 2. Pad the matrices to the target size
    Matrix paddedA = padMatrix(matrixA, target_size);
    Matrix paddedB = padMatrix(matrixB, target_size);

    // 3. Perform recursive Strassen's multiplication
    Matrix paddedC = strassenMultiplyRecursive(paddedA, paddedB);

    // 4. Unpad the result to get the matrix of the original size (n x n)
    return unpadMatrix(paddedC, n);
}


// --- Utility and Main Function ---

/**
 * @brief Calculates the sum of all entries in the matrix.
 * @param m The input matrix (taken as const reference).
 * @return The sum of all entries (using long long for safety).
 */
long long sumOfMatrixEntries(const Matrix &m) {
    long long sum = 0;
    for(size_t i=0; i<m.size(); i++) {
        for(size_t j=0; j<m[i].size(); j++) {
            sum += m[i][j];
        }
    }
    return sum;
}

/**
 * @brief Prints the matrix to the console.
 */
void printMatrix(const Matrix& M, const string& name) {
    cout << name << " Matrix (" << M.size() << "x" << M.size() << "):" << endl;
    for (const auto& row : M) {
        for (int val : row) {
            cout << val << "\t";
        }
        cout << "\n";
    }
}

//Gives an example usage using the two text files exampleMatrix1 and exampleMatrix2
int main() {

    // Define the filenames for input as specified in your last request
    const string filenameA = "exampleMatrix1.txt";
    const string filenameB = "exampleMatrix2.txt";

    Matrix matrixA = readMatrixFromFile(filenameA);
    Matrix matrixB = readMatrixFromFile(filenameB);
    Matrix matrixC = strassenMultiply(matrixA, matrixB);
    int answer = sumOfMatrixEntries(matrixC);

    cout << answer;

}



/*
Objective:
    • Implement a robust matrix class using object-oriented programming principles in C++.
    • Apply multithreading std::thread to enhance computational efficiency for large matrix multiplication.
    • Compare the performance of single-threaded and multithreaded computations.
*/

#include <vector>
#include <thread>
#include <functional>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <random>
using namespace std;

class Matrix {
private:
    int rows; 
    int cols;
    std::vector<std::vector<int>> data;  // 2D vector to store matrix data

public:
    // Constructor to initialize matrix with given dimensions and fill with zeros
    Matrix(int rows = 1, int cols = 1) : rows(rows), cols(cols), data(rows, std::vector<int>(cols, 0)) {}
    ~Matrix() {}

    // Function to access individual elements of the matrix
    int& at(int row, int col) {
        if (row < 0 || row >= rows || col < 0 || col >= cols) {
            throw out_of_range("Index out of range");
        }
        return data[row][col];
    }

    // Function to initialize the matrix with random values between 1 and 10
    void fill_increment_value() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 10);

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                data[i][j] = dis(gen);
            }
        }
    }

    // Function to perform single-threaded matrix multiplication
    Matrix multiply(const Matrix& other) {
        if (this->cols != other.rows) {
            throw invalid_argument("Matrix dimensions mismatch for multiplication");
        }

        Matrix result(this->rows, other.cols);

        for (int i = 0; i < this->rows; ++i) {
            for (int j = 0; j < other.cols; ++j) {
                for (int k = 0; k < this->cols; ++k) {
                    result.data[i][j] += this->data[i][k] * other.data[k][j];
                }
            }
        }

        return result;
    }

    // Function for multi-threaded matrix multiplication
    Matrix multiplyParallel(const Matrix& other, int num_threads) {
        if (this->cols != other.rows) {
            throw invalid_argument("Matrix dimensions mismatch for multiplication");
        }

        Matrix result(this->rows, other.cols);

        vector<thread> threads;
        int elements_per_thread = this->rows * other.cols / num_threads;

        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back([&, i]() {
                int start = i * elements_per_thread;
                int end = (i == num_threads - 1) ? (this->rows * other.cols) : ((i + 1) * elements_per_thread);

                for (int index = start; index < end; ++index) {
                    int row = index / other.cols;
                    int col = index % other.cols;
                    for (int k = 0; k < this->cols; ++k) {
                        result.data[row][col] += this->data[row][k] * other.data[k][col];
                    }
                }
            });
        }

        // Wait for all threads to finish
        for (auto& t : threads) {
            t.join();
        }

        return result;
    }
};

// MUST NOT CHANGE
int demo(int row_1, int col_1, int row_2, int col_2, int num_threads=2) {

    // Create two large matrices
    Matrix matrixA(row_1, col_1);  // Initialize with 0
    matrixA.fill_increment_value();
    Matrix matrixB(row_2, col_2);  // Initialize with 0
    matrixB.fill_increment_value();

    // Perform single-threaded matrix multiplication
    auto start = chrono::high_resolution_clock::now();
    Matrix resultSingle = matrixA.multiply(matrixB);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> singleThreadTime = end - start;
    cout << "Single-threaded multiplication took " << singleThreadTime.count() << " ms." << endl;

    // Perform multithreaded matrix multiplication
    start = chrono::high_resolution_clock::now();
    Matrix resultMulti = matrixA.multiplyParallel(matrixB, num_threads);  // Using 4 threads
    end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> multiThreadTime = end - start;
    cout << "Multithreaded multiplication took " << multiThreadTime.count() << " ms." << endl;

    // Compare results for validation purposes
    cout << "Validating results..." << endl;
    bool valid = true;
    for (int i = 0; i < row_1; i++) {
        for (int j = 0; j < col_2; j++) {
            if (resultSingle.at(i, j) != resultMulti.at(i, j)) {
                valid = false;
                break;
            }
        }
        if (!valid) break;
    }
    if (valid) {
        cout << "Results are identical!" << endl;
    } else {
        cout << "Results differ!" << endl;
    }

    return 0;
}

// Function to safely get integer input from the user
void SafeIntInput(int &input, const string& info)
{
    cout<<info;
    cin>>input;

    while(cin.fail() or input<1)
    {
        cout<<"Invalid input. "<<info;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        cin>>input;
    }
}

int main()
{
    int row_1 = 0, col_1 = 0, row_2 = 0, col_2 = 0, num_threads=2;
    char QUIT = 'N';
    while (!(tolower(QUIT) == 'y'))
    {
        SafeIntInput(row_1,"Enter the number of rows of Matrix A: ");
        SafeIntInput(col_1,"Enter the number of cols of Matrix A: ");
        SafeIntInput(row_2,"Enter the number of rows of Matrix B: ");
        SafeIntInput(col_2,"Enter the number of cols of Matrix B: ");

        SafeIntInput(num_threads,"Enter the number of threads: ");

        try
        {
            demo(row_1,col_1,row_2,col_2,num_threads);
        }
        catch(const exception &e)
        {
            cout << e.what() << endl;
        }

        cout<<"Quit (Y/N): ";
        while(!(cin>>QUIT) or !(tolower(QUIT) == 'y' or tolower(QUIT) == 'n'))
        {
            cout<<"Invalid input: the input must be either Y or N. Quit (Y/N):";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cout<<endl;

    }
}
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

int main() {
    srand(time(NULL)); // Seed the random number generator with the current time

    // Generate N random numbers
    int N = 20; // You can change this to the desired number of random numbers
    int* numbers = new int[N];
    for (int i = 0; i < N; ++i) {
        numbers[i] = rand() % 100; // Generate random numbers between 0 and 99
    }

    // Write the random numbers to a file
    std::ofstream outFile("random_numbers.txt");
    for (int i = 0; i < N; ++i) {
        outFile << numbers[i];
        if (i < N - 1) {
            outFile << ",";
        }
    }
    outFile.close();

    // Read the numbers from the file
    std::ifstream inFile("random_numbers.txt");
    if (!inFile) {
        std::cerr << "Error opening file for reading." << std::endl;
        return 1;
    }

    // Read the numbers into an array
    int* readNumbers = new int[N];
    for (int i = 0; i < N; ++i) {
        char comma;
        inFile >> readNumbers[i] >> comma;
    }
    inFile.close();

    // Sort the numbers
    std::sort(readNumbers, readNumbers + N);

    // Write the sorted numbers to another file
    std::ofstream sortedFile("sorted_numbers.txt");
    for (int i = 0; i < N; ++i) {
        sortedFile << readNumbers[i];
        if (i < N - 1) {
            sortedFile << ",";
        }
    }
    sortedFile.close();

    // Clean up memory
    delete[] numbers;
    delete[] readNumbers;

    return 0;
}

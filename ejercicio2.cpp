// Universidad del Valle de Guatemala
// Computación Paralela y Distribuida
// Stefano Aragoni, Carol Arevalo
// 23/08/2023

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <omp.h>
#include <cmath>
#include <chrono>

int main() {

    // Obtenemos el tiempo de inicio
    auto start_time = std::chrono::high_resolution_clock::now();

    // Configuración inicial
    srand(time(NULL)); // Inicialización del generador de números aleatorios basado en la hora actual

    // Definir el rango de números aleatorios
    int exponente = 2; // Exponente para determinar el tamaño del rango de números
    int limit = pow(10, exponente);

    int* numbers = new int[limit]; // Declaración y reserva de memoria para 'numbers'


    // Generar N números aleatorios en paralelo //CAMBIO 2
    #pragma omp parallel for
    for (int i = 0; i < limit; ++i) {
        numbers[i] = rand() % 100;
    }

    // Escribir los números aleatorios en un archivo
    std::ofstream outFile("random_numbers_p.csv");
    for (int i = 0; i < limit; ++i) {
        outFile << numbers[i];
        if (i < limit - 1) {
            outFile << ",";
        }
    }
    outFile.close();

    // Leer los números desde el archivo
    std::ifstream inFile("random_numbers_p.csv");
    if (!inFile) {
        std::cerr << "Error al abrir el archivo para lectura." << std::endl;
        return 1;
    }

    // Leer los números en un arreglo
    int* readNumbers = new int[limit];
    for (int i = 0; i < limit; ++i) {
        char comma;
        inFile >> readNumbers[i] >> comma;
    }
    inFile.close();

    // Ordenar los números en paralelo //CAMBIO 1
    #pragma omp parallel for
    for (int i = 0; i < limit; ++i) {
        std::sort(readNumbers, readNumbers + limit);
    }
    
    // Escribir los números ordenados en otro archivo
    std::ofstream sortedFile("sorted_numbers.csv");
    for (int i = 0; i < limit; ++i) {
        sortedFile << readNumbers[i];
        if (i < limit - 1) {
            sortedFile << ",";
        }
    }
    sortedFile.close();

    // Liberar la memoria
    delete[] numbers;
    delete[] readNumbers;

    // Obtenemos el tiempo de finalización
    auto end_time = std::chrono::high_resolution_clock::now();

    // Calculamos la duración total de la ejecución en segundos
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    // Imprimimos el tiempo de ejecución en segundos
    std::cout << "Tiempo de ejecución: " << duration.count() << " microsegundos." << std::endl;


    return 0;
}

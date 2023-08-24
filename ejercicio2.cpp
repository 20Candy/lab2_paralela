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
#include <random>

// -----QUICK SORT-----
// Funciones proveídas por Sebastián.

int compare (const int * a, const int * b) //what is it returning?
{
    return ( *(int*)a - *(int*)b ); //What is a and b?
}

void par_qsort(int *data, int lo, int hi) //}, int (*compare)(const int *, const int*))
{
    if(lo > hi) return;
    int l = lo;
    int h = hi;
    int p = data[(hi + lo)/2];

    while (l <= h) {
        while ((data[l] - p) < 0) l++;
        while ((data[h] - p) > 0) h--;
        if (l <= h) {
            // Swap
            int tmp = data[l];
            data[l] = data[h];
            data[h] = tmp;
            l++; h--;
        }
    }
        
    // Recursive call
    par_qsort(data, lo, h);
    par_qsort(data, l, hi);

}

// -----MAIN-----
int main() {

    // Obtenemos el tiempo de inicio
    auto start_time = std::chrono::high_resolution_clock::now();

    // Seed basado en la hora actual
    unsigned int seed = 42;
    std::mt19937 gen(seed);

    // Definir el rango de números aleatorios
    int N = 10e6;
    int posibles_elementos = N/2;

    // Declaración y reserva de memoria para 'numbers'
    int* numbers = new int[N]; 

    // ------------CAMBIO 1 - VERSION 1----------------
    // Generar N números aleatorios en paralelo 
    #pragma omp parallel for                // Paralelizar el bucle
    for (int i = 0; i < N; ++i) {
        std::uniform_int_distribution<int> distribution(1, posibles_elementos);
        numbers[i] = distribution(gen);
    }

    // ------------ORIGINAL (CAMBIO 1 - VERSION 1)----------------
    // for (int i = 0; i < N; ++i) {
    //     std::uniform_int_distribution<int> distribution(1, posibles_elementos);
    //     numbers[i] = distribution(gen);
    // }



    // ------------CAMBIO 2 - VERSION 1----------------
    // Escribir los números aleatorios en un archivo
    std::ofstream outFile("random_numbers_P.csv");

    int num_hilos = omp_get_max_threads();              // Número de hilos
    int nums_bloque = ceil((double)N / num_hilos);      // Números por bloque

    std::string* localBuffers1 = new std::string[num_hilos];     // Buffer local para cada hilo

    #pragma omp parallel                        
    {
        int ID = omp_get_thread_num();                  // ID del hilo
        int inicio = ID * nums_bloque;                  // Inicio del bloque
        int fin = std::min(inicio + nums_bloque, N);    // Fin del bloque

        for (int i = inicio; i < fin; ++i) {
            localBuffers1[ID] += std::to_string(numbers[i]);     // Cada hilo escribe en su buffer
            localBuffers1[ID] += ",";                                // Escribe los números en orden
        }

        #pragma omp critical                // Critical: Solo un hilo escribe a la vez
        outFile << localBuffers1[ID];       // Cada hilo escribe su buffer en el archivo
    }

    delete[] localBuffers1;          // Liberar memoria
    outFile.close();

    // ------------ORIGINAL (CAMBIO 2 - VERSION 1)----------------
    // std::ofstream outFile("random_numbers_S.csv");
    // for (int i = 0; i < N; ++i) {
    //     std::string output = std::to_string(numbers[i]);
    //     output += ",";
        
    //     outFile << output;
    // }



    outFile.close();

    // Leer los números desde el archivo
    std::ifstream inFile("random_numbers_P.csv");
    if (!inFile) {
        std::cerr << "Error al abrir el archivo para lectura." << std::endl;
        return 1;
    }

    // ------------CAMBIO 3 - VERSION 1----------------
    // Leer los números en un arreglo
    int* readNumbers = new int[N];
    #pragma omp parallel for                    // Paralelizar el bucle
    for (int i = 0; i < N; ++i) {
        char comma;
        int num;

        #pragma omp critical                    // Critical: Solo un hilo lee a la vez
        {
            inFile >> num >> comma;             // Cada hilo lee un número
        }
    
        readNumbers[i] = num;                   // Cada hilo escribe en su posición del arreglo

    }

    // ------------ORIGINAL (CAMBIO 3 - VERSION 1)----------------
    // int* readNumbers = new int[N];
    // for (int i = 0; i < N; ++i) {
    //     char comma;
    //     inFile >> readNumbers[i] >> comma;
    // }



    inFile.close();

    // Ordenar los números usando ejemplo de Sebastián
    par_qsort(readNumbers, 0, N - 1);
    
    // ------------CAMBIO 4 - VERSION 1----------------
    // Escribir los números ordenados en otro archivo
    std::ofstream sortedFile("sorted_numbers_S.csv");

    num_hilos = omp_get_max_threads();              // Número de hilos
    nums_bloque = ceil((double)N / num_hilos);      // Números por bloque

    std::string* localBuffers = new std::string[num_hilos];     // Buffer local para cada hilo

    #pragma omp parallel                        
    {
        int ID = omp_get_thread_num();                  // ID del hilo
        int inicio = ID * nums_bloque;                  // Inicio del bloque
        int fin = std::min(inicio + nums_bloque, N);    // Fin del bloque

        for (int i = inicio; i < fin; ++i) {
            localBuffers[ID] += std::to_string(readNumbers[i]);     // Cada hilo escribe en su buffer
            localBuffers[ID] += ",";                                // Escribe los números en orden
        }

        #pragma omp barrier             // Esperar a que todos los hilos terminen de
                                        // llenar los buffers locales (con numeros ordenados)

        #pragma omp master              // Solo el hilo master escribe al archivo
        {
            for (int i = 0; i < num_hilos; ++i) {
                sortedFile << localBuffers[i];          // Garantiza el orden de los números
            }
        }
    }

    delete[] localBuffers;          // Liberar memoria
    sortedFile.close();

    // ------------ORIGINAL (CAMBIO 4 - VERSION 1)----------------
    // std::ofstream sortedFile("sorted_numbers_S.csv");
    // for (int i = 0; i < N; ++i) {
    //     std::string output = std::to_string(readNumbers[i]);
    //     output += ",";
        
    //     sortedFile << output;
    // }



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

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

  while(l <= h){
    while((data[l] - p) < 0) l++;
    while((data[h] - p) > 0) h--;
    if(l<=h){
      //swap
      int tmp = data[l];
      data[l] = data[h];
      data[h] = tmp;
      l++; h--;
    }
  }
  //recursive call
  par_qsort(data, lo, h);
  par_qsort(data, l, hi);
}

// -----MAIN-----
int main() {

    // Obtenemos el tiempo de inicio
    auto start_time = std::chrono::high_resolution_clock::now();

    // Seed basado en la hora actual
    srand(time(NULL));

    // Definir el rango de números aleatorios
    int N = 10e3;
    int posibles_elementos = N/2;

    // Declaración y reserva de memoria para 'numbers'
    int* numbers = new int[N]; 

    // Generar N números aleatorios en paralelo 
    #pragma omp parallel for    //CAMBIO 1: Paralelizar el bucle
    for (int i = 0; i < N; ++i) {
        numbers[i] = rand()%(posibles_elementos)+1;
    }

    // Escribir los números aleatorios en un archivo
    std::ofstream outFile("random_numbers_P.csv"); 
    #pragma omp parallel for    // CAMBIO 2: Paralelizar el bucle
    for (int i = 0; i < N; ++i) {
        outFile << numbers[i];
        if (i < N - 1) {
            outFile << ",";
        }
    }
    outFile.close(); // Cerrar el archivo después del bucle

    // Leer los números desde el archivo
    std::ifstream inFile("random_numbers_P.csv");
    if (!inFile) {
        std::cerr << "Error al abrir el archivo para lectura." << std::endl;
        return 1;
    }

    // Leer los números en un arreglo
    int* readNumbers = new int[N];
    #pragma omp parallel for    // CAMBIO 3: Paralelizar el bucle
    for (int i = 0; i < N; ++i) {
        char comma;
        inFile >> readNumbers[i] >> comma;
    }
    inFile.close();

    // Ordenar los números usando ejemplo de Sebastián
    par_qsort(readNumbers, 0, N-1);
    
    // Escribir los números ordenados en otro archivo
    std::ofstream sortedFile("sorted_numbers_P.csv");
    for (int i = 0; i < N; ++i) {
        sortedFile << readNumbers[i];
        if (i < N - 1) {
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

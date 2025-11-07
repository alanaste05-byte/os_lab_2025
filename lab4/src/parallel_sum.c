#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "utils.h"
#include "sum.h"

int main(int argc, char **argv) {
    uint32_t threads_num = 0;    // Количество потоков
    uint32_t array_size = 0;    // Размер массива
    uint32_t seed = 0;    // Seed для генерации случайных чисел

    // Парсинг аргументов командной строки
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--threads_num") == 0 && i + 1 < argc) {
            threads_num = atoi(argv[++i]);    // Получаем количество потоков
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = atoi(argv[++i]);    // Получаем seed для генератора
        } else if (strcmp(argv[i], "--array_size") == 0 && i + 1 < argc) {
            array_size = atoi(argv[++i]);    // Получаем размер массива
        }
    }

    // Проверка корректности введенных параметров
    if (threads_num == 0 || array_size == 0) {
        printf("Usage: %s --threads_num <num> --seed <num> --array_size <num>\n", argv[0]);
        return 1;
    }

    // Проверка, что количество потоков не превышает размер массива
if (threads_num > array_size) {
    threads_num = array_size; // Уменьшаем количество потоков до размера массива
    printf("Warning: threads_num reduced to array_size (%u)\n", array_size);
}

// Выделение памяти под массив
int *array = malloc(sizeof(int) * array_size);
if (array == NULL) {
    printf("Error: Memory allocation failed!\n");
    return 1;
}

// Генерация массива случайных чисел
GenerateArray(array, array_size, seed);

pthread_t threads[threads_num]; // Массив идентификаторов потоков
struct SumArgs args[threads_num]; // Массив аргументов для потоков

// Вычисление размера части массива для каждого потока
int chunk_size = array_size / threads_num;

// Подготовка аргументов для каждого потока
for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array; // Указатель на массив
    args[i].begin = i * chunk_size; // Начальный индекс
    args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * chunk_size; // Конечный индекс
}

// Начало замера времени (только вычисления суммы)
struct timespec start, end;

clock_gettime(CLOCK_MONOTONIC, &start);

// Создание потоков
for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
        printf("Error: pthread_create failed!\n");
        free(array);
        return 1;
    }
}

// Ожидание завершения потоков и сбор результатов
int total_sum = 0;
for (uint32_t i = 0; i < threads_num; i++) {
    int *sum = 0;
    if (pthread_join(threads[i], (void **)&sum)) {
        printf("Error: pthread_join failed!\n");
    }
    if (sum != NULL) {
    total_sum += *sum;
    free(sum);
    }
}

// Конец замера времени
clock_gettime(CLOCK_MONOTONIC, &end);

// Вычисление затраченного времени в миллисекундах
double elapsed_time = (end.tv_sec - start.tv_sec) * 1000.0;
elapsed_time += (end.tv_nsec - start.tv_nsec) / 1000000.0;

// Освобождение памяти и вывод результатов
free(array);
printf("Total: %d\n", total_sum);
printf("Elapsed time: %.2f ms\n", elapsed_time);

return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Два мьютекса для демонстрации взаимной блокировки (deadlock)
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

// Функция для первого потока
void* thread1_function(void* arg) {
    printf("Thread 1: Trying to lock mutex1...\n");
    pthread_mutex_lock(&mutex1);  // Поток 1 захватывает mutex1
    printf("Thread 1: Locked mutex1\n");
    
    sleep(1);  // Искусственная задержка для создания условия deadlock
    
    printf("Thread 1: Trying to lock mutex2...\n");
    pthread_mutex_lock(&mutex2);  // DEADLOCK! Поток 1 пытается захватить mutex2, но он уже захвачен потоком 2
    printf("Thread 1: Locked mutex2\n");  // Эта строка никогда не выполнится
    
    // Критическая секция (недостижима из-за deadlock)
    printf("Thread 1: Working with both mutexes\n");
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    return NULL;
}

// Функция для второго потока
void* thread2_function(void* arg) {
    printf("Thread 2: Trying to lock mutex2...\n");
    pthread_mutex_lock(&mutex2);  // Поток 2 захватывает mutex2
    printf("Thread 2: Locked mutex2\n");
    
    sleep(1);  // Искусственная задержка для создания условия deadlock
    
    printf("Thread 2: Trying to lock mutex1...\n");
    pthread_mutex_lock(&mutex1);  // DEADLOCK! Поток 2 пытается захватить mutex1, но он уже захвачен потоком 1
    printf("Thread 2: Locked mutex1\n");  // Эта строка никогда не выполнится
    
    // Критическая секция (недостижима из-за deadlock)
    printf("Thread 2: Working with both mutexes\n");
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    
    printf("=== DEADLOCK DEMONSTRATION ===\n");
    printf("This program will demonstrate a classic deadlock scenario.\n");
    printf("Two threads will try to lock two mutexes in different order.\n\n");
    
    // Создаем первый поток
    if (pthread_create(&thread1, NULL, thread1_function, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }
    
    // Создаем второй поток
    if (pthread_create(&thread2, NULL, thread2_function, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }
    
    sleep(3);  // Даем время потокам дойти до состояния deadlock
    
    printf("\nDEADLOCK DETECTED!\n");
    printf("Threads are blocked waiting for each other's mutexes.\n");
    printf("The program is now hung in deadlock state...\n");
    printf("Press Ctrl+C to terminate the program.\n");
    
    // Ожидание завершения потоков (никогда не произойдет из-за deadlock)
    pthread_join(thread1, NULL);  // Программа зависнет здесь
    pthread_join(thread2, NULL);  // Эта строка никогда не выполнится
    
    printf("This message will never be printed due to deadlock!\n");
    return 0;
}
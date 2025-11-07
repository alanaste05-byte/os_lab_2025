#include "sum.h"
#include <pthread.h>
#include <stdlib.h>

int Sum(const struct SumArgs *args) {
    int sum = 0;
    if (args == NULL || args->array == NULL) {
        return 0;
    }

    for (int i = args->begin; i < args->end; i++) {
        sum += args->array[i];
    }
    return sum;
}

void *ThreadSum(void *args) {
    if (args == NULL) {
        return NULL;
    }
    
    struct SumArgs *sum_args = (struct SumArgs *)args;
    int *result = malloc(sizeof(int));
    if (result != NULL) {
        *result = Sum(sum_args);
    }
    return result;
}
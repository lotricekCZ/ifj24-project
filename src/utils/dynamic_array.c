#include "dynamic_array.h"

err_codes dynamic_array_init(dynamic_array_t* array) {
    array->data = (int*)malloc(sizeof(int) * 16);
    if (array->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return err_internal;
    }
    array->size = 0;
    array->capacity = 16;
}

err_codes dynamic_array_insert(dynamic_array_t* array, int value) {
    if (array->size == array->capacity) {
        array->capacity *= 2;
        array->data = (int*)realloc(array->data, sizeof(int) * array->capacity);
        if (array->data == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            return err_internal;
        }
    }
    array->data[array->size++] = value;
}

void dynamic_array_clear(dynamic_array_t* array) {
    array->size = 0;
}

void dynamic_array_destroy(dynamic_array_t* array) {
    if (array->data != NULL) {
        free(array->data);
    }
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
}
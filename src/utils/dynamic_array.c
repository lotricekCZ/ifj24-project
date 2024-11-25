#include "dynamic_array.h"
#include "memory_table.h"

err_codes dynamic_array_init(dynamic_array_t* array) {
    array->data = (int*)imalloc(sizeof(int) * 16);
    if (array->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return err_internal;
    }
    array->size = 0;
    array->capacity = 16;
    return err_none;
}

err_codes dynamic_array_insert(dynamic_array_t* array, int value) {
    if (array->size == array->capacity) {
        array->capacity *= 2;
        array->data = (int*)irealloc(array->data, sizeof(int) * array->capacity);
        if (array->data == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            return err_internal;
        }
    }
    array->data[array->size++] = value;
    return err_none;
}

void dynamic_array_clear(dynamic_array_t* array) {
    array->size = 0;
}

void dynamic_array_destroy(dynamic_array_t* array) {
    if (array->data != NULL) {
        ifree(array->data);
    }
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
}
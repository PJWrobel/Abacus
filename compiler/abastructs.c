#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void** data;
    size_t size;
    size_t capacity;
} Vector;

Vector* vec_new();
void vec_free(Vector* arr);
int vec_push(Vector* arr, void* element);
void* vec_get(Vector* arr, size_t index);
int vec_remove(Vector* arr, size_t index);
int vec_resize(Vector* arr, size_t new_capacity);

// Create a new dynamic array
Vector* vec_new() {
    Vector* arr = malloc(sizeof(Vector));
    if (!arr) {
        fprintf(stderr, "Memory allocation failed for dynamic array\n");
        return NULL;
    }
    
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
    return arr;
}

// Destroy the dynamic array and free memory
void vec_free(Vector* arr) {
    if (!arr) return;
    
    // Free individual elements if needed
    for (size_t i = 0; i < arr->size; i++) {
        free(arr->data[i]);
    }
    
    // Free the data array and the array structure
    free(arr->data);
    free(arr);
}

// Resize the array to a new capacity
int vec_resize(Vector* arr, size_t new_capacity) {
    if (!arr) return 0;
    
    // Allocate new memory block
    void** new_data = realloc(arr->data, new_capacity * sizeof(void*));
    if (!new_data) {
        fprintf(stderr, "Memory reallocation failed\n");
        return 0;
    }
    
    arr->data = new_data;
    arr->capacity = new_capacity;
    
    // Adjust size if new capacity is smaller than current size
    if (arr->size > new_capacity) {
        arr->size = new_capacity;
    }
    
    return 1;
}

int vec_push(Vector* arr, void* element) {
    if (!arr) return 0;
    
    if (arr->size == arr->capacity) {
        size_t new_capacity = arr->capacity == 0 ? 4 : arr->capacity * 2;
        if (!vec_resize(arr, new_capacity)) {
            return 0;
        }
    }
    
    arr->data[arr->size++] = element;
    return 1;
}

void* vec_get(Vector* arr, size_t index) {
    if (!arr || index >= arr->size) {
        return NULL;
    }
    return arr->data[index];
}

int vec_remove(Vector* arr, size_t index) {
    if (!arr || index >= arr->size) {
        return 0;
    }
    
    free(arr->data[index]);
    
    // Shift elements to fill the gap
    for (size_t i = index; i < arr->size - 1; i++) {
        arr->data[i] = arr->data[i + 1];
    }
    
    arr->size--;
    
    // Optional: shrink if size is much smaller than capacity
    if (arr->size < arr->capacity / 4 && arr->capacity > 8) {
        vec_resize(arr, arr->capacity / 2);
    }
    
    return 1;
}

//-------------tests--------------------
int main() {

    Vector* arr = vec_new();
    
    for (int i = 0; i < 10; i++) {
        int* num = malloc(sizeof(int));
        *num = i * 10;
        vec_push(arr, num);
    }
    
    printf("Array contents:\n");
    for (size_t i = 0; i < arr->size; i++) {
        int* val = (int*)vec_get(arr, i);
        printf("%d ", *val);
    }
    printf("\n");
    
    vec_remove(arr, 3);
    
    // Clean up
    vec_free(arr);
    
    return 0;
}

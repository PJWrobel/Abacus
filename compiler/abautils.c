#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

#define i8  int8_t
#define u8  uint8_t
#define i16 int16_t
#define u16 uint16_t
#define i32 int32_t
#define u32 uint32_t
#define i64 int64_t
#define u64 uint64_t

//--------------------------ABA_CMP----------------------------

enum aba_cmp {
    less = -1,
    eq = 0,
    greater = 1
};

#define ABA_CMP(A,B) ( A == B ? eq : A < B ? less : greater )

//----------------------------static pool allocator----------------------------

#include <stddef.h>
#include <string.h>

// Pool allocator structure
typedef struct {
    char* memory;           // Base pointer to memory pool
    size_t total_size;      // Total size of memory pool
    size_t used;           // Amount of memory currently used
    size_t allocations;    // Number of allocations made (for debugging)
} MemPool;

// Initialize memory pool
MemPool* pool_create(size_t size) {
    MemPool* pool = malloc(sizeof(MemPool));
    if (!pool) return NULL;

    pool->memory = malloc(size);
    if (!pool->memory) {
        free(pool);
        return NULL;
    }

    pool->total_size = size;
    pool->used = 0;
    pool->allocations = 0;

    return pool;
}

// Allocate memory from pool
void* pool_alloc(MemPool* pool, size_t size) {
    // Align size to 8 bytes to ensure proper alignment for all types
    size = (size + 7) & ~7;

    // Check if we have enough space
    if (pool->used + size > pool->total_size) {
        return NULL;  // Out of memory
    }

    // Get pointer to allocated memory
    void* ptr = pool->memory + pool->used;
    pool->used += size;
    pool->allocations++;

    return ptr;
}

// Get pool statistics
void pool_stats(MemPool* pool, size_t* used, size_t* total, size_t* count) {
    if (used) *used = pool->used;
    if (total) *total = pool->total_size;
    if (count) *count = pool->allocations;
}

// Cleanup pool (only needed at program end)
void pool_destroy(MemPool* pool) {
    free(pool->memory);
    free(pool);
}

// Optional: Reset pool without freeing memory
void pool_reset(MemPool* pool) {
    pool->used = 0;
    pool->allocations = 0;
}

#ifdef ABAUTILS_H_
#define ABAUTILS_H_

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


//----------------------------static pool allocator-------------------------------

#include <stddef.h>
#include <string.h>

// Pool allocator structure
typedef struct MemPool;

// Initialize memory pool
MemPool* pool_create(size_t size) {
// Allocate memory from pool
void* pool_alloc(MemPool* pool, size_t size) {

// Get pool statistics
void pool_stats(MemPool* pool, size_t* used, size_t* total, size_t* count) {

// Cleanup pool (only needed at program end)
void pool_destroy(MemPool* pool) {
}

// Optional: Reset pool without freeing memory
void pool_reset(MemPool* pool) {

#endif

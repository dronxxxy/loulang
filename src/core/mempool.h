#pragma once

#include <stddef.h>

typedef struct mempool_t mempool_t;

mempool_t *mempool_new_with_capacity(size_t capacity);
mempool_t *mempool_new();
void mempool_free(mempool_t *mempool);

void *mempool_alloc(mempool_t *mempool, size_t size, size_t alignment);

void *mempool_vec_new_with_capacity(mempool_t *mempool, size_t size, size_t capacity);
void *mempool_vec_new(mempool_t *mempool, size_t size);

#define MEMPOOL_ALLOC(MEMPOOL, TYPE) ((TYPE*)mempool_alloc(MEMPOOL, sizeof(TYPE), alignof(TYPE)))
#define MEMPOOL_VEC_NEW(MEMPOOL, TYPE) ((TYPE*)mempool_vec_new(MEMPOOL, sizeof(TYPE)))
#define MEMPOOL_VEC_NEW_WITH_CAPACITY(MEMPOOL, TYPE, CAPACITY) ((TYPE*)mempool_vec_new_with_capacity(MEMPOOL, sizeof(TYPE), CAPACITY))


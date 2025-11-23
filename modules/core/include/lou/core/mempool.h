#pragma once

#include <stddef.h>

typedef struct lou_mempool_t lou_mempool_t;

lou_mempool_t *lou_mempool_new_with_capacity(size_t capacity);
lou_mempool_t *lou_mempool_new();
void lou_mempool_free(lou_mempool_t *mempool);

void *lou_mempool_alloc(lou_mempool_t *mempool, size_t size, size_t alignment);

void *lou_mempool_vec_new_with_capacity(lou_mempool_t *mempool, size_t size, size_t capacity);
void *lou_mempool_vec_new(lou_mempool_t *mempool, size_t size);

#define LOU_MEMPOOL_ALLOC(MEMPOOL, TYPE) ((TYPE*)lou_mempool_alloc(MEMPOOL, sizeof(TYPE), alignof(TYPE)))
#define LOU_MEMPOOL_VEC_NEW(MEMPOOL, TYPE) ((TYPE*)lou_mempool_vec_new(MEMPOOL, sizeof(TYPE)))
#define LOU_MEMPOOL_VEC_NEW_WITH_CAPACITY(MEMPOOL, TYPE, CAPACITY) ((TYPE*)lou_mempool_vec_new_with_capacity(MEMPOOL, sizeof(TYPE), CAPACITY))


#include "lou/core/mempool.h"
#include "lou/core/arena.h"
#include "lou/core/vec.h"
#include <stdlib.h>
#include <string.h>

typedef struct lou_mempool_t {
  void **vecs;
  lou_arena_t *arena;
} lou_mempool_t;

lou_mempool_t *lou_mempool_new_with_capacity(size_t capacity) {
  lou_mempool_t *mempool = malloc(sizeof(lou_mempool_t));
  mempool->arena = lou_arena_new(capacity);
  mempool->vecs = LOU_VEC_NEW(void*);
  return mempool;
}

lou_mempool_t *lou_mempool_new() {
  // TODO: select best capacity
  return lou_mempool_new_with_capacity(128);
}

void *lou_mempool_alloc(lou_mempool_t *mempool, size_t size, size_t alignment) {
  return lou_arena_alloc(mempool->arena, size, alignment);
}

char *lou_mempool_slice_to_cstr(lou_mempool_t *mempool, lou_slice_t slice) {
  char *result = lou_mempool_alloc(mempool, slice.length + 1, 1);
  memcpy(result, slice.ptr, slice.length);
  result[slice.length] = '\0';
  return result;
}

static inline void lou_mempool_relink(lou_mempool_t *mempool) {
  for (size_t i = 0; i < lou_vec_len(mempool->vecs); i++) {
    lou_vec_link(mempool->vecs[i], lou_vec_at(mempool->vecs, i));
  }
}

static inline void *lou_mempool_linked_vec(lou_mempool_t *mempool, void *vec) {
  void **vecs = mempool->vecs;
  void **link = LOU_VEC_PUSH(&mempool->vecs);
  *link = vec;
  lou_vec_link(vec, link);
  if (vecs != mempool->vecs) {
    lou_mempool_relink(mempool);
  }
  return vec;
}

void *lou_mempool_vec_new_with_capacity(lou_mempool_t *mempool, size_t size, size_t capacity) {
  return lou_mempool_linked_vec(mempool, lou_vec_new_with_capacity(size, capacity));
}

void *lou_mempool_vec_new(lou_mempool_t *mempool, size_t size) {
  return lou_mempool_linked_vec(mempool, lou_vec_new(size));
}

void lou_mempool_free(lou_mempool_t *mempool) {
  for (size_t i = 0; i < lou_vec_len(mempool->vecs); i++) {
    void *vec = mempool->vecs[i];
    if (vec) lou_vec_free(vec);
  }
  lou_vec_free(mempool->vecs);
  lou_arena_free(mempool->arena);
  free(mempool);
}


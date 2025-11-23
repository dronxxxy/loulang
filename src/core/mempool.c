#include "mempool.h"
#include "core/arena.h"
#include "core/vec.h"
#include <stdlib.h>

typedef struct mempool_t {
  void **vecs;
  arena_t *arena;
} mempool_t;

mempool_t *mempool_new_with_capacity(size_t capacity) {
  mempool_t *mempool = malloc(sizeof(mempool_t));
  mempool->arena = arena_new(capacity);
  mempool->vecs = VEC_NEW(void*);
  return mempool;
}

mempool_t *mempool_new() {
  // TODO: select best capacity
  return mempool_new_with_capacity(128);
}

void *mempool_alloc(mempool_t *mempool, size_t size, size_t alignment) {
  return arena_alloc(mempool->arena, size, alignment);
}

static inline void mempool_relink(mempool_t *mempool) {
  for (size_t i = 0; i < vec_len(mempool->vecs); i++) {
    vec_link(mempool->vecs[i], vec_at(mempool->vecs, i));
  }
}

static inline void *mempool_linked_vec(mempool_t *mempool, void *vec) {
  void **vecs = mempool->vecs;
  void **link = VEC_PUSH(&mempool->vecs);
  *link = vec;
  vec_link(vec, link);
  if (vecs != mempool->vecs) {
    mempool_relink(mempool);
  }
  return vec;
}

void *mempool_vec_new_with_capacity(mempool_t *mempool, size_t size, size_t capacity) {
  return mempool_linked_vec(mempool, vec_new_with_capacity(size, capacity));
}

void *mempool_vec_new(mempool_t *mempool, size_t size) {
  return mempool_linked_vec(mempool, vec_new(size));
}

void mempool_free(mempool_t *mempool) {
  for (size_t i = 0; i < vec_len(mempool->vecs); i++) {
    void *vec = mempool->vecs[i];
    if (vec) vec_free(vec);
  }
  vec_free(mempool->vecs);
  arena_free(mempool->arena);
  free(mempool);
}


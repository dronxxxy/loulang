#include "arena.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct arena_t {
  size_t capacity;
  size_t top;
  arena_t *next;
} arena_t;

arena_t *arena_new(size_t capacity) {
  arena_t *arena = malloc(sizeof(arena_t) + capacity);
  arena->capacity = capacity;
  arena->top = 0;
  arena->next = NULL;
  return arena;
}

void arena_free(arena_t *arena) {
  if (arena->next) {
    arena_free(arena->next);
  }
  free(arena);
}

static inline size_t arena_align_factor(const arena_t *arena, size_t alignment) {
  ptrdiff_t ptr = (ptrdiff_t)&((char*)(&arena[1]))[arena->top];
  if (ptr % alignment == 0) return 0;
  return alignment - ptr % alignment;
}

static inline size_t arena_aligned_top_idx(const arena_t *arena, size_t alignment) {
  return arena->top + arena_align_factor(arena, alignment);
}

static inline void *arena_top(const arena_t *arena, size_t alignment) {
  return &((char*)(&arena[1]))[arena_aligned_top_idx(arena, alignment)];
}

static inline size_t arena_remains(const arena_t *arena, size_t alignment) {
  return arena->capacity - arena_aligned_top_idx(arena, alignment);
}

void *arena_alloc(arena_t *arena, size_t size, size_t alignment) {
  for (arena_t *a = arena; a; a = a->next) {
    if (arena_remains(a, alignment) >= size) {
      void *top = arena_top(a, alignment);
      a->top = arena_aligned_top_idx(a, alignment) + size;
      return top;
    }
    arena = a;
  }

  size_t new_arena_cap = size + alignment;
  if (new_arena_cap < arena->capacity) {
    new_arena_cap = arena->capacity;
  }
  assert(!arena->next);
  arena->next = arena_new(new_arena_cap);
  return arena_alloc(arena->next, size, alignment);
}


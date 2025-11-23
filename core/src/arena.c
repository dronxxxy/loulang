#include "lou/core/arena.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct lou_arena_t {
  size_t capacity;
  size_t top;
  lou_arena_t *next;
} lou_arena_t;

lou_arena_t *lou_arena_new(size_t capacity) {
  lou_arena_t *arena = malloc(sizeof(lou_arena_t) + capacity);
  arena->capacity = capacity;
  arena->top = 0;
  arena->next = NULL;
  return arena;
}

void lou_arena_free(lou_arena_t *arena) {
  if (arena->next) {
    lou_arena_free(arena->next);
  }
  free(arena);
}

static inline size_t arena_align_factor(const lou_arena_t *arena, size_t alignment) {
  ptrdiff_t ptr = (ptrdiff_t)&((char*)(&arena[1]))[arena->top];
  if (ptr % alignment == 0) return 0;
  return alignment - ptr % alignment;
}

static inline size_t lou_arena_aligned_top_idx(const lou_arena_t *arena, size_t alignment) {
  return arena->top + arena_align_factor(arena, alignment);
}

static inline void *lou_arena_top(const lou_arena_t *arena, size_t alignment) {
  return &((char*)(&arena[1]))[lou_arena_aligned_top_idx(arena, alignment)];
}

static inline size_t lou_arena_remains(const lou_arena_t *arena, size_t alignment) {
  return arena->capacity - lou_arena_aligned_top_idx(arena, alignment);
}

void *lou_arena_alloc(lou_arena_t *arena, size_t size, size_t alignment) {
  for (lou_arena_t *a = arena; a; a = a->next) {
    if (lou_arena_remains(a, alignment) >= size) {
      void *top = lou_arena_top(a, alignment);
      a->top = lou_arena_aligned_top_idx(a, alignment) + size;
      return top;
    }
    arena = a;
  }

  size_t new_arena_cap = size + alignment;
  if (new_arena_cap < arena->capacity) {
    new_arena_cap = arena->capacity;
  }
  assert(!arena->next);
  arena->next = lou_arena_new(new_arena_cap);
  return lou_arena_alloc(arena->next, size, alignment);
}


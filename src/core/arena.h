#pragma once

#include <stddef.h>

typedef struct arena_t arena_t;

arena_t *arena_new(size_t capacity);
void arena_free(arena_t *arena);

void *arena_alloc(arena_t *arena, size_t size, size_t alignment);

#define ARENA_ALLOC(ARENA, TYPE) ((TYPE*)arena_alloc(ARENA, sizeof(TYPE), alignof(TYPE)))


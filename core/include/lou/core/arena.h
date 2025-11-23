#pragma once

#include <stddef.h>

typedef struct lou_arena_t lou_arena_t;

lou_arena_t *lou_arena_new(size_t capacity);
void lou_arena_free(lou_arena_t *arena);

void *lou_arena_alloc(lou_arena_t *arena, size_t size, size_t alignment);

#define LOU_ARENA_ALLOC(ARENA, TYPE) ((TYPE*)arena_alloc(ARENA, sizeof(TYPE), alignof(TYPE)))


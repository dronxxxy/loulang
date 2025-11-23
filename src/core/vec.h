#pragma once

#include <stddef.h>

void *vec_new_with_capacity(size_t size, size_t capacity);
void *vec_new(size_t size);
void vec_free(void *vec);

void vec_reserve(void **vec, size_t capacity);
void *vec_push(void **vec);
void vec_pop(void **vec);

size_t vec_len(const void *vec);
void *vec_at(void *vec, size_t id);
void *vec_last(void *vec);

void vec_link(void *vec, void **link);
void vec_unlink(void *vec);

#define VEC_NEW_WITH_CAPACITY(TYPE, CAPACITY) ((TYPE*)vec_new_with_capacity(sizeof(TYPE), CAPACITY))
#define VEC_NEW(TYPE) ((TYPE*)vec_new(sizeof(TYPE)))
#define VEC_RESERVE(VEC, CAPACITY) vec_reserve((void**)(VEC), CAPACITY)
#define VEC_PUSH(VEC) ((typeof(*(VEC)))vec_push((void**)(VEC)))
#define VEC_POP(VEC) (vec_pop((void**)(VEC)))
#define VEC_LINK(VEC, LINK) (vec_link(VEC, (void**)(LINK)))

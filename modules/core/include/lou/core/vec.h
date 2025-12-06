#pragma once

#include <stdbool.h>
#include <stddef.h>

void *lou_vec_new_with_capacity(size_t size, size_t capacity);
void *lou_vec_new(size_t size);
void lou_vec_free(void *vec);

void lou_vec_reserve(void **vec, size_t capacity);
void *lou_vec_push(void **vec);
void lou_vec_pop(void **vec);
void lou_vec_unshift(void **vec);

size_t lou_vec_len(const void *vec);
void *lou_vec_at(void *vec, size_t id);
void *lou_vec_last(void *vec);

typedef bool (*lou_veq_func_t)(const void *a, const void *b);
bool lou_vec_eq_ptr(const void *a, const void *b, lou_veq_func_t eq);
bool lou_vec_eq(const void *a, const void *b, lou_veq_func_t eq);

void lou_vec_link(void *vec, void **link);
void lou_vec_unlink(void *vec);

#define LOU_VEC_NEW_WITH_CAPACITY(TYPE, CAPACITY) ((TYPE*)lou_vec_new_with_capacity(sizeof(TYPE), CAPACITY))
#define LOU_VEC_NEW(TYPE) ((TYPE*)lou_vec_new(sizeof(TYPE)))
#define LOU_VEC_RESERVE(VEC, CAPACITY) lou_vec_reserve((void**)(VEC), CAPACITY)
#define LOU_VEC_PUSH(VEC) ((typeof(*(VEC)))lou_vec_push((void**)(VEC)))
#define LOU_VEC_POP(VEC) (lou_vec_pop((void**)(VEC)))
#define LOU_VEC_LAST(VEC) ((typeof(VEC))lou_vec_last((void*)(VEC)))
#define LOU_VEC_UNSHIFT(VEC) (lou_vec_unshift((void**)(VEC)))
#define LOU_VEC_LINK(VEC, LINK) (lou_vec_link(VEC, (void**)(LINK)))

#include "lou/core/vec.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t size;
  size_t length;
  size_t capacity;
  void **link;
} lou_vec_header_t;

static inline lou_vec_header_t *lou_vec_header(const void *vec) {
  return &((lou_vec_header_t*)vec)[-1];
}

static inline size_t lou_vec_size(size_t size, size_t capacity) {
  return sizeof(lou_vec_header_t) + capacity * size;
}

void *lou_vec_new_with_capacity(size_t size, size_t capacity) {
  lou_vec_header_t *header = malloc(lou_vec_size(size, capacity));
  header->size = size;
  header->length = 0;
  header->capacity = capacity;
  header->link = NULL;
  return &header[1];
}

void *lou_vec_new(size_t size) {
  return lou_vec_new_with_capacity(size, 2);
}

void lou_vec_free(void *vec) {
  lou_vec_header_t *header = lou_vec_header(vec);
  lou_vec_unlink(vec);
  free(header);
}

void lou_vec_reserve(void **vec, size_t capacity) {
  lou_vec_header_t *header = lou_vec_header(*vec);
  if (header->capacity >= capacity) return;
  header = realloc(header, lou_vec_size(header->size, capacity));
  *vec = &header[1];
}

inline size_t lou_vec_len(const void *vec) {
  return lou_vec_header(vec)->length;
}

inline void *lou_vec_at(void *vec, size_t id) {
  lou_vec_header_t *header = lou_vec_header(vec);
  return &((char*)vec)[header->size * id];
}

void *lou_vec_last(void *vec) {
  return lou_vec_at(vec, lou_vec_len(vec) - 1);
}

bool lou_vec_eq(const void *a, const void *b, lou_veq_func_t eq) {
  if (lou_vec_len(a) != lou_vec_len(b)) {
    return false;
  }
  for (size_t i = 0; i < lou_vec_len(a); i++) {
    if (!eq(*(void**)lou_vec_at((void*)a, i), *(void**)lou_vec_at((void*)b, i))) {
      return false;
    }
  }
  return false;
}

bool lou_vec_eq_ptr(const void *a, const void *b, lou_veq_func_t eq) {
  if (lou_vec_len(a) != lou_vec_len(b)) {
    return false;
  }
  for (size_t i = 0; i < lou_vec_len(a); i++) {
    if (!eq(lou_vec_at((void*)a, i), lou_vec_at((void*)b, i))) {
      return false;
    }
  }
  return false;
}

void *lou_vec_push(void **vec) {
  lou_vec_header_t *header = lou_vec_header(*vec);
  void *old = vec;
  lou_vec_reserve(vec, header->length + 1);
  header = lou_vec_header(*vec);
  header->length++;
  if (header->link && old != *vec) {
    *header->link = *vec;
  }
  return lou_vec_at(*vec, lou_vec_len(*vec) - 1);
}


void lou_vec_append_raw(void **vec, const void *other, size_t count) {
  lou_vec_reserve(vec, lou_vec_len(*vec) + count);
  size_t size = lou_vec_header(*vec)->size;
  for (size_t i = 0; i < count; i++) {
    memcpy(lou_vec_push(vec), (char*)other + size * i, size);
  }
}

void lou_vec_append(void **vec, const void *other) {
  assert(lou_vec_header(*vec)->size == lou_vec_header(other)->size);
  lou_vec_append_raw(vec, other, lou_vec_len(other));
}

void lou_vec_pop(void **vec) {
  lou_vec_header(*vec)->length--;
}

void lou_vec_unshift(void **vec) {
  lou_vec_header_t *header = lou_vec_header(*vec);
  memmove(*vec, lou_vec_at(*vec, 1), (header->length - 1) * header->size);
  lou_vec_pop(vec);
}

void lou_vec_unlink(void *vec) {
  lou_vec_header_t *header = lou_vec_header(vec);
  if (header->link) *header->link = NULL;
}

void lou_vec_link(void *vec, void **link) {
  lou_vec_header_t *header = lou_vec_header(vec);
  header->link = link;
}


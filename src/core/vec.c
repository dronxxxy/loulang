#include "vec.h"
#include <stdlib.h>

typedef struct {
  size_t size;
  size_t length;
  size_t capacity;
  void **link;
} vec_header_t;

static inline vec_header_t *vec_header(const void *vec) {
  return &((vec_header_t*)vec)[-1];
}

static inline size_t vec_size(size_t size, size_t capacity) {
  return sizeof(vec_header_t) + capacity * size;
}

void *vec_new_with_capacity(size_t size, size_t capacity) {
  vec_header_t *header = malloc(vec_size(size, capacity));
  header->size = size;
  header->length = 0;
  header->capacity = capacity;
  header->link = NULL;
  return &header[1];
}

void *vec_new(size_t size) {
  return vec_new_with_capacity(size, 2);
}

void vec_free(void *vec) {
  vec_header_t *header = vec_header(vec);
  vec_unlink(vec);
  free(header);
}

void vec_reserve(void **vec, size_t capacity) {
  vec_header_t *header = vec_header(*vec);
  if (header->capacity >= capacity) return;
  header = realloc(header, vec_size(header->size, capacity));
  *vec = &header[1];
}

inline size_t vec_len(const void *vec) {
  return vec_header(vec)->length;
}

inline void *vec_at(void *vec, size_t id) {
  vec_header_t *header = vec_header(vec);
  return &((char*)vec)[header->size * id];
}

void *vec_last(void *vec) {
  return vec_at(vec, vec_len(vec) - 1);
}

void *vec_push(void **vec) {
  vec_header_t *header = vec_header(*vec);
  void *old = vec;
  vec_reserve(vec, header->length + 1);
  header = vec_header(*vec);
  header->length++;
  if (header->link && old != *vec) {
    *header->link = *vec;
  }
  return vec_at(*vec, vec_len(*vec) - 1);
}

void vec_pop(void **vec) {
  vec_header(*vec)->length--;
}

void vec_unlink(void *vec) {
  vec_header_t *header = vec_header(vec);
  if (header->link) *header->link = NULL;
}

void vec_link(void *vec, void **link) {
  vec_header_t *header = vec_header(vec);
  header->link = link;
}


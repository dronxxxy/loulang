#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  const char *ptr;
  size_t length;
} lou_slice_t;

static inline lou_slice_t lou_slice_new(const char *ptr, size_t length) {
  return (lou_slice_t) {
    .ptr = ptr,
    .length = length,
  };
}

lou_slice_t lou_slice_from_cstr(const char *cstr);
lou_slice_t lou_subslice(lou_slice_t slice, size_t begin, size_t end);
lou_slice_t lou_slice_union(lou_slice_t a, lou_slice_t b);
bool lou_slice_eq(lou_slice_t a, lou_slice_t b);

typedef struct {
  bool has_value;
  lou_slice_t value;
} lou_opt_slice_t;

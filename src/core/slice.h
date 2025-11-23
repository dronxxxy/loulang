#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  const char *ptr;
  size_t length;
} slice_t;

static inline slice_t slice_new(const char *ptr, size_t length) {
  return (slice_t) {
    .ptr = ptr,
    .length = length,
  };
}

slice_t slice_from_cstr(const char *cstr);
slice_t subslice(slice_t slice, size_t begin, size_t end);

typedef struct {
  bool has_value;
  slice_t value;
} opt_slice_t;

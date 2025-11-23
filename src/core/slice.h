#pragma once

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

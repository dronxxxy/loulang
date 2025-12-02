#include "lou/core/slice.h"
#include <string.h>

lou_slice_t lou_slice_from_cstr(const char *cstr) {
  return lou_slice_new(cstr, strlen(cstr));
}

lou_slice_t lou_subslice(lou_slice_t slice, size_t begin, size_t end) {
  if (end > slice.length) {
    end = slice.length;
  }
  return lou_slice_new(&slice.ptr[begin], end - begin);
}

bool lou_slice_eq(lou_slice_t a, lou_slice_t b) {
  if (a.length != b.length) {
    return false;
  }
  return !memcmp(a.ptr, b.ptr, a.length);
}

lou_slice_t lou_slice_union(lou_slice_t a, lou_slice_t b) {
  if (a.ptr > b.ptr) {
    lou_slice_t temp = a;
    a = b;
    b = temp;
  }
  size_t length = b.ptr - a.ptr + b.length;
  if (length < a.length) {
    length = a.length;
  }
  return (lou_slice_t) {
    .ptr = a.ptr,
    .length = length
  };
}

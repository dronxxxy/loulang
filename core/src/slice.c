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


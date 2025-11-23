#include "slice.h"
#include <string.h>

slice_t slice_from_cstr(const char *cstr) {
  return slice_new(cstr, strlen(cstr));
}

slice_t subslice(slice_t slice, size_t begin, size_t end) {
  if (end > slice.length) {
    end = slice.length;
  }
  return slice_new(&slice.ptr[begin], end - begin);
}


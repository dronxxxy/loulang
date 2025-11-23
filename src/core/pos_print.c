#include "pos_print.h"
#include <assert.h>
#include <stdio.h>

void pos_print(FILE *stream, slice_t file, slice_t part) {
  assert(part.ptr >= file.ptr);
  assert(part.ptr + part.length <= file.ptr + file.length);

  size_t start = 0;
  size_t highlight_begin = part.ptr - file.ptr;
  size_t highlight_end = highlight_begin + part.length;
  size_t end = file.length;

  for (size_t i = 0; i < file.length; i++) {
    if (file.ptr[i] != '\n') {
      continue;
    }
    if (i <= highlight_begin + 1) {
      start = i + 1;
    }
    if (end == file.length && i >= highlight_end) {
      end = i;
    }
  }

  assert(start <= highlight_begin);
  assert(highlight_begin <= highlight_end);
  assert(highlight_end <= end);

  fwrite(file.ptr + start, 1, highlight_begin - start, stream);
  fprintf(stream, "\033[31m");
  fwrite(file.ptr + highlight_begin, 1, highlight_end - highlight_begin, stream);
  fprintf(stream, "\033[0m");
  fwrite(file.ptr + highlight_end, 1, end - highlight_end, stream);
}


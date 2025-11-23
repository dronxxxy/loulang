#include "pos_print.h"
#include <assert.h>
#include <stdio.h>

#define START_LINE_FROM 0
#define START_COLUMN_FROM 1

static inline void pos_print_content(FILE *stream, const char *ptr, size_t length) {
  for (size_t i = 0; i < length; i++) {
    fputc(ptr[i], stream);
    if (ptr[i] == '\n') {
      fprintf(stream, "| ");
    }
  }
}

void pos_print(FILE *stream, slice_t path, slice_t file, slice_t part) {
  assert(part.ptr >= file.ptr);
  assert(part.ptr + part.length <= file.ptr + file.length);

  size_t line = START_LINE_FROM;
  size_t column = START_COLUMN_FROM - 1;

  size_t start = 0;
  size_t highlight_begin = part.ptr - file.ptr;
  size_t highlight_end = highlight_begin + part.length;
  size_t end = file.length;

  for (size_t i = 0; i < file.length; i++) {
    if (i <= highlight_begin) {
      column++;
    }
    if (file.ptr[i] != '\n') {
      continue;
    }

    if (i <= highlight_begin) {
      line++;
      column = START_COLUMN_FROM - 1;
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

  fprintf(stderr, "at ");
  fwrite(path.ptr, 1, path.length, stderr);
  fprintf(stderr, ":%lu:%lu\n", line, column);
  fprintf(stream, "| ");
  pos_print_content(stream, file.ptr + start, highlight_begin - start);
  fprintf(stream, "\033[31m");
  pos_print_content(stream, file.ptr + highlight_begin, highlight_end - highlight_begin);
  fprintf(stream, "\033[0m");
  pos_print_content(stream, file.ptr + highlight_end, end - highlight_end);
}


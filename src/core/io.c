#include "io.h"
#include "core/slice.h"

bool file_read_all(mempool_t *mempool, FILE *stream, slice_t *output) {
  fseek(stream, 0, SEEK_END);  
  size_t length = ftell(stream);
  fseek(stream, 0, SEEK_SET);  
  char *ptr = mempool_alloc(mempool, length, 1);
  if (fread(ptr, 1, length, stream) != length) {
    return false;
  }
  *output = slice_new(ptr, length);
  return true;
}

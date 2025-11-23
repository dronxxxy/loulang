#include "lou/core/io.h"
#include "lou/core/slice.h"

bool lou_file_read_all(lou_mempool_t *mempool, FILE *stream, lou_slice_t *output) {
  fseek(stream, 0, SEEK_END);  
  size_t length = ftell(stream);
  fseek(stream, 0, SEEK_SET);  
  char *ptr = lou_mempool_alloc(mempool, length, 1);
  if (fread(ptr, 1, length, stream) != length) {
    return false;
  }
  *output = lou_slice_new(ptr, length);
  return true;
}

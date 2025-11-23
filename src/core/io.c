#include "io.h"

bool file_read_all(mempool_t *mempool, FILE *stream, slice_t *output) {
  fseek(stream, 0, SEEK_END);  
  output->length = ftell(stream);
  fseek(stream, 0, SEEK_SET);  
  output->ptr = mempool_alloc(mempool, output->length, 1);
  if (fread(output->ptr, 1, output->length, stream) != output->length) {
    return false;
  }
  return true;
}

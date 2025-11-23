#include "core/log.h"
#include "core/mempool.h"
#include "core/vec.h"
#include <stdio.h>

int main(int argc, char** argv) {
  mempool_t *mempool = mempool_new();
  for (size_t i = 0; i < 100; i++) {
    size_t *a = MEMPOOL_ALLOC(mempool, size_t);
    size_t *b = MEMPOOL_VEC_NEW(mempool, size_t);
    for (size_t j = 0; j < 100; j++) { *VEC_PUSH(&b) = 10; }
  }
  mempool_free(mempool);
  log_fmt(LOG_ERROR, "hello, how are you?");
  return 0;
}

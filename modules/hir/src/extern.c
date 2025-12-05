#include "extern.h"
#include "lou/core/mempool.h"

lou_hir_extern_t *lou_hir_extern_new(lou_mempool_t *mempool, lou_hir_type_t *type, lou_slice_t name) {
  lou_hir_extern_t *external = LOU_MEMPOOL_ALLOC(mempool, lou_hir_extern_t);
  external->name = name;
  external->type = type;
  return external;
}

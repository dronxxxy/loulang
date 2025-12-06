#include "const.h"

lou_sema_const_t *lou_sema_const_new_int(lou_mempool_t *mempool, lou_sema_type_t *type, uint64_t value) {
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_INTEGER;
  constant->type = type;
  constant->integer = value;
  return constant;
}

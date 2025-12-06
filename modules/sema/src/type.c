#include "type.h"
#include "lou/core/assertions.h"

lou_sema_type_t *lou_sema_type_new_int(lou_mempool_t *mempool, lou_sema_int_size_t size, bool is_signed);

bool lou_sema_type_eq(const lou_sema_type_t *a, const lou_sema_type_t *b) {
  if (a == b) return true;
  if ((a == NULL) != (b == NULL)) return false;
  if (a->kind != b->kind) return false;
  switch (a->kind) {
    case LOU_SEMA_TYPE_INTEGER: return a->integer.size == b->integer.size && a->integer.is_signed == b->integer.is_signed;
  }
  UNREACHABLE();
}


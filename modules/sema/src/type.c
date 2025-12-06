#include "type.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include <assert.h>

lou_sema_type_t *lou_sema_type_new_func(lou_mempool_t *mempool) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->complete = false;
  type->kind = LOU_SEMA_TYPE_FUNCTION;
  return type;
}

lou_sema_type_t *lou_sema_type_new_int(lou_mempool_t *mempool, lou_sema_int_size_t size, bool is_signed) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->complete = true;
  type->kind = LOU_SEMA_TYPE_INTEGER;
  type->integer.size = size;
  type->integer.is_signed = is_signed;
  return type;
}

void lou_sema_type_init_func(lou_sema_type_t *type, lou_sema_type_t **args, lou_sema_type_t *returns) {
  type->complete = true;
  type->func.args = args;
  type->func.returns = returns;
}

bool lou_sema_type_eq(const lou_sema_type_t *a, const lou_sema_type_t *b) {
  assert(!a || a->complete);
  assert(!b || b->complete);
  if (a == b) return true;
  if ((a == NULL) != (b == NULL)) return false;
  if (a->kind != b->kind) return false;
  switch (a->kind) {
    case LOU_SEMA_TYPE_INTEGER: return a->integer.size == b->integer.size && a->integer.is_signed == b->integer.is_signed;
    case LOU_SEMA_TYPE_FUNCTION:
      return ((a->func.returns == NULL) == (b->func.returns == NULL)) &&
        (a->func.returns == NULL || lou_sema_type_eq(a->func.returns, b->func.returns)) &&
        lou_vec_eq(a->func.args, b->func.args, (lou_veq_func_t)lou_sema_type_eq);
  }
  UNREACHABLE();
}


#include "type.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/hir/type.h"
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

lou_sema_type_t *lou_sema_type_new_string(lou_mempool_t *mempool) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->complete = true;
  type->kind = LOU_SEMA_TYPE_STRING;
  return type;
}

void lou_sema_type_init_func(lou_sema_type_t *type, lou_sema_type_t **args, lou_sema_type_t *returns) {
  assert(type->kind == LOU_SEMA_TYPE_FUNCTION);
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
    case LOU_SEMA_TYPE_STRING: return true;
    case LOU_SEMA_TYPE_FUNCTION:
      return ((a->func.returns == NULL) == (b->func.returns == NULL)) &&
        (a->func.returns == NULL || lou_sema_type_eq(a->func.returns, b->func.returns)) &&
        lou_vec_eq(a->func.args, b->func.args, (lou_veq_func_t)lou_sema_type_eq);
  }
  UNREACHABLE();
}

static inline lou_hir_int_size_t lou_sema_int_size_as_hir(lou_sema_int_size_t size) {
  switch (size) {
    case LOU_SEMA_INT_8: return LOU_HIR_INT_8;
    case LOU_SEMA_INT_16: return LOU_HIR_INT_16;
    case LOU_SEMA_INT_32: return LOU_HIR_INT_32;
    case LOU_SEMA_INT_64: return LOU_HIR_INT_64;
  }
  UNREACHABLE();
}

lou_hir_type_t *lou_sema_type_as_hir(lou_mempool_t *mempool, const lou_sema_type_t *type) {
  assert(type->complete);
  switch (type->kind) {
    case LOU_SEMA_TYPE_STRING: UNREACHABLE();
    case LOU_SEMA_TYPE_INTEGER: return lou_hir_type_new_integer(mempool, lou_sema_int_size_as_hir(type->integer.size), type->integer.is_signed);
    case LOU_SEMA_TYPE_FUNCTION: {
      lou_hir_type_t **args = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_type_t*);
      for (size_t i = 0; i < lou_vec_len(type->func.args); i++) {
        *LOU_VEC_PUSH(&args) = lou_sema_type_as_hir(mempool, type->func.args[i]);
      }
      lou_hir_type_t *returns = type->func.returns ? lou_sema_type_as_hir(mempool, type->func.returns) : NULL;
      return lou_hir_type_new_func(mempool, args, returns);
    }
  }
  UNREACHABLE();
}

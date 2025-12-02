#include "lou/hir/type.h"

lou_hir_type_t *lou_hir_type_new_func(lou_mempool_t *mempool, lou_hir_type_t **args, lou_hir_type_t *returns)  {
  lou_hir_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_hir_type_t);
  type->kind = LOU_HIR_TYPE_FUNC;
  type->func.args = args;
  type->func.returns = returns;
  return type;
}

lou_hir_type_t *lou_hir_type_new_integer(lou_mempool_t *mempool, lou_hir_int_size_t size, bool is_signed)  {
  lou_hir_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_hir_type_t);
  type->kind = LOU_HIR_TYPE_INT;
  type->integer.size = size;
  type->integer.is_signed = is_signed;
  return type;
}


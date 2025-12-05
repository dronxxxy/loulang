#include "lou/hir/value.h"
#include "lou/core/assertions.h"
#include "lou/hir/code.h"
#include "lou/hir/func.h"

lou_hir_type_t *lou_hir_value_typeof(lou_hir_value_t *value) {
  switch (value->kind) {
    case LOU_HIR_VALUE_CONST: return value->constant->type;
    case LOU_HIR_VALUE_LOCAL: return value->local->type;
    case LOU_HIR_VALUE_FUNCTION: return value->func->type;
  }
  UNREACHABLE();
}

lou_hir_mutability_t lou_hir_value_mutability(lou_hir_value_t *value) {
  switch (value->kind) {
    case LOU_HIR_VALUE_FUNCTION: case LOU_HIR_VALUE_CONST: return LOU_HIR_IMMUTABLE;
    case LOU_HIR_VALUE_LOCAL: return value->local->mutability;
  }
  UNREACHABLE();
}

lou_hir_value_t *lou_hir_value_new_const(lou_mempool_t *mempool, lou_hir_const_t *constant) {
  lou_hir_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_hir_value_t);
  value->kind = LOU_HIR_VALUE_CONST;
  value->constant = constant;
  return value;
}

lou_hir_value_t *lou_hir_value_new_local(lou_mempool_t *mempool, lou_hir_local_t *local) {
  lou_hir_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_hir_value_t);
  value->kind = LOU_HIR_VALUE_LOCAL;
  value->local = local;
  return value;
}

lou_hir_value_t *lou_hir_value_new_func(lou_mempool_t *mempool, lou_hir_func_t *func) {
  lou_hir_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_hir_value_t);
  value->kind = LOU_HIR_VALUE_FUNCTION;
  value->func = func;
  return value;
}

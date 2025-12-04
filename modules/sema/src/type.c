#include "type.h"
#include "lou/core/assertions.h"
#include "lou/core/log.h"
#include "lou/core/vec.h"
#include <stdio.h>

void lou_sema_type_print(FILE *stream, lou_sema_type_t *type) {
  if (!type) {
    fprintf(stream, "nothing");
    return;
  }
  switch (type->kind) {
    case LOU_SEMA_TYPE_INTEGER:
      fprintf(stream, type->integer.is_signed ? "i" : "u");
      switch (type->integer.size) {
        case LOU_SEMA_INT_8: fprintf(stream, "8"); return;
        case LOU_SEMA_INT_16: fprintf(stream, "16"); return;
        case LOU_SEMA_INT_32: fprintf(stream, "32"); return;
        case LOU_SEMA_INT_64: fprintf(stream, "64"); return;
      }
      break;
    case LOU_SEMA_TYPE_FUNCTION:
      fprintf(stream, "@fun((");
      for (size_t i = 0; i < lou_vec_len(type->func.args); i++) {
        lou_log_puts(stream, i == 0 ? "#T" : ", #T", type->func.args[i]);
      }
      fprintf(stream, ")");
      if (type->func.returns) {
        lou_log_puts(stream, ", #T", type->func.returns);
      }
      fprintf(stream, ")");
      return;
    case LOU_SEMA_TYPE_STRING:
      fprintf(stream, "string");
      return;
    case LOU_SEMA_TYPE_POINTER:
      lou_log_puts(stream, "@ptr(#T)", type->pointer_to);
      return;
  }
  UNREACHABLE();
}

lou_sema_type_t *lou_sema_type_new_string(lou_mempool_t *mempool) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->kind = LOU_SEMA_TYPE_STRING;
  return type;
}

lou_sema_type_t *lou_sema_type_new_function(lou_mempool_t *mempool, lou_sema_type_func_arg_t *args, lou_sema_type_t *returns) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->kind = LOU_SEMA_TYPE_FUNCTION;
  type->func.args = args;
  type->func.returns = returns;
  return type;
}

lou_sema_type_t *lou_sema_type_new_pointer(lou_mempool_t *mempool, lou_sema_type_t *to) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->kind = LOU_SEMA_TYPE_POINTER;
  type->pointer_to = to;
  return type;
}

lou_sema_type_t *lou_sema_type_new_integer(lou_mempool_t *mempool, lou_sema_type_int_t integer) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->kind = LOU_SEMA_TYPE_INTEGER;
  type->integer = integer;
  return type;
}

static inline bool lou_sema_types_veq(const lou_sema_type_t **a, const lou_sema_type_t **b) {
  return lou_sema_types_eq(*a, *b);
}

bool lou_sema_types_eq(const lou_sema_type_t *a, const lou_sema_type_t *b) {
  if (a == b) {
    return true;
  }
  if (a == NULL || b == NULL) {
    return false;
  }
  if (a->kind != b->kind) {
    return false;
  }
  switch (a->kind) {
    case LOU_SEMA_TYPE_INTEGER: return a->integer.size == b->integer.size && a->integer.is_signed == b->integer.is_signed;
    case LOU_SEMA_TYPE_FUNCTION:
      return lou_vec_eq(a->func.args, b->func.args, (lou_veq_func_t)lou_sema_types_veq) &&
        ((b->func.returns == a->func.returns && a->func.returns == NULL) || lou_sema_types_eq(b->func.returns, a->func.returns));
    case LOU_SEMA_TYPE_STRING: return true;
    case LOU_SEMA_TYPE_POINTER: return lou_sema_types_eq(a->pointer_to, b->pointer_to);
  }
  UNREACHABLE();
}

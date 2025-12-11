#include "type.h"
#include "lou/core/assertions.h"
#include "lou/core/log.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/hir/type.h"
#include <assert.h>

lou_sema_struct_field_t lou_sema_struct_field_new(lou_slice_t name, lou_sema_type_t *type) {
  return (lou_sema_struct_field_t) {
    .name = name,
    .type = type,
  };
}

static inline lou_sema_type_t *lou_sema_type_new(lou_mempool_t *mempool) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->converted_type = NULL;
  return type;
}

lou_sema_type_t *lou_sema_type_new_func(lou_mempool_t *mempool) {
  lou_sema_type_t *type = lou_sema_type_new(mempool);
  type->complete = false;
  type->kind = LOU_SEMA_TYPE_FUNCTION;
  return type;
}

lou_sema_type_t *lou_sema_type_new_array(lou_mempool_t *mempool, lou_sema_type_t *of, size_t length) {
  lou_sema_type_t *type = lou_sema_type_new(mempool);
  type->complete = true;
  type->kind = LOU_SEMA_TYPE_ARRAY;
  type->array.of = of;
  type->array.length = length;
  return type;
}

lou_sema_type_t *lou_sema_type_new_pointer(lou_mempool_t *mempool) {
  lou_sema_type_t *type = lou_sema_type_new(mempool);
  type->complete = false;
  type->kind = LOU_SEMA_TYPE_POINTER;
  return type;
}

lou_sema_type_t *lou_sema_type_new_bool(lou_mempool_t *mempool) {
  lou_sema_type_t *type = lou_sema_type_new(mempool);
  type->complete = true;
  type->kind = LOU_SEMA_TYPE_BOOL;
  return type;
}

lou_sema_type_t *lou_sema_type_new_int(lou_mempool_t *mempool, lou_sema_int_size_t size, bool is_signed) {
  lou_sema_type_t *type = lou_sema_type_new(mempool);
  type->complete = true;
  type->kind = LOU_SEMA_TYPE_INTEGER;
  type->integer.size = size;
  type->integer.is_signed = is_signed;
  return type;
}

lou_sema_type_t *lou_sema_type_new_string(lou_mempool_t *mempool) {
  lou_sema_type_t *type = lou_sema_type_new(mempool);
  type->complete = true;
  type->kind = LOU_SEMA_TYPE_STRING;
  return type;
}

lou_sema_type_t *lou_sema_type_new_struct(lou_mempool_t *mempool, lou_sema_struct_field_t *fields) {
  lou_sema_type_t *type = lou_sema_type_new(mempool);
  type->complete = true;
  type->kind = LOU_SEMA_TYPE_STRUCT;
  type->structure.fields = fields;
  return type;
}

void lou_sema_type_init_func(lou_sema_type_t *type, lou_sema_type_t **args, lou_sema_type_t *returns) {
  assert(type->kind == LOU_SEMA_TYPE_FUNCTION);
  type->complete = true;
  type->func.args = args;
  type->func.returns = returns;
}

void lou_sema_type_init_pointer(lou_sema_type_t *type, lou_sema_type_t *to) {
  assert(type->kind == LOU_SEMA_TYPE_POINTER);
  type->complete = true;
  type->pointer_to = to;
}

static bool lou_sema_struct_field_eq(const lou_sema_struct_field_t *a, const lou_sema_struct_field_t *b) {
  return lou_slice_eq(a->name, b->name) && lou_sema_type_eq(a->type, b->type);
}

bool lou_sema_type_eq(const lou_sema_type_t *a, const lou_sema_type_t *b) {
  assert(!a || a->complete);
  assert(!b || b->complete);
  if (a == b) return true;
  if ((a == NULL) != (b == NULL)) return false;
  if (a->kind != b->kind) return false;
  switch (a->kind) {
    case LOU_SEMA_TYPE_POINTER: return lou_sema_type_eq(a->pointer_to, b->pointer_to);
    case LOU_SEMA_TYPE_INTEGER: return a->integer.size == b->integer.size && a->integer.is_signed == b->integer.is_signed;
    case LOU_SEMA_TYPE_STRING: return true;
    case LOU_SEMA_TYPE_BOOL: return true;
    case LOU_SEMA_TYPE_FUNCTION:
      return ((a->func.returns == NULL) == (b->func.returns == NULL)) &&
        (a->func.returns == NULL || lou_sema_type_eq(a->func.returns, b->func.returns)) &&
        lou_vec_eq(a->func.args, b->func.args, (lou_veq_func_t)lou_sema_type_eq);
    case LOU_SEMA_TYPE_ARRAY:
      return a->array.length == b->array.length && lou_sema_type_eq(a->array.of, b->array.of);
    case LOU_SEMA_TYPE_STRUCT:
      return lou_vec_eq(a->structure.fields, b->structure.fields, (lou_veq_func_t)lou_sema_struct_field_eq);
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

lou_hir_type_t *lou_sema_type_as_hir(lou_mempool_t *mempool, lou_sema_type_t *type) {
  if (type->converted_type) return type->converted_type;
  assert(type->complete);
  switch (type->kind) {
    case LOU_SEMA_TYPE_STRING: NOT_IMPLEMENTED;
    case LOU_SEMA_TYPE_POINTER: return lou_hir_type_new_pointer(mempool, lou_sema_type_as_hir(mempool, type->pointer_to));
    case LOU_SEMA_TYPE_INTEGER: return lou_hir_type_new_integer(mempool, lou_sema_int_size_as_hir(type->integer.size), type->integer.is_signed);
    case LOU_SEMA_TYPE_FUNCTION: {
      lou_hir_type_t *result = type->converted_type = lou_hir_type_new_func(mempool, LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_type_t*), NULL);
      for (size_t i = 0; i < lou_vec_len(type->func.args); i++) {
        *LOU_VEC_PUSH(&result->func.args) = lou_sema_type_as_hir(mempool, type->func.args[i]);
      }
      result->func.returns = type->func.returns ? lou_sema_type_as_hir(mempool, type->func.returns) : NULL;
      return result;
    }
    case LOU_SEMA_TYPE_BOOL: return lou_hir_type_new_bool(mempool);
    case LOU_SEMA_TYPE_ARRAY: return lou_hir_type_new_array(mempool, lou_sema_type_as_hir(mempool, type->array.of), type->array.length);
    case LOU_SEMA_TYPE_STRUCT: {
      lou_hir_type_t *result = type->converted_type = lou_hir_type_new_structure(mempool, LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_type_t*));
      for (size_t i = 0; i < lou_vec_len(type->func.args); i++) {
        *LOU_VEC_PUSH(&result->structure.fields) = lou_sema_type_as_hir(mempool, type->structure.fields[i].type);
      }
      return result;
    }
  }
  UNREACHABLE();
}

void lou_sema_type_log(FILE *stream, lou_sema_type_t *type) {
  if (!type) {
    lou_log_puts(stream, "(null)");
    return;
  }
  if (!type->complete) {
    lou_log_puts(stream, "(incomplete type)");
    return;
  }
  switch (type->kind) {
    case LOU_SEMA_TYPE_STRING: fprintf(stream, "string"); return;
    case LOU_SEMA_TYPE_INTEGER:
      fprintf(stream, type->integer.is_signed ? "i" : "u");
      switch (type->integer.size) {
        case LOU_SEMA_INT_8: fprintf(stream, "8"); break;
        case LOU_SEMA_INT_16: fprintf(stream, "16"); break;
        case LOU_SEMA_INT_32: fprintf(stream, "32"); break;
        case LOU_SEMA_INT_64: fprintf(stream, "64"); break;
      }
      return;
    case LOU_SEMA_TYPE_FUNCTION:
      lou_log_puts(stream, "@fun((");
      for (size_t i = 0; i < lou_vec_len(type->func.args); i++) {
        lou_log_puts(stream, i == 0 ? "#T" : ", #T", type->func.args[i]);
      }
      lou_log_puts(stream, ")");
      if (type->func.returns) {
        lou_log_puts(stream, ", #T", type->func.returns);

      }
      lou_log_puts(stream, ")");
      return;
    case LOU_SEMA_TYPE_STRUCT:
      lou_log_puts(stream, "struct {");
      for (size_t i = 0; i < lou_vec_len(type->structure.fields); i++) {
        lou_log_puts(stream, i == 0 ? " #S: #T" : ", #S: #T", type->structure.fields[i].name, type->structure.fields[i].type);
      }
      lou_log_puts(stream, " }");
      if (type->func.returns) {
        lou_log_puts(stream, ", #T", type->func.returns);

      }
      lou_log_puts(stream, ")");
      return;
    case LOU_SEMA_TYPE_POINTER:
      lou_log_puts(stream, "@ptr(#T)", type->pointer_to);
      return;
    case LOU_SEMA_TYPE_BOOL:
      lou_log_puts(stream, "bool");
      return;
    case LOU_SEMA_TYPE_ARRAY:
      lou_log_puts(stream, "@array(#l, #T)", type->array.length, type->array.of);
      return;
  }
  UNREACHABLE();
}

size_t lou_sema_type_struct_get_member(lou_sema_type_t *type, lou_slice_t name) {
  assert(type->kind == LOU_SEMA_TYPE_STRUCT);
  for (size_t i = 0; i < lou_vec_len(type->structure.fields); i++) {
    if (lou_slice_eq(type->structure.fields[i].name, name)) {
      return i;
    }
  }
  return -1;
}

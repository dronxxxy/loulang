#pragma once

#include "lou/core/mempool.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct lou_sema_type_t lou_sema_type_t;

typedef enum {
  LOU_SEMA_TYPE_INTEGER,
  LOU_SEMA_TYPE_FUNCTION,
  LOU_SEMA_TYPE_STRING,
  LOU_SEMA_TYPE_POINTER,
} lou_sema_type_kind_t;

typedef enum {
  LOU_SEMA_INT_8,
  LOU_SEMA_INT_16,
  LOU_SEMA_INT_32,
  LOU_SEMA_INT_64,
} lou_sema_int_size_t;

typedef struct {
  lou_sema_int_size_t size;
  bool is_signed;
} lou_sema_type_int_t;

typedef struct {
  lou_opt_slice_t name;
  lou_sema_type_t *type;
} lou_sema_type_func_arg_t;

typedef struct {
  lou_sema_type_func_arg_t *args;
  lou_sema_type_t *returns;
} lou_sema_type_func_t;

typedef struct lou_sema_type_t {
  lou_sema_type_kind_t kind;

  union {
    lou_sema_type_int_t integer;
    lou_sema_type_func_t func;
    size_t string_length;
    lou_sema_type_t *pointer_to;
  };
} lou_sema_type_t;

lou_sema_type_t *lou_sema_type_new_string(lou_mempool_t *mempool);
lou_sema_type_t *lou_sema_type_new_function(lou_mempool_t *mempool, lou_sema_type_func_t func);
lou_sema_type_t *lou_sema_type_new_pointer(lou_mempool_t *mempool, lou_sema_type_t *to);
lou_sema_type_t *lou_sema_type_new_integer(lou_mempool_t *mempool, lou_sema_type_int_t integer);

bool lou_sema_types_eq(const lou_sema_type_t *a, const lou_sema_type_t *b);

void lou_sema_type_print(FILE *stream, lou_sema_type_t *type);

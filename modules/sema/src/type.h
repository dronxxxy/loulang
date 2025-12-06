#pragma once

#include "lou/core/mempool.h"
#include <stdbool.h>

typedef enum {
  LOU_SEMA_INT_8, 
  LOU_SEMA_INT_16, 
  LOU_SEMA_INT_32, 
  LOU_SEMA_INT_64, 
} lou_sema_int_size_t;

typedef enum {
  LOU_SEMA_TYPE_INTEGER, 
  LOU_SEMA_TYPE_FUNCTION, 
} lou_sema_type_kind_t;

typedef struct lou_sema_type_t lou_sema_type_t;

typedef struct lou_sema_type_t {
  lou_sema_type_kind_t kind;
  bool complete;

  union {
    struct {
      lou_sema_int_size_t size;
      bool is_signed;
    } integer;

    struct {
      lou_sema_type_t **args;
      lou_sema_type_t *returns;
    } func;
  };
} lou_sema_type_t;

lou_sema_type_t *lou_sema_type_new_int(lou_mempool_t *mempool, lou_sema_int_size_t size, bool is_signed);
lou_sema_type_t *lou_sema_type_new_func(lou_mempool_t *mempool);

void lou_sema_type_init_func(lou_sema_type_t *type, lou_sema_type_t **args, lou_sema_type_t *returns);

bool lou_sema_type_eq(const lou_sema_type_t *a, const lou_sema_type_t *b);

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
} lou_sema_type_kind_t;

typedef struct {
  lou_sema_type_kind_t kind;

  union {
    struct {
      lou_sema_int_size_t size;
      bool is_signed;
    } integer;
  };
} lou_sema_type_t;

lou_sema_type_t *lou_sema_type_new_int(lou_mempool_t *mempool, lou_sema_int_size_t size, bool is_signed);

bool lou_sema_type_eq(const lou_sema_type_t *a, const lou_sema_type_t *b);

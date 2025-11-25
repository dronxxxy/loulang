#pragma once

#include "lir/type.h"
#include <stdbool.h>

typedef struct {
  lir_type_int_size_t size;
  bool is_signed;
} lir_type_int_t;

typedef enum {
  LIR_TYPE_INT,
} lir_type_kind_t;

typedef struct lir_type_t {
  lir_type_kind_t kind;

  union {
    lir_type_int_t integer;
  };
} lir_type_t;

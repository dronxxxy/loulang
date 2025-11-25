#pragma once

#include "type.h"
#include <stdint.h>

typedef enum {
  LIR_CONST_INT,
} lir_const_kind_t;

typedef struct {
  lir_const_kind_t kind;
  lir_type_t *type;

  union {
    uint64_t integer;
  };
} lir_const_t;


#pragma once

typedef enum {
  SEMA_VALUE_CONST,
} lou_sema_value_kind_t;

typedef struct lou_sema_value_t {
  lou_sema_value_kind_t kind;
} lou_sema_value_t;

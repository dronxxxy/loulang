#pragma once

#include "lou/core/mempool.h"
#include <stdbool.h>

typedef struct lou_sema_const_t lou_sema_const_t;
typedef struct lou_sema_plugin_t lou_sema_plugin_t;
typedef struct lou_sema_decl_t lou_sema_decl_t;
typedef struct lou_sema_const_func_t lou_sema_const_func_t;
typedef struct lou_sema_type_t lou_sema_type_t;

typedef enum {
  LOU_SEMA_VALUE_PLUGIN,
  LOU_SEMA_VALUE_TYPE,
  LOU_SEMA_VALUE_CONSTANT,
  LOU_SEMA_VALUE_DECL,
} lou_sema_value_kind_t;

typedef struct lou_sema_value_t {
  lou_sema_value_kind_t kind;

  union {
    lou_sema_const_t *constant;
    lou_sema_plugin_t *plugin;
    lou_sema_decl_t *decl;
    lou_sema_type_t *type;
  };
} lou_sema_value_t;

lou_sema_value_t *lou_sema_value_new_decl(lou_mempool_t *mempool, lou_sema_decl_t *decl);
lou_sema_value_t *lou_sema_value_new_plugin(lou_mempool_t *mempool, lou_sema_plugin_t *plugin);
lou_sema_value_t *lou_sema_value_new_type(lou_mempool_t *mempool, lou_sema_type_t *type);
lou_sema_value_t *lou_sema_value_new_constant(lou_mempool_t *mempool, lou_sema_const_t *constant);

lou_slice_t *lou_sema_value_is_const_string(lou_sema_value_t *value);
lou_sema_const_func_t *lou_sema_value_is_const_func(lou_sema_value_t *value);
lou_sema_type_t *lou_sema_value_is_type(lou_sema_value_t *value);
lou_sema_type_t *lou_sema_value_is_runtime(lou_sema_value_t *value);

#pragma once

#include "const.h"
#include "lou/core/mempool.h"
#include "type.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct lou_sema_plugin_t lou_sema_plugin_t;
typedef struct lou_sema_decl_t lou_sema_decl_t;
typedef struct lou_sema_type_t lou_sema_type_t;

typedef enum {
  LOU_SEMA_VALUE_COMPTIME_PLUGIN,
  LOU_SEMA_VALUE_COMPTIME_TYPE,
} lou_sema_value_comptime_kind_t;

typedef struct {
  lou_sema_value_comptime_kind_t kind;

  union {
    lou_sema_plugin_t *plugin;
    lou_sema_type_t *type;
  };
} lou_sema_value_comptime_t;

typedef enum {
  LOU_SEMA_VALUE_RUNTIME_CONSTANT,
  LOU_SEMA_VALUE_RUNTIME_DECL,
} lou_sema_value_runtime_kind_t;

typedef enum {
  LOU_SEMA_VALUE_DECL_GLOBAL,
  LOU_SEMA_VALUE_DECL_LOCAL,
} lou_sema_value_decl_kind_t;

typedef struct {
  lou_sema_value_runtime_kind_t kind;

  union {
    lou_sema_const_t *constant;
    struct {
      lou_sema_value_decl_kind_t kind;
      lou_sema_type_t *type;
    } decl;
  };
} lou_sema_value_runtime_t;

typedef enum {
  LOU_SEMA_VALUE_COMPTIME,
  LOU_SEMA_VALUE_RUNTIME,
} lou_sema_value_kind_t;

typedef struct lou_sema_value_t {
  lou_sema_value_kind_t kind;

  union {
    lou_sema_value_runtime_t runtime;
    lou_sema_value_comptime_t comptime;
  };
} lou_sema_value_t;

lou_sema_value_t *lou_sema_value_new_plugin(lou_mempool_t *mempool, lou_sema_plugin_t *plugin);
lou_sema_value_t *lou_sema_value_new_type(lou_mempool_t *mempool, lou_sema_type_t *type);
lou_sema_value_t *lou_sema_value_new_constant(lou_mempool_t *mempool, lou_sema_const_t *constant);
lou_sema_value_t *lou_sema_value_new_global_decl(lou_mempool_t *mempool, lou_sema_type_t *type);

lou_sema_plugin_t *lou_sema_value_is_plugin(lou_sema_value_t *value);
lou_sema_type_t *lou_sema_value_is_type(lou_sema_value_t *value);
lou_sema_const_t *lou_sema_value_is_const(lou_sema_value_t *value);
lou_sema_type_t *lou_sema_value_is_decl(lou_sema_value_t *value);
lou_sema_type_t *lou_sema_value_is_runtime(lou_sema_value_t *value);

lou_slice_t *lou_sema_value_is_const_string(lou_sema_value_t *value);
lou_sema_const_t *lou_sema_value_is_const_func(lou_sema_value_t *value);

void lou_sema_value_print(FILE *stream, lou_sema_value_t *value);

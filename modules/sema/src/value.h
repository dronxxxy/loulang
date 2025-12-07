#pragma once

#include "lou/hir/code.h"
#include "plugin.h"
#include "type.h"
#include "const.h"

typedef enum {
  LOU_SEMA_VALUE_COMPTIME_PLUGIN,
  LOU_SEMA_VALUE_COMPTIME_TYPE,
} lou_sema_value_comptime_kind_t;

typedef enum {
  LOU_SEMA_VALUE_RUNTIME_CONSTANT,
} lou_sema_value_runtime_kind_t;

typedef enum {
  LOU_SEMA_VALUE_RUNTIME,
  LOU_SEMA_VALUE_COMPTIME,
} lou_sema_value_kind_t;

typedef struct lou_sema_value_t {
  lou_sema_value_kind_t kind;

  union {
    struct {
      lou_sema_value_runtime_kind_t kind;

      union {
        lou_sema_const_t *constant;
      };
    } runtime;

    struct {
      lou_sema_value_comptime_kind_t kind;

      union {
        lou_sema_type_t *type;
        lou_sema_plugin_t *plugin;
      };
    } comptime;
  };
} lou_sema_value_t;

lou_sema_value_t *lou_sema_value_new_const(lou_mempool_t *mempool, lou_sema_const_t *constant);
lou_sema_value_t *lou_sema_value_new_plugin(lou_mempool_t *mempool, lou_sema_plugin_t *plugin);
lou_sema_value_t *lou_sema_value_new_type(lou_mempool_t *mempool, lou_sema_type_t *type);

lou_sema_const_t *lou_sema_value_is_const(lou_sema_value_t *value);
lou_sema_type_t *lou_sema_value_is_runtime(lou_sema_value_t *value);
lou_sema_type_t *lou_sema_value_is_type(lou_sema_value_t *value);
lou_sema_plugin_t *lou_sema_value_is_plugin(lou_sema_value_t *value);

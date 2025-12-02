#pragma once

#include "lou/core/mempool.h"
typedef struct lou_sema_const_t lou_sema_const_t;
typedef struct lou_sema_plugin_t lou_sema_plugin_t;
typedef struct lou_sema_decl_t lou_sema_decl_t;

typedef enum {
  LOU_SEMA_VALUE_PLUGIN,
  LOU_SEMA_VALUE_CONSTANT,
  LOU_SEMA_VALUE_GLOBAL_DECL,
} lou_sema_value_kind_t;

typedef struct lou_sema_value_t {
  lou_sema_value_kind_t kind;

  union {
    lou_sema_const_t *constant;
    lou_sema_plugin_t *plugin;
    lou_sema_decl_t *decl;
  };
} lou_sema_value_t;

static inline lou_sema_value_t *lou_sema_value_new_decl(lou_mempool_t *mempool, lou_sema_decl_t *decl) {
  lou_sema_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_sema_value_t);
  value->kind = LOU_SEMA_VALUE_GLOBAL_DECL;
  value->decl = decl;
  return value;
}

static inline lou_sema_value_t *lou_sema_value_new_plugin(lou_mempool_t *mempool, lou_sema_plugin_t *plugin) {
  lou_sema_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_sema_value_t);
  value->kind = LOU_SEMA_VALUE_CONSTANT;
  value->plugin= plugin;
  return value;
}

static inline lou_sema_value_t *lou_sema_value_new_constant(lou_mempool_t *mempool, lou_sema_const_t *constant) {
  lou_sema_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_sema_value_t);
  value->kind = LOU_SEMA_VALUE_CONSTANT;
  value->constant = constant;
  return value;
}

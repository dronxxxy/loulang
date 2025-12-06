#include "value.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"

lou_sema_value_t *lou_sema_value_new_const(lou_mempool_t *mempool, lou_sema_const_t *constant) {
  lou_sema_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_sema_value_t);
  value->kind = LOU_SEMA_VALUE_RUNTIME;
  value->runtime.kind = LOU_SEMA_VALUE_RUNTIME_CONSTANT;
  value->runtime.constant = constant;
  return value;
}

lou_sema_value_t *lou_sema_value_new_type(lou_mempool_t *mempool, lou_sema_type_t *type) {
  lou_sema_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_sema_value_t);
  value->kind = LOU_SEMA_VALUE_COMPTIME;
  value->comptime.kind = LOU_SEMA_VALUE_COMPTIME_TYPE;
  value->comptime.type = type;
  return value;
}

lou_sema_value_t *lou_sema_value_new_plugin(lou_mempool_t *mempool, lou_sema_plugin_t *plugin) {
  lou_sema_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_sema_value_t);
  value->kind = LOU_SEMA_VALUE_COMPTIME;
  value->comptime.kind = LOU_SEMA_VALUE_COMPTIME_PLUGIN;
  value->comptime.plugin = plugin;
  return value;
}

lou_sema_type_t *lou_sema_value_is_runtime(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_RUNTIME) return NULL;
  switch (value->runtime.kind) {
    case LOU_SEMA_VALUE_RUNTIME_CONSTANT: return value->runtime.constant->type;
  }
  UNREACHABLE();
}

lou_sema_type_t *lou_sema_value_is_type(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_COMPTIME || value->comptime.kind != LOU_SEMA_VALUE_COMPTIME_TYPE) return NULL;
  return value->comptime.type;
}

lou_sema_plugin_t *lou_sema_value_is_plugin(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_COMPTIME || value->comptime.kind != LOU_SEMA_VALUE_COMPTIME_PLUGIN) return NULL;
  return value->comptime.plugin;
}

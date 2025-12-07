#include "value.h"
#include "lou/core/assertions.h"
#include "lou/core/log.h"
#include "lou/core/mempool.h"
#include "lou/hir/value.h"

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

lou_sema_value_t *lou_sema_value_new_local(lou_mempool_t *mempool, lou_sema_type_t *type, lou_hir_local_t *local) {
  lou_sema_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_sema_value_t);
  value->kind = LOU_SEMA_VALUE_RUNTIME;
  value->runtime.kind = LOU_SEMA_VALUE_RUNTIME_LOCAL;
  value->runtime.local.type = type;
  value->runtime.local.hir = local;
  return value;
}

lou_sema_type_t *lou_sema_value_is_runtime(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_RUNTIME) return NULL;
  switch (value->runtime.kind) {
    case LOU_SEMA_VALUE_RUNTIME_CONSTANT: return value->runtime.constant->type;
    case LOU_SEMA_VALUE_RUNTIME_LOCAL: return value->runtime.local.type;
  }
  UNREACHABLE();
}

lou_sema_const_t *lou_sema_value_is_const(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_RUNTIME || value->runtime.kind != LOU_SEMA_VALUE_RUNTIME_CONSTANT) return NULL;
  return value->runtime.constant;
}

lou_sema_type_t *lou_sema_value_is_type(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_COMPTIME || value->comptime.kind != LOU_SEMA_VALUE_COMPTIME_TYPE) return NULL;
  return value->comptime.type;
}

lou_sema_plugin_t *lou_sema_value_is_plugin(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_COMPTIME || value->comptime.kind != LOU_SEMA_VALUE_COMPTIME_PLUGIN) return NULL;
  return value->comptime.plugin;
}

lou_hir_value_t *lou_sema_value_as_hir(lou_mempool_t *mempool, lou_sema_value_t *value) {
  assert(lou_sema_value_is_runtime(value));
  switch (value->runtime.kind) {
    case LOU_SEMA_VALUE_RUNTIME_CONSTANT: return lou_hir_value_new_const(mempool, lou_sema_const_as_hir(mempool, value->runtime.constant));
    case LOU_SEMA_VALUE_RUNTIME_LOCAL: return lou_hir_value_new_local(mempool, value->runtime.local.hir);
  }
  UNREACHABLE();
}

void lou_sema_value_log(FILE *stream, lou_sema_value_t *value) {
  switch (value->kind) {
    case LOU_SEMA_VALUE_RUNTIME:
      switch (value->runtime.kind) {
        case LOU_SEMA_VALUE_RUNTIME_CONSTANT: lou_log_puts(stream, "constant #C", value->runtime.constant); return;
        case LOU_SEMA_VALUE_RUNTIME_LOCAL: lou_log_puts(stream, "local of type #T", value->runtime.local.type); return;
      }
      UNREACHABLE();
    case LOU_SEMA_VALUE_COMPTIME:
      switch (value->comptime.kind) {
        case LOU_SEMA_VALUE_COMPTIME_PLUGIN: fprintf(stream, "plugin"); return;
        case LOU_SEMA_VALUE_COMPTIME_TYPE: lou_log_puts(stream, "type #T", value->comptime.type); return;
      }
      UNREACHABLE();
  }
  UNREACHABLE();
}

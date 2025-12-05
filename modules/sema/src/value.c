#include "value.h"
#include "const.h"
#include "lou/core/assertions.h"
#include "lou/core/log.h"
#include "lou/hir/code.h"
#include "sema.h"
#include <endian.h>
#include <stdio.h>

void lou_sema_value_print(FILE *stream, lou_sema_value_t *value) {
  switch (value->kind) {
    case LOU_SEMA_VALUE_RUNTIME:
      switch (value->comptime.kind) {
        case LOU_SEMA_VALUE_RUNTIME_DECL: lou_log_puts(stream, "declaration #T", value->runtime.decl.type); return;
        case LOU_SEMA_VALUE_RUNTIME_CONSTANT: lou_log_puts(stream, "constant #T", value->runtime.constant->type); return;
      }
      break;
    case LOU_SEMA_VALUE_COMPTIME:
      switch (value->comptime.kind) {
        case LOU_SEMA_VALUE_COMPTIME_PLUGIN: fprintf(stream, "plugin"); return;
        case LOU_SEMA_VALUE_COMPTIME_TYPE: lou_log_puts(stream, "type #T", value->comptime.type); return;
      }
      break;
  }
  UNREACHABLE();
}

lou_sema_plugin_t *lou_sema_value_is_plugin(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_COMPTIME || value->comptime.kind != LOU_SEMA_VALUE_COMPTIME_PLUGIN) {
    return NULL;
  }
  return value->comptime.plugin;
}

lou_sema_type_t *lou_sema_value_is_type(lou_sema_value_t *value)  {
  if (value->kind != LOU_SEMA_VALUE_COMPTIME || value->comptime.kind != LOU_SEMA_VALUE_COMPTIME_TYPE) {
    return NULL;
  }
  return value->comptime.type;
}

lou_sema_const_t *lou_sema_value_is_const(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_RUNTIME || value->runtime.kind != LOU_SEMA_VALUE_RUNTIME_CONSTANT) {
    return NULL;
  }
  return value->runtime.constant;
}

lou_sema_type_t *lou_sema_value_is_decl(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_RUNTIME || value->runtime.kind != LOU_SEMA_VALUE_RUNTIME_DECL) {
    return NULL;
  }
  return value->runtime.decl.type;
}

lou_slice_t *lou_sema_value_is_const_string(lou_sema_value_t *value) {
  lou_sema_const_t *constant = NOT_NULL(lou_sema_value_is_const(value));
  if (constant->kind != LOU_SEMA_CONST_STRING) {
    return NULL;
  }
  return &constant->string;
}

lou_sema_const_t *lou_sema_value_is_const_func(lou_sema_value_t *value) {
  lou_sema_const_t *constant = NOT_NULL(lou_sema_value_is_const(value));
  if (constant->kind != LOU_SEMA_CONST_FUNCTION) {
    return NULL;
  }
  return constant;
}

static inline lou_sema_value_t *lou_sema_value_new_runtime(lou_mempool_t *mempool, lou_sema_value_runtime_t runtime) {
  lou_sema_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_sema_value_t);
  value->kind = LOU_SEMA_VALUE_RUNTIME;
  value->runtime = runtime;
  return value;
}

static inline lou_sema_value_t *lou_sema_value_new_comptime(lou_mempool_t *mempool, lou_sema_value_comptime_t comptime) {
  lou_sema_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_sema_value_t);
  value->kind = LOU_SEMA_VALUE_COMPTIME;
  value->comptime = comptime;
  return value;
}

lou_sema_value_t *lou_sema_value_new_plugin(lou_mempool_t *mempool, lou_sema_plugin_t *plugin) {
  return lou_sema_value_new_comptime(mempool, (lou_sema_value_comptime_t) {
    .kind = LOU_SEMA_VALUE_COMPTIME_PLUGIN,
    .plugin = plugin,
  });
}

lou_sema_value_t *lou_sema_value_new_type(lou_mempool_t *mempool, lou_sema_type_t *type) {
  return lou_sema_value_new_comptime(mempool, (lou_sema_value_comptime_t) {
    .kind = LOU_SEMA_VALUE_COMPTIME_TYPE,
    .type = type,
  });
}

lou_sema_value_t *lou_sema_value_new_constant(lou_mempool_t *mempool, lou_sema_const_t *constant) {
  return lou_sema_value_new_runtime(mempool, (lou_sema_value_runtime_t) {
    .kind = LOU_SEMA_VALUE_RUNTIME_CONSTANT,
    .constant = constant,
  });
}

lou_sema_value_t *lou_sema_value_new_local_decl(lou_mempool_t *mempool, lou_sema_type_t *type, lou_hir_local_t *local) {
  return lou_sema_value_new_runtime(mempool, (lou_sema_value_runtime_t) {
    .kind = LOU_SEMA_VALUE_RUNTIME_DECL,
    .decl = {
      .kind = LOU_SEMA_VALUE_DECL_LOCAL,
      .type = type,
      .local = local,
    },
  });
}

lou_sema_type_t *lou_sema_value_is_runtime(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_RUNTIME) {
    return NULL;
  }
  switch (value->runtime.kind) {
    case LOU_SEMA_VALUE_RUNTIME_CONSTANT: return value->runtime.constant->type;
    case LOU_SEMA_VALUE_RUNTIME_DECL: return value->runtime.decl.type;
  }
  UNREACHABLE();
}

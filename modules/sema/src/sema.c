#include "sema.h"
#include "analyze/node.h"
#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/core/vec.h"
#include "lou/parser/ast/decl.h"
#include "plugin.h"
#include "scope.h"
#include "type.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

void lou_sema_init_decl(lou_sema_decl_t *decl, lou_sema_value_t *value) {
  decl->prefetch_node = NULL;
  decl->value = value;
}

lou_sema_decl_t *lou_sema_add_decl(
  lou_sema_t *sema,
  bool is_public,
  lou_slice_t name,
  lou_sema_decl_kind_t kind,
  lou_ast_node_t *prefetch_node
) {
  lou_sema_decl_t *result = LOU_MEMPOOL_ALLOC(sema->mempool, lou_sema_decl_t);
  *result = (lou_sema_decl_t) {
    .name = name,
    .in = sema,
    .is_public = is_public,
    .kind = kind,
    .prefetch_node = prefetch_node,
    .value = NULL,
    .scope_frame = lou_vec_len(sema->scope_frames) ? *LOU_VEC_LAST(sema->scope_frames) : NULL,
  };
  *LOU_VEC_PUSH(&sema->global_decls) = result;
  return result;
}

void lou_sema_err(lou_sema_t *sema, lou_slice_t slice, const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  lou_parser_log_error(sema->parser, slice, fmt, list);
  va_end(list);

  sema->failed = true;
}

static inline bool lou_sema_decl_is_initialized(lou_sema_decl_t *decl) {
  return decl->prefetch_node == NULL;
}

lou_sema_value_t *lou_sema_resolve(lou_sema_t *sema, lou_slice_t name) {
  for (ssize_t i = lou_vec_len(sema->global_decls) - 1; i >= 0; i--) {
    lou_sema_decl_t *decl = sema->global_decls[i];
    if (lou_slice_eq(decl->name, name)) {
      if (lou_sema_decl_is_initialized(decl)) {
        assert(decl->value);
        return decl->value;
      }
      if (!lou_sema_analyze_node(sema, decl->prefetch_node)) {
        return NULL;
      }
      assert(decl->value);
      return decl->value;
    }
  }
  lou_sema_err(sema, name, "declaration `#S` was not found!", name);
  return NULL;
}

lou_sema_value_t *lou_sema_call_plugin(lou_sema_t *sema, lou_sema_plugin_t *plugin, lou_slice_t slice, lou_slice_t *arg_slices, lou_sema_value_t **args) {
  lou_sema_plugin_call_ctx_t ctx = {
    .slice = slice,
    .sema = sema,
    .args = args,
    .arg_slices = arg_slices,
  };
  return plugin->func(&ctx);
}

lou_sema_type_t *lou_sema_expect_type(lou_sema_t *sema, lou_slice_t at, lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_TYPE) {
    lou_sema_err(sema, at, "expected type got #v", value);
    return NULL;
  }
  return value->type;
}

lou_sema_type_t *lou_sema_default_integer_type(lou_sema_t *sema) {
  return lou_sema_type_new_integer(sema->mempool, (lou_sema_type_int_t) {
    .size = LOU_SEMA_INT_32,
    .is_signed = true,
  });
}

void lou_sema_push_scope_frame(lou_sema_t *sema) {
  *LOU_VEC_PUSH(&sema->scope_frames) = lou_sema_scope_frame_new(sema->mempool);
}

void lou_sema_pop_scope_frame(lou_sema_t *sema) {
  LOU_VEC_POP(&sema->scope_frames);
}

void lou_sema_push_scope(lou_sema_t *sema) {
  assert(lou_vec_len(sema->scope_frames));
  lou_sema_scope_add(*LOU_VEC_LAST(sema->scope_frames));
}

void lou_sema_pop_scope(lou_sema_t *sema);

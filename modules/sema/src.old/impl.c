#include "impl.h"
#include "analyze/expr_ctx.h"
#include "analyze/node.h"
#include "analyze/value.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/core/vec.h"
#include "lou/hir/hir.h"
#include "lou/parser/ast/decl.h"
#include "plugin.h"
#include "scope.h"
#include "type.h"
#include "value.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

static inline lou_sema_value_t *lou_sema_add_hir_decl(lou_sema_t *sema, lou_slice_t slice, lou_sema_decl_kind_t kind, lou_sema_value_t *value) {
  switch (kind) {
    case LOU_SEMA_DECL_META:
    case LOU_SEMA_DECL_CONSTANT:
    case LOU_SEMA_DECL_FINAL:
      return value;
    case LOU_SEMA_DECL_VARIABLE: {
      lou_sema_type_t *type = lou_sema_value_is_runtime(value);
      if (!type) {
        lou_sema_err(sema, slice, "trying to assign #v to runtime declaration. Use `meta` declaration for storing compile-time values", value);
        return NULL;
      }
      lou_sema_const_t *constant = lou_sema_value_is_const(value);
      if (!type) {
        lou_sema_err(sema, slice, "non-constant initializers are not allowed in global scope, #v was passed", value);
        return NULL;
      }
      (void)constant;
      NOT_IMPLEMENTED;
    }
  }
  UNREACHABLE();
}

void lou_sema_init_decl(lou_sema_t *sema, lou_slice_t slice, lou_sema_decl_t *decl, lou_sema_value_t *value) {
  decl->prefetch_node = NULL;
  decl->value = lou_sema_add_hir_decl(sema, slice, decl->kind, value);
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

lou_sema_value_t *lou_sema_decl_ensure_initialized(lou_sema_t *sema, lou_sema_decl_t *decl) {
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

static inline lou_sema_analysing_node_t *lou_sema_get_analysing_node_of(const lou_sema_t *sema, const lou_ast_node_t *node) {
  for (size_t i = 0; i < lou_vec_len(sema->node_stack); i++) {
    lou_sema_analysing_node_t *analysing_node = &sema->node_stack[i];
    if (analysing_node->node == node) {
      return analysing_node;
    }
  }
  return NULL;
}

static inline lou_sema_value_t *lou_sema_look_up_for_prefetched(lou_sema_t *sema, lou_ast_node_t *node) {
  return NOT_NULL(lou_sema_get_analysing_node_of(sema, node))->prefetched_value;
}

bool lou_sema_is_initializer_expr(const lou_sema_t *sema, const lou_ast_expr_t *expr) {
  if (lou_vec_len(sema->node_stack) == 0) return false;
  return LOU_VEC_LAST(sema->node_stack)->initializer == expr;
}

bool lou_sema_is_analysing_node(const lou_sema_t *sema, const lou_ast_node_t *node) {
  return lou_sema_get_analysing_node_of(sema, node) != NULL;
}

lou_sema_value_t *lou_sema_resolve(lou_sema_t *sema, lou_slice_t name, bool weak) {
  for (ssize_t i = lou_vec_len(sema->global_decls) - 1; i >= 0; i--) {
    lou_sema_decl_t *decl = sema->global_decls[i];
    if (lou_slice_eq(decl->name, name)) {
      if (weak && decl->prefetch_node) return lou_sema_look_up_for_prefetched(sema, decl->prefetch_node);
      return lou_sema_decl_ensure_initialized(sema, decl);
    }
  }
  lou_sema_err(sema, name, "declaration `#S` was not found!", name);
  return NULL;
}

lou_sema_value_t *lou_sema_call_plugin(
  lou_sema_t *sema,
  lou_sema_plugin_t *plugin,
  lou_slice_t slice,
  lou_slice_t *arg_slices,
  lou_sema_value_t **args,
  lou_ast_expr_t *expr,
  lou_sema_expr_ctx_t expr_ctx
) {
  if (lou_vec_len(args) != lou_vec_len(plugin->args)) {
    lou_sema_err(sema, slice, "plugin takes #l arguments but #l were passsed", lou_vec_len(plugin->args), lou_vec_len(args));
    return NULL;
  }
  lou_sema_plugin_call_ctx_t ctx = {
    .slice = slice,
    .sema = sema,
    .args = args,
    .arg_slices = arg_slices,
    .ctx = expr_ctx
  };
  return plugin->func(&ctx);
}

lou_sema_type_t *lou_sema_default_integer_type(lou_sema_t *sema) {
  return lou_sema_type_new_integer(sema->mempool, LOU_SEMA_INT_32, true);
}

lou_hir_local_t *lou_sema_add_final(lou_sema_t *sema, lou_sema_type_t *type) {
  assert(lou_vec_len(sema->scope_frames));
  lou_sema_scope_frame_t *frame = *LOU_VEC_LAST(sema->scope_frames);
  return lou_sema_scope_add_local(sema->mempool, frame, LOU_HIR_IMMUTABLE, lou_sema_emit_type(sema->mempool, type));
}

void lou_sema_push_scope_frame(lou_sema_t *sema, lou_sema_scope_frame_t *frame) {
  *LOU_VEC_PUSH(&sema->scope_frames) = frame;
}

lou_sema_scope_frame_t *lou_sema_pop_scope_frame(lou_sema_t *sema) {
  assert(lou_vec_len(sema->scope_frames));
  lou_sema_scope_frame_t *frame = *LOU_VEC_LAST(sema->scope_frames);
  LOU_VEC_POP(&sema->scope_frames);
  return frame;
}

void lou_sema_push_scope(lou_sema_t *sema) {
  assert(lou_vec_len(sema->scope_frames) > 0);
  lou_sema_scope_add(sema->mempool, *LOU_VEC_LAST(sema->scope_frames));
}

lou_sema_scope_t *lou_sema_pop_scope(lou_sema_t *sema) {
  assert(lou_vec_len(sema->scope_frames) > 0);
  return lou_sema_scope_pop(*LOU_VEC_LAST(sema->scope_frames));
}

void lou_sema_push_stmt(lou_sema_t *sema, lou_hir_stmt_t *stmt) {
  assert(lou_vec_len(sema->scope_frames) > 0);
  lou_sema_scope_frame_push_instr(*LOU_VEC_LAST(sema->scope_frames), stmt);
}

lou_sema_type_t *lou_sema_func_returns(lou_sema_t *sema) {
  assert(lou_vec_len(sema->scope_frames) > 0);
  return (*LOU_VEC_LAST(sema->scope_frames))->returns;
}

lou_hir_t *lou_sema_hir(const lou_sema_t *sema) {
  return sema->hir;
}

void lou_sema_push_analysing_node(lou_sema_t *sema, lou_ast_node_t *node, lou_ast_expr_t *initializer) {
  *LOU_VEC_PUSH(&sema->node_stack) = (lou_sema_analysing_node_t) {
    .node = node,
    .initializer = initializer,
    .prefetched_value = NULL,
  };
}

void lou_sema_prefetch_current_node(lou_sema_t *sema, lou_sema_value_t *value) {
  assert(lou_vec_len(sema->node_stack) > 0);
  LOU_VEC_LAST(sema->node_stack)->prefetched_value = value;
}

void lou_sema_pop_analysing_node(lou_sema_t *sema) {
  LOU_VEC_POP(&sema->node_stack);
}


#include "impl.h"
#include "ast/node.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/core/vec.h"
#include "lou/hir/code.h"
#include "lou/parser/parser.h"
#include "type.h"
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>

inline bool lou_sema_is_global_scope(const lou_sema_t *sema) {
  return lou_vec_len(sema->scope_stacks) == 0;
}

void lou_sema_err(lou_sema_t *sema, lou_slice_t slice, const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  lou_parser_log_error(sema->parser, slice, fmt, list);
  va_end(list);
  sema->failed = true;
}

inline lou_sema_scope_t *lou_sema_current_scope(lou_sema_t *sema) {
  return *LOU_VEC_LAST((*LOU_VEC_LAST(sema->scope_stacks))->scopes);
}

lou_sema_decl_t *lou_sema_add_decl(lou_sema_t *sema, lou_sema_visibility_t visibility, lou_slice_t name, lou_ast_node_t *node) {
  lou_sema_decl_t *decl = LOU_MEMPOOL_ALLOC(sema->mempool, lou_sema_decl_t);
  decl->name = name;
  decl->stage = LOU_SEMA_DECL_STAGE_DECLARED;
  decl->node = node;

  if (lou_sema_is_global_scope(sema)) {
    *LOU_VEC_PUSH(&sema->global_decls) = (lou_sema_global_decl_t) {
      .decl = decl,
      .visibility = visibility,
    };
  } else {
    if (visibility == LOU_SEMA_PUBLIC) {
      lou_sema_err(sema, name, "trying to specify declaration as public in local scope");
    }
    *LOU_VEC_PUSH(&lou_sema_current_scope(sema)->decls) = decl;
  }

  return decl;
}

void lou_sema_outline_decl(lou_sema_decl_t *decl, lou_sema_value_t *value) {
  if (decl->stage >= LOU_SEMA_DECL_STAGE_SKELETON) return;
  decl->stage = LOU_SEMA_DECL_STAGE_SKELETON;
  decl->value = value;
}

void lou_sema_finalize_decl(lou_sema_decl_t *decl) {
  if (decl->stage >= LOU_SEMA_DECL_STAGE_COMPLETE) return;
  assert(decl->value);
  decl->stage = LOU_SEMA_DECL_STAGE_COMPLETE;
}

void lou_sema_kill_decl(lou_sema_decl_t *decl) {
  decl->stage = LOU_SEMA_DECL_STAGE_KILLED;
}

void lou_sema_add_internal_decl(lou_sema_t *sema, const char *name, lou_sema_value_t *value) {
  assert(lou_sema_is_global_scope(sema));

  lou_sema_decl_t *decl = LOU_MEMPOOL_ALLOC(sema->mempool, lou_sema_decl_t);
  decl->name = lou_slice_from_cstr(name);
  decl->stage = LOU_SEMA_DECL_STAGE_COMPLETE;
  decl->node = NULL;
  decl->value = value;

  *LOU_VEC_PUSH(&sema->global_decls) = (lou_sema_global_decl_t) {
    .decl = decl,
    .visibility = LOU_SEMA_INTERNAL,
  };
}

lou_sema_type_t *lou_sema_type_default_int(lou_sema_t *sema) {
  // TODO: select integer size depending on target architecture
  return lou_sema_type_new_int(sema->mempool, LOU_SEMA_INT_32, true);
}

lou_hir_local_t *lou_sema_add_local_final(lou_sema_t *sema, lou_sema_type_t *type) {
  return lou_hir_code_local_add(sema->mempool, lou_sema_current_scope(sema)->code, LOU_HIR_IMMUTABLE,
    lou_sema_type_as_hir(sema->mempool, type));
}

lou_hir_local_t *lou_sema_add_local_var(lou_sema_t *sema, lou_sema_type_t *type) {
  return lou_hir_code_local_add(sema->mempool, lou_sema_current_scope(sema)->code, LOU_HIR_MUTABLE,
    lou_sema_type_as_hir(sema->mempool, type));
}

void lou_sema_push_stmt(lou_sema_t *sema, lou_hir_stmt_t *stmt) {
  lou_hir_code_append_stmt(lou_sema_current_scope(sema)->code, stmt);
}

lou_sema_type_t *lou_sema_returns(lou_sema_t *sema) {
  return (*LOU_VEC_LAST(sema->scope_stacks))->returns;
}

static inline lou_sema_decl_t *lou_sema_resolve_decl(lou_sema_t *sema, lou_slice_t name) {
  for (ssize_t i = lou_vec_len(sema->scope_stacks) - 1; i >= 0; i--) {
    lou_sema_scope_stack_t *scope_stack = sema->scope_stacks[i];
    for (ssize_t j = lou_vec_len(scope_stack->scopes) - 1; j >= 0; j--) {
      lou_sema_scope_t *scope = scope_stack->scopes[j];
      for (ssize_t k = lou_vec_len(scope->decls) - 1; k >= 0; k--) {
        lou_sema_decl_t *decl = scope->decls[k];
        if (lou_slice_eq(decl->name, name)) return decl;
      }
    }
  }
  // TODO(1): currentNodeId
  for (ssize_t i = lou_vec_len(sema->global_decls) - 1; i >= 0; i--) {
    lou_sema_global_decl_t *global_decl = &sema->global_decls[i];
    if (!lou_slice_eq(global_decl->decl->name, name)) continue;
    // TODO: check visibility but return decl on private :)
    return global_decl->decl;
  }
  lou_sema_err(sema, name, "declaration `#S` was not found", name);
  return NULL;
}

static inline bool lou_sema_decl_check_cycle(lou_sema_t *sema, lou_slice_t name, lou_sema_decl_t *decl) {
  for (size_t i = 0; i < lou_vec_len(sema->node_stack); i++) {
    if (sema->node_stack[i] == decl->node) {
      lou_sema_err(sema, name, "cycle dependency detected");
      return false;
    }
  }
  return true;
}

static inline void lou_sema_decl_check_stack_frame(lou_sema_t *sema, lou_slice_t slice, lou_sema_decl_t *decl) {
  lou_sema_value_local_t *local = lou_sema_value_is_local(decl->value);
  if (local && local->scope_stack && local->scope_stack != lou_sema_current_scope_stack(sema)) {
    lou_sema_err(sema, slice, "trying to get final or variable from another stack frame");
  }
}

lou_sema_value_t *lou_sema_resolve_skeleton(lou_sema_t *sema, lou_slice_t name) {
  lou_sema_decl_t *decl = NOT_NULL(lou_sema_resolve_decl(sema, name));
  if (decl->stage == LOU_SEMA_DECL_STAGE_KILLED) return NULL;
  if (decl->stage < LOU_SEMA_DECL_STAGE_SKELETON) {
    NOT_NULL(lou_sema_decl_check_cycle(sema, name, decl));
    lou_sema_outline_node(sema, decl->node, decl);
    if (decl->stage == LOU_SEMA_DECL_STAGE_KILLED) return NULL;
  }
  assert(decl->stage >= LOU_SEMA_DECL_STAGE_SKELETON);
  assert(decl->value);
  lou_sema_decl_check_stack_frame(sema, name, decl);
  return decl->value;
}

inline lou_sema_scope_stack_t *lou_sema_current_scope_stack(lou_sema_t *sema) {
  return lou_vec_len(sema->scope_stacks) > 0 ? *LOU_VEC_LAST(sema->scope_stacks) : NULL;
}

lou_sema_value_t *lou_sema_resolve(lou_sema_t *sema, lou_slice_t name) {
  lou_sema_decl_t *decl = NOT_NULL(lou_sema_resolve_decl(sema, name));
  if (decl->stage == LOU_SEMA_DECL_STAGE_KILLED) return NULL;
  if (decl->stage < LOU_SEMA_DECL_STAGE_COMPLETE) {
    if (decl->stage < LOU_SEMA_DECL_STAGE_SKELETON) {
      NOT_NULL(lou_sema_decl_check_cycle(sema, name, decl));
      lou_sema_outline_node(sema, decl->node, decl);
      if (decl->stage == LOU_SEMA_DECL_STAGE_KILLED) return NULL;
    }
    NOT_NULL(lou_sema_decl_check_cycle(sema, name, decl));
    lou_sema_finalize_node(sema, decl->node, decl);
    if (decl->stage == LOU_SEMA_DECL_STAGE_KILLED) return NULL;
  }
  assert(decl->stage == LOU_SEMA_DECL_STAGE_COMPLETE);
  assert(decl->value);
  lou_sema_decl_check_stack_frame(sema, name, decl);
  return decl->value;
}

void lou_sema_push_analysing_node(lou_sema_t *sema, lou_ast_node_t *node) {
  *LOU_VEC_PUSH(&sema->node_stack) = node;
}

void lou_sema_pop_analysing_node(lou_sema_t *sema) {
  LOU_VEC_POP(&sema->node_stack);
}

void lou_sema_push_scope_stack(lou_sema_t *sema, lou_sema_type_t *returns) {
  lou_sema_scope_stack_t *scope_stack = LOU_MEMPOOL_ALLOC(sema->mempool, lou_sema_scope_stack_t);
  scope_stack->scopes = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_scope_t*);
  scope_stack->returns = returns;
  *LOU_VEC_PUSH(&sema->scope_stacks) = scope_stack;
}

void lou_sema_pop_scope_stack(lou_sema_t *sema) {
  LOU_VEC_POP(&sema->scope_stacks);
}

void lou_sema_push_scope(lou_sema_t *sema) {
  lou_sema_scope_t *scope = LOU_MEMPOOL_ALLOC(sema->mempool, lou_sema_scope_t);
  scope->decls = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_decl_t*);
  scope->code = lou_hir_code_new(sema->mempool);
  scope->break_kind = LOU_SEMA_SCOPE_BREAK_NONE;
  *LOU_VEC_PUSH(&(*LOU_VEC_LAST(sema->scope_stacks))->scopes) = scope;
}

lou_sema_scope_t *lou_sema_pop_scope(lou_sema_t *sema) {
  lou_sema_scope_stack_t *stack = *LOU_VEC_LAST(sema->scope_stacks);
  lou_sema_scope_t *scope = *LOU_VEC_LAST(stack->scopes);
  LOU_VEC_POP(&stack->scopes);
  return scope;
}

void lou_sema_break_scope(lou_sema_t *sema, lou_sema_scope_break_t break_kind) {
  lou_sema_current_scope(sema)->break_kind = break_kind;
}

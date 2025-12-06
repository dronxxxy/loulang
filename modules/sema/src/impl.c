#include "impl.h"
#include "ast/node.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/core/vec.h"
#include "lou/parser/parser.h"
#include "type.h"
#include <assert.h>
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
    // TODO: implement local scope declaring
    NOT_IMPLEMENTED;
  }

  return decl;
}

void lou_sema_outline_decl(lou_sema_decl_t *decl, lou_sema_value_t *value) {
  decl->stage = LOU_SEMA_DECL_STAGE_SKELETON;
  decl->value = value;
}

void lou_sema_finalize_decl(lou_sema_decl_t *decl) {
  assert(decl->value);
  decl->stage = LOU_SEMA_DECL_STAGE_COMPLETED;
}

void lou_sema_kill_decl(lou_sema_decl_t *decl) {
  decl->stage = LOU_SEMA_DECL_STAGE_KILLED;
}

lou_sema_type_t *lou_sema_type_default_int(lou_sema_t *sema) {
  // TODO: select integer size depending on target architecture
  return lou_sema_type_new_int(sema->mempool, LOU_SEMA_INT_32, true);
}

static inline lou_sema_decl_t *lou_sema_resolve_decl(lou_sema_t *sema, lou_slice_t name) {
  for (ssize_t i = lou_vec_len(sema->scope_stacks) - 1; i >= 0; i--) {
    lou_sema_scope_stack_t *scope_stack = &sema->scope_stacks[i];
    for (ssize_t j = lou_vec_len(scope_stack->scopes) - 1; j >= 0; j--) {
      lou_sema_scope_t *scope = &scope_stack->scopes[j];
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

lou_sema_value_t *lou_sema_resolve_skeleton(lou_sema_t *sema, lou_slice_t name) {
  lou_sema_decl_t *decl = NOT_NULL(lou_sema_resolve_decl(sema, name));
  if (decl->stage == LOU_SEMA_DECL_STAGE_KILLED) return NULL;
  if (decl->stage < LOU_SEMA_DECL_STAGE_SKELETON) {
    lou_sema_outline_node(sema, decl->node, decl);
    if (decl->stage == LOU_SEMA_DECL_STAGE_KILLED) return NULL;
  }
  assert(decl->stage >= LOU_SEMA_DECL_STAGE_SKELETON);
  assert(decl->value);
  return decl->value;
}

lou_sema_value_t *lou_sema_resolve(lou_sema_t *sema, lou_slice_t name) {
  lou_sema_decl_t *decl = NOT_NULL(lou_sema_resolve_decl(sema, name));
  if (decl->stage == LOU_SEMA_DECL_STAGE_KILLED) return NULL;
  if (decl->stage < LOU_SEMA_DECL_STAGE_COMPLETED) {
    if (decl->stage < LOU_SEMA_DECL_STAGE_SKELETON) {
      lou_sema_outline_node(sema, decl->node, decl);
      if (decl->stage == LOU_SEMA_DECL_STAGE_KILLED) return NULL;
    }
    lou_sema_outline_node(sema, decl->node, decl);
    if (decl->stage == LOU_SEMA_DECL_STAGE_KILLED) return NULL;
  }
  assert(decl->stage == LOU_SEMA_DECL_STAGE_COMPLETED);
  assert(decl->value);
  return decl->value;
}

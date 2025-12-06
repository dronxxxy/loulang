#include "impl.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/parser/parser.h"
#include <assert.h>
#include <stdarg.h>

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

#include "scope.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/hir/code.h"
#include <assert.h>

lou_sema_scope_frame_t *lou_sema_scope_frame_new(lou_mempool_t *mempool, lou_sema_type_t *returns) {
  lou_sema_scope_frame_t *frame = LOU_MEMPOOL_ALLOC(mempool, lou_sema_scope_frame_t);
  frame->scopes = LOU_MEMPOOL_VEC_NEW(mempool, lou_sema_scope_t*);
  frame->returns = returns;
  return frame;
}

static inline lou_sema_scope_t *lou_sema_scope_new(lou_mempool_t *mempool, lou_hir_code_t *code) {
  lou_sema_scope_t *scope = LOU_MEMPOOL_ALLOC(mempool, lou_sema_scope_t);
  scope->decls = LOU_MEMPOOL_VEC_NEW(mempool, lou_sema_decl_t*);
  scope->code = code;
  return scope;
}

void lou_sema_scope_add(lou_mempool_t *mempool, lou_sema_scope_frame_t *frame) {
  *LOU_VEC_PUSH(&frame->scopes) = lou_sema_scope_new(mempool, lou_hir_code_new(mempool));
}

void lou_sema_scope_frame_push_instr(lou_sema_scope_frame_t *frame, lou_hir_stmt_t *stmt) {
  assert(lou_vec_len(frame->scopes) > 0);
  lou_hir_code_append_stmt((*LOU_VEC_LAST(frame->scopes))->code, stmt);
}

lou_sema_scope_t *lou_sema_scope_pop(lou_sema_scope_frame_t *frame) {
  lou_sema_scope_t *scope = *LOU_VEC_LAST(frame->scopes);
  LOU_VEC_POP(&frame->scopes);
  return scope;
}


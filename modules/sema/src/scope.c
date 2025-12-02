#include "scope.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"

lou_sema_scope_frame_t *lou_sema_scope_frame_new(lou_mempool_t *mempool) {
  lou_sema_scope_frame_t *frame = LOU_MEMPOOL_ALLOC(mempool, lou_sema_scope_frame_t);
  frame->scopes = LOU_MEMPOOL_VEC_NEW(mempool, lou_sema_scope_t*);
  return frame;
}

static inline lou_sema_scope_t *lou_sema_scope_new(lou_mempool_t *mempool) {
  lou_sema_scope_t *scope = LOU_MEMPOOL_ALLOC(mempool, lou_sema_scope_t);
  scope->decls = LOU_MEMPOOL_VEC_NEW(mempool, lou_sema_decl_t*);
  return scope;
}

void lou_sema_scope_add(lou_sema_scope_frame_t *frame) {
  *LOU_VEC_PUSH(&frame->scopes) = lou_sema_scope_new(frame->mempool);
}

void lou_sema_scope_pop(lou_sema_scope_frame_t *frame) {
  *LOU_VEC_PUSH(&frame->scopes) = lou_sema_scope_new(frame->mempool);
}

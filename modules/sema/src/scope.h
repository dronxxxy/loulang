#pragma once

#include "lou/core/mempool.h"

typedef struct lou_sema_decl_t lou_sema_decl_t;

typedef struct {
  lou_sema_decl_t **decls;
} lou_sema_scope_t;

typedef struct lou_sema_scope_frame_t {
  lou_mempool_t *mempool;
  lou_sema_scope_t **scopes;
} lou_sema_scope_frame_t;

lou_sema_scope_frame_t *lou_sema_scope_frame_new(lou_mempool_t *mempool);
void lou_sema_scope_add(lou_sema_scope_frame_t *frame);

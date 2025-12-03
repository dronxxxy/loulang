#pragma once

#include "const.h"
#include "lou/core/mempool.h"
#include "lou/hir/code.h"
#include "lou/hir/func.h"

typedef struct lou_sema_decl_t lou_sema_decl_t;

typedef struct {
  lou_sema_decl_t **decls;
  lou_hir_code_t *code;
} lou_sema_scope_t;

typedef struct lou_sema_scope_frame_t {
  lou_sema_scope_t **scopes;
  lou_sema_type_t *returns;
} lou_sema_scope_frame_t;

lou_sema_scope_frame_t *lou_sema_scope_frame_new(lou_mempool_t *mempool, lou_sema_type_t *returns);
void lou_sema_scope_frame_push_instr(lou_sema_scope_frame_t *frame, lou_hir_stmt_t *stmt);
void lou_sema_scope_add(lou_mempool_t *mempool, lou_sema_scope_frame_t *frame);
lou_sema_scope_t *lou_sema_scope_pop(lou_sema_scope_frame_t *frame);

#pragma once

typedef struct lou_sema_decl_t lou_sema_decl_t;

typedef struct {
  lou_sema_decl_t **decls;
} lou_sema_scope_t;

typedef struct lou_sema_scope_frame_t {
  lou_sema_scope_t **scopes;
} lou_sema_scope_frame_t;


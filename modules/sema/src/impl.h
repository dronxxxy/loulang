#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/hir/hir.h"
#include "lou/parser/parser.h"
#include "type.h"
#include "value.h"

typedef enum {
  LOU_SEMA_DECL_STAGE_DECLARED,
  LOU_SEMA_DECL_STAGE_SKELETON,
  LOU_SEMA_DECL_STAGE_COMPLETE,

  LOU_SEMA_DECL_STAGE_KILLED,
} lou_sema_decl_stage_t;

typedef struct {
  lou_sema_decl_stage_t stage;
  lou_slice_t name;

  lou_ast_node_t *node;
  lou_sema_value_t *value;
} lou_sema_decl_t;

typedef enum {
  LOU_SEMA_PUBLIC,
  LOU_SEMA_PRIVATE,
  LOU_SEMA_INTERNAL,
} lou_sema_visibility_t;

typedef struct {
  lou_sema_decl_t *decl;
  lou_sema_visibility_t visibility;
} lou_sema_global_decl_t;

typedef struct {
  lou_sema_decl_t **decls;
} lou_sema_scope_t;

typedef struct {
  lou_sema_scope_t *scopes;
} lou_sema_scope_stack_t;

typedef struct {
  lou_ast_node_t *node;
  lou_sema_decl_t *decl;
} lou_sema_node_record_t;

typedef struct lou_sema_t {
  lou_mempool_t *mempool;

  lou_ast_node_t **node_stack;
  lou_sema_node_record_t *nodes;
  lou_sema_global_decl_t *global_decls;
  lou_sema_scope_stack_t *scope_stacks;

  lou_parser_t *parser;
  lou_hir_t *hir;

  bool failed;
} lou_sema_t;

bool lou_sema_is_global_scope(const lou_sema_t *sema);

void lou_sema_err(lou_sema_t *sema, lou_slice_t slice, const char *fmt, ...);

lou_sema_decl_t *lou_sema_add_decl(lou_sema_t *sema, lou_sema_visibility_t visibility, lou_slice_t name, lou_ast_node_t *node);
void lou_sema_outline_decl(lou_sema_decl_t *decl, lou_sema_value_t *value);
void lou_sema_finalize_decl(lou_sema_decl_t *decl);
void lou_sema_kill_decl(lou_sema_decl_t *decl);
void lou_sema_add_internal_decl(lou_sema_t *sema, const char *name, lou_sema_value_t *value);

lou_sema_value_t *lou_sema_resolve_skeleton(lou_sema_t *sema, lou_slice_t name);
lou_sema_value_t *lou_sema_resolve(lou_sema_t *sema, lou_slice_t name);

void lou_sema_push_analysing_node(lou_sema_t *sema, lou_ast_node_t *node);
void lou_sema_pop_analysing_node(lou_sema_t *sema);

lou_sema_type_t *lou_sema_type_default_int(lou_sema_t *sema);

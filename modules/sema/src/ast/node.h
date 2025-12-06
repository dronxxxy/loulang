#pragma once

#include "impl.h"
#include "lou/parser/ast/node.h"
#include "lou/sema/sema.h"

lou_sema_decl_t *lou_sema_declare_node(lou_sema_t *sema, lou_ast_node_t *node);
void lou_sema_outline_node(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl);
void lou_sema_finalize_node(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl);

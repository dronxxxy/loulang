#pragma once

#include "lou/parser/ast/node.h"
#include "lou/sema/sema.h"

void lou_sema_prefetch_node(lou_sema_t *sema, lou_ast_node_t *node);
bool lou_sema_analyze_node(lou_sema_t *sema, lou_ast_node_t *node);

lou_slice_t lou_sema_node_slice(lou_ast_node_t *node);


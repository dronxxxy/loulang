#pragma once

#include "lou/parser/ast/node.h"
#include "lou/sema/sema.h"

void lou_sema_read_node(lou_sema_t *sema, lou_ast_node_t *node);
void lou_sema_analyze_node(lou_sema_t *sema, lou_ast_node_t *node);


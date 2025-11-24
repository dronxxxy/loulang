#pragma once

#include "lou/core/mempool.h"
#include "lou/parser/parser.h"
#include <stdbool.h>

typedef struct lou_sema_t {
  lou_parser_t *parser;
  lou_mempool_t *mempool;
  lou_ast_node_t **nodes;
  bool failed;
} lou_sema_t;



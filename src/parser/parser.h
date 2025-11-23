#pragma once

#include "core/slice.h"
#include "parser/ast/node.h"
#include <stdbool.h>

typedef struct parser_t parser_t;

parser_t *parser_new(slice_t path);
void parser_free(parser_t *parser);

bool parser_failed(const parser_t *parser);

ast_node_t parser_next(parser_t *parser);

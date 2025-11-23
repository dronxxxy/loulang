#pragma once

#include "lou/core/slice.h"
#include <stdbool.h>

typedef struct lou_ast_node_t lou_ast_node_t;
typedef struct lou_parser_t lou_parser_t;

lou_parser_t *lou_parser_new(lou_slice_t path);
void lou_parser_free(lou_parser_t *parser);

bool lou_parser_failed(const lou_parser_t *parser);

lou_ast_node_t *lou_parser_next(lou_parser_t *parser);

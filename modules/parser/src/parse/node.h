#pragma once

#include "lou/parser/parser.h"

#define LOU_PARSER_CASE_NODE \
    case LOU_TOKEN_PUBLIC: case LOU_TOKEN_META: case LOU_TOKEN_CONST: case LOU_TOKEN_FINAL: case LOU_TOKEN_VAR

lou_ast_node_t *lou_parser_parse_node(lou_parser_t *parser);


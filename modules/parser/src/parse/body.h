#pragma once

#include "lou/parser/ast/body.h"
#include "lou/parser/parser.h"

lou_ast_body_t *lou_parser_parse_body(lou_parser_t *parser);
lou_ast_body_t *lou_parser_parse_body_one_stmt(lou_parser_t *parser);


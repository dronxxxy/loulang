#include "node.h"
#include "lou/core/assertions.h"
#include "lou/parser/ast/node.h"
#include "parse/decl.h"
#include "parse/expr.h"
#include "parser.h"

lou_ast_node_t *lou_parser_parse_node(lou_parser_t *parser) {
  lou_token_t token = lou_parser_peek(parser);
  switch (token.kind) {
    case LOU_TOKEN_PUBLIC: case LOU_TOKEN_META: case LOU_TOKEN_CONST: case LOU_TOKEN_FINAL: case LOU_TOKEN_VAR:
      return lou_parser_parse_decl_node(parser);

    default: return lou_ast_node_new_expr(parser->mempool, NOT_NULL(lou_parser_parse_expr(parser)));
  }
}


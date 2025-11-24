#include "node.h"
#include "parse/decl.h"
#include "parser.h"

lou_ast_node_t *lou_parser_parse_node(lou_parser_t *parser) {
  lou_token_t token = lou_parser_peek(parser);
  switch (token.kind) {
    LOU_PARSER_CASE_NODE:
      return lou_parser_parse_decl_node(parser);
    default:
      lou_parser_err(parser, lou_parser_take(parser).slice, "expected node");
      return NULL;
  }
}


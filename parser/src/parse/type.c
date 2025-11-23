#include "type.h"
#include "lou/parser/ast/type.h"
#include "parse/expr.h"
#include "parser.h"

lou_ast_type_t *lou_parser_parse_type(lou_parser_t *parser) {
  lou_ast_expr_t *expr = lou_parser_parse_expr(parser);
  if (!expr) return NULL;
  return lou_ast_type_new_expr(parser->mempool, expr);
}

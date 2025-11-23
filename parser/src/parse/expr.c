#include "expr.h"
#include "lou/lexer/token.h"
#include "lou/parser/ast/expr.h"
#include "lou/parser/parser.h"
#include "parser.h"

lou_ast_expr_t *lou_parser_parse_expr(lou_parser_t *parser) {
  lou_token_t token = lou_parser_peek(parser);
  switch (token.kind) {
    case LOU_TOKEN_IDENT:
      lou_parser_take(parser);
      return lou_ast_expr_new_ident(parser->mempool, token.slice);
    default:
      lou_parser_err(parser, token.slice, "expected expression");
      return NULL;
  }
}

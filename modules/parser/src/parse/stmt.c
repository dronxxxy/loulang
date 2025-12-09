#include "stmt.h"
#include "lou/core/assertions.h"
#include "lou/lexer/token.h"
#include "lou/parser/ast/stmt.h"
#include "parse/body.h"
#include "parse/expr.h"
#include "parse/node.h"
#include "parser.h"
#include <stdio.h>

lou_ast_stmt_t *lou_parser_parse_stmt(lou_parser_t *parser) {
  lou_token_t token = lou_parser_peek(parser);

  switch (token.kind) {
    case LOU_TOKEN_IF: {
      lou_parser_take(parser);
      lou_ast_expr_t *expr = lou_parser_parse_expr(parser);
      lou_ast_body_t *body = lou_parser_parse_body(parser);
      if (!expr || !body) return NULL;
      return lou_ast_stmt_new_if(parser->mempool, lou_parser_slice(parser, token.slice), expr, body);
    }
    case LOU_TOKEN_RETURN: {
      lou_parser_take(parser);
      lou_ast_expr_t *expr = NULL;
      if (!lou_parser_is_nl(parser)) {
        expr = lou_parser_parse_expr(parser);
      }
      return lou_ast_stmt_new_ret(parser->mempool, lou_parser_slice(parser, token.slice), token.slice, expr);
    }
    default: return lou_ast_stmt_new_node(parser->mempool, lou_parser_slice(parser, token.slice), NOT_NULL(lou_parser_parse_node(parser)));
  }
}



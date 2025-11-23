#include "expr.h"
#include "lou/core/vec.h"
#include "lou/lexer/token.h"
#include "lou/parser/ast/body.h"
#include "lou/parser/ast/expr.h"
#include "lou/parser/parser.h"
#include "parse/body.h"
#include "parse/type.h"
#include "parser.h"

static inline lou_ast_expr_t *lou_parser_parse_func_expr(lou_parser_t *parser) {
  lou_ast_expr_func_arg_t *args = LOU_MEMPOOL_VEC_NEW(parser->mempool, lou_ast_expr_func_arg_t);

  LOU_PARSER_EXPECT(parser, LOU_TOKEN_OPENING_CIRCLE_BRACE);

  if (!lou_parser_take_if(parser, LOU_TOKEN_CLOSING_CIRCLE_BRACE)) {
    do {
      lou_slice_t name = LOU_PARSER_EXPECT(parser, LOU_TOKEN_IDENT).slice;
      lou_ast_type_t *type = lou_parser_take_if(parser, LOU_TOKEN_COLON) ? lou_parser_parse_type(parser) : NULL;

      *LOU_VEC_PUSH(&args) = (lou_ast_expr_func_arg_t) {
        .type = type,
        .name = name,
      };
    } while (!lou_parser_is_list_end(parser, LOU_TOKEN_CLOSING_CIRCLE_BRACE));
  }

  lou_ast_type_t *returns = lou_parser_take_if(parser, LOU_TOKEN_FUNCTION_RETURNS) ? lou_parser_parse_type(parser) : NULL;
  lou_ast_body_t *body = lou_parser_parse_body(parser);

  return lou_ast_expr_new_func(parser->mempool, (lou_ast_expr_func_t) {
    .args = args,
    .returns = returns,
    .body = body,
  });
}


lou_ast_expr_t *lou_parser_parse_expr(lou_parser_t *parser) {
  lou_token_t token = lou_parser_take(parser);
  switch (token.kind) {
    case LOU_TOKEN_IDENT: return lou_ast_expr_new_ident(parser->mempool, token.slice);
    case LOU_TOKEN_FUN: return lou_parser_parse_func_expr(parser);
    case LOU_TOKEN_INTEGER: return lou_ast_expr_new_integer(parser->mempool, token.integer);
    default:
      lou_parser_err(parser, token.slice, "expected expression");
      return NULL;
  }
}

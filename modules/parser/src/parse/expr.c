#include "expr.h"
#include "lou/core/assertions.h"
#include "lou/core/slice.h"
#include "lou/core/vec.h"
#include "lou/lexer/token.h"
#include "lou/parser/ast/body.h"
#include "lou/parser/ast/expr.h"
#include "lou/parser/parser.h"
#include "parse/body.h"
#include "parser.h"

static inline lou_ast_expr_t *lou_parser_parse_func_expr(lou_parser_t *parser, lou_token_t start_token) {
  lou_ast_expr_func_arg_t *args = LOU_MEMPOOL_VEC_NEW(parser->mempool, lou_ast_expr_func_arg_t);

  LOU_PARSER_EXPECT(parser, LOU_TOKEN_OPENING_CIRCLE_BRACE);

  if (!lou_parser_take_if(parser, LOU_TOKEN_CLOSING_CIRCLE_BRACE)) {
    do {
      lou_slice_t name = LOU_PARSER_EXPECT(parser, LOU_TOKEN_IDENT).slice;
      lou_ast_expr_t *type = lou_parser_take_if(parser, LOU_TOKEN_COLON) ? lou_parser_parse_expr(parser) : NULL;

      *LOU_VEC_PUSH(&args) = (lou_ast_expr_func_arg_t) {
        .type = type,
        .name = name,
      };
    } while (!lou_parser_is_list_end(parser, LOU_TOKEN_CLOSING_CIRCLE_BRACE));
  }
  lou_ast_expr_t *returns = lou_parser_take_if(parser, LOU_TOKEN_FUNCTION_RETURNS) ? lou_parser_parse_expr(parser) : NULL;
  lou_ast_body_t *body = lou_parser_parse_body(parser);

  return lou_ast_expr_new_func(parser->mempool, lou_parser_slice(parser, start_token.slice), args, returns, body);
}

lou_ast_expr_t *lou_parser_parse_post_expr(lou_parser_t *parser, lou_ast_expr_t *expr) {
  lou_token_t next = lou_parser_peek(parser);
  switch (next.kind) {
    case LOU_TOKEN_DOT: {
      lou_parser_take(parser);
      lou_slice_t ident = LOU_PARSER_EXPECT(parser, LOU_TOKEN_IDENT).slice;
      return lou_ast_expr_new_get_ident(parser->mempool, expr, ident);
    }
    case LOU_TOKEN_ASSIGN: {
      lou_parser_take(parser);
      lou_ast_expr_t *value = NOT_NULL(lou_parser_parse_expr(parser));
      return lou_ast_expr_new_assign(parser->mempool, lou_slice_union(expr->slice, value->slice), expr, value);
    }
    default: break;
  }
  if (lou_parser_is_nl(parser)) {
    return NULL;
  }
  switch (next.kind) {
    case LOU_TOKEN_OPENING_CIRCLE_BRACE: {
      lou_parser_take(parser);
      lou_ast_expr_t **args = LOU_MEMPOOL_VEC_NEW(parser->mempool, lou_ast_expr_t*);

      if (!lou_parser_take_if(parser, LOU_TOKEN_CLOSING_CIRCLE_BRACE)) {
        do {
          lou_ast_expr_t *expr = lou_parser_parse_expr(parser);
          if (expr) {
            *LOU_VEC_PUSH(&args) = expr;
          }
        } while (!lou_parser_is_list_end(parser, LOU_TOKEN_CLOSING_CIRCLE_BRACE));
      }

      return lou_ast_expr_new_call(parser->mempool, lou_parser_slice(parser, expr->slice), expr, args);
    }
    default: return NULL;
  }
}

lou_ast_expr_t *lou_parser_parse_expr(lou_parser_t *parser) {
  lou_token_t token = lou_parser_take(parser);
  lou_ast_expr_t *expr = NULL;
  switch (token.kind) {
    case LOU_TOKEN_IDENT: expr = lou_ast_expr_new_ident(parser->mempool, token.slice); break;
    case LOU_TOKEN_FUN: expr = lou_parser_parse_func_expr(parser, token); break;
    case LOU_TOKEN_INTEGER: expr = lou_ast_expr_new_integer(parser->mempool, token.slice, token.integer); break;
    case LOU_TOKEN_CHAR: expr = lou_ast_expr_new_char(parser->mempool, token.slice, token.character); break;
    case LOU_TOKEN_STRING: expr = lou_ast_expr_new_string(parser->mempool, token.slice, token.string.content, token.string.kind); break;
    case LOU_TOKEN_OPENING_SQUARE_BRACE: {
      lou_ast_expr_t **values = LOU_MEMPOOL_VEC_NEW(parser->mempool, lou_ast_expr_t*);

      if (!lou_parser_take_if(parser, LOU_TOKEN_CLOSING_SQUARE_BRACE)) {
        do {
          lou_ast_expr_t *expr = lou_parser_parse_expr(parser);
          if (expr) {
            *LOU_VEC_PUSH(&values) = expr;
          }
        } while (!lou_parser_is_list_end(parser, LOU_TOKEN_CLOSING_SQUARE_BRACE));
      }

      return lou_ast_expr_new_array(parser->mempool, lou_parser_slice(parser, token.slice), values);
    }
    default:
      lou_parser_err(parser, token.slice, "expected expression");
      return NULL;
  }
  while (true) {
    lou_ast_expr_t *post_expr = lou_parser_parse_post_expr(parser, expr);
    if (post_expr) {
      expr = post_expr;
    } else {
      break;
    }
  }
  return expr;
}

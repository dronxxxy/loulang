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

static inline int lou_expr_binop_level(lou_ast_binop_t binop) {
  switch (binop) {
    case LOU_AST_BINOP_EQUALS: case LOU_AST_BINOP_NOT_EQUALS:
      return 30;
    case LOU_AST_BINOP_GREATER: case LOU_AST_BINOP_LESS:
    case LOU_AST_BINOP_GREATER_OR_EQUALS: case LOU_AST_BINOP_LESS_OR_EQUALS:
      return 40;
    case LOU_AST_BINOP_ADD: case LOU_AST_BINOP_SUBTRACT:
      return 50;
    case LOU_AST_BINOP_MOD:
      return 60;
    case LOU_AST_BINOP_MULTIPLY: case LOU_AST_BINOP_DIVIDE:
      return 70;
    }
  UNREACHABLE();
}

static lou_ast_expr_t *lou_expr_binop_level_swap_left(lou_ast_expr_t *expr) {
  if (expr->binop.right->kind != LOU_AST_EXPR_BINOP) return expr;
  int left_priority = lou_expr_binop_level(expr->binop.kind);
  int right_priority = lou_expr_binop_level(expr->binop.right->binop.kind);

  if (left_priority >= right_priority) {
    // i don't wanna allocate memory(
    lou_ast_expr_t *r = expr->binop.right;

    expr->binop.right = r->binop.left;
    expr->slice = lou_slice_union(expr->binop.left->slice, expr->binop.right->slice);

    r->binop.left = lou_expr_binop_level_swap_left(expr);
    r->slice = lou_slice_union(r->binop.left->slice, r->binop.right->slice);

    return r;
  }
  return expr;
}

static inline lou_ast_expr_t *lou_parser_add_right_binop(lou_parser_t *parser, lou_ast_expr_t *left, lou_ast_binop_t binop) {
  lou_parser_take(parser);
  lou_ast_expr_t *right = NOT_NULL(lou_parser_parse_expr(parser));
  lou_ast_expr_t *result = lou_ast_expr_new_binop(parser->mempool, lou_slice_union(left->slice, right->slice), binop, left, right);
  return lou_expr_binop_level_swap_left(result);
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
    case LOU_TOKEN_PLUS: return lou_parser_add_right_binop(parser, expr, LOU_AST_BINOP_ADD);
    case LOU_TOKEN_MINUS: return lou_parser_add_right_binop(parser, expr, LOU_AST_BINOP_SUBTRACT);
    case LOU_TOKEN_STAR: return lou_parser_add_right_binop(parser, expr, LOU_AST_BINOP_MULTIPLY);
    case LOU_TOKEN_SLASH: return lou_parser_add_right_binop(parser, expr, LOU_AST_BINOP_DIVIDE);
    case LOU_TOKEN_PERCENT: return lou_parser_add_right_binop(parser, expr, LOU_AST_BINOP_MOD);
    case LOU_TOKEN_EQUALS: return lou_parser_add_right_binop(parser, expr, LOU_AST_BINOP_EQUALS);
    case LOU_TOKEN_NOT_EQUALS: return lou_parser_add_right_binop(parser, expr, LOU_AST_BINOP_NOT_EQUALS);
    case LOU_TOKEN_GREATER: return lou_parser_add_right_binop(parser, expr, LOU_AST_BINOP_GREATER);
    case LOU_TOKEN_LESS: return lou_parser_add_right_binop(parser, expr, LOU_AST_BINOP_LESS);
    case LOU_TOKEN_GREATER_OR_EQUALS: return lou_parser_add_right_binop(parser, expr, LOU_AST_BINOP_GREATER_OR_EQUALS);
    case LOU_TOKEN_LESS_OR_EQUALS: return lou_parser_add_right_binop(parser, expr, LOU_AST_BINOP_LESS_OR_EQUALS);
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

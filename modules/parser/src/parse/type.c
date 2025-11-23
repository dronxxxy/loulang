#include "type.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/parser/ast/type.h"
#include "lou/parser/parser.h"
#include "parse/expr.h"
#include "parser.h"
#include <stdbool.h>

static inline lou_ast_type_t *lou_parser_parse_func_type(lou_parser_t *parser) {
  lou_ast_type_func_arg_t *args = LOU_MEMPOOL_VEC_NEW(parser->mempool, lou_ast_type_func_arg_t);

  LOU_PARSER_EXPECT(parser, LOU_TOKEN_OPENING_CIRCLE_BRACE);

  if (!lou_parser_take_if(parser, LOU_TOKEN_CLOSING_CIRCLE_BRACE)) {
    do {
      lou_opt_slice_t name = { .has_value = false };

      if (lou_parser_peek_n(parser, 2, (lou_token_kind_t[]) { LOU_TOKEN_IDENT, LOU_TOKEN_COLON })) {
        name = (lou_opt_slice_t) {
          .has_value = true,
          .value = lou_parser_take(parser).slice,
        };
        lou_parser_take(parser);
      }

      lou_ast_type_t *type = lou_parser_parse_type(parser);
      if (type) {
        *LOU_VEC_PUSH(&args) = (lou_ast_type_func_arg_t) {
          .type = type,
          .name = name,
        };
      }
    } while (!lou_parser_is_list_end(parser, LOU_TOKEN_CLOSING_CIRCLE_BRACE));
  }

  lou_ast_type_t *returns = lou_parser_take_if(parser, LOU_TOKEN_FUNCTION_RETURNS) ? lou_parser_parse_type(parser) : NULL;

  return lou_ast_type_new_func(parser->mempool, (lou_ast_type_func_t) {
    .args = args,
    .returns = returns,
  });
}

lou_ast_type_t *lou_parser_parse_type(lou_parser_t *parser) {
  if (lou_parser_take_if(parser, LOU_TOKEN_FUN)) {
    return lou_parser_parse_func_type(parser);    
  }

  lou_ast_expr_t *expr = lou_parser_parse_expr(parser);
  if (!expr) return NULL;
  return lou_ast_type_new_expr(parser->mempool, expr);
}

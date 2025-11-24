#include "decl.h"
#include "lou/parser/ast/decl.h"
#include "lou/parser/ast/node.h"
#include "parse/expr.h"
#include "parse/type.h"
#include "parser.h"

static inline bool lou_parser_peek_decl_kind(lou_parser_t *parser, lou_ast_decl_kind_t *kind) {
  lou_token_t token = lou_parser_peek(parser);
  switch (token.kind) {
    case LOU_TOKEN_META: *kind = LOU_AST_DECL_META; return true;
    case LOU_TOKEN_CONST: *kind = LOU_AST_DECL_CONST; return true;
    case LOU_TOKEN_FINAL: *kind = LOU_AST_DECL_FINAL; return true;
    case LOU_TOKEN_VAR: *kind = LOU_AST_DECL_VAR; return true;
    default:
      lou_parser_err(parser, token.slice, "expected declaration kind (meta, const, final or var)");
      return false;
  }
}
static inline lou_ast_decl_link_kind_t lou_parser_peek_decl_link_kind(lou_parser_t *parser) {
  lou_token_t token = lou_parser_peek(parser);
  switch (token.kind) {
    case LOU_TOKEN_GLOBAL:
      lou_parser_take(parser);
      return LOU_AST_DECL_LINK_GLOBAL;

    case LOU_TOKEN_EXTERN:
      lou_parser_take(parser);
      return LOU_AST_DECL_LINK_EXTERN;

    default:
      return LOU_AST_DECL_LINK_NONE;
  }
}

lou_ast_node_t *lou_parser_parse_decl_node(lou_parser_t *parser) {
  bool is_public = lou_parser_take_if(parser, LOU_TOKEN_PUBLIC);
  bool link_kind = lou_parser_peek_decl_link_kind(parser);

  lou_ast_decl_kind_t kind;
  if (!lou_parser_peek_decl_kind(parser, &kind)) return NULL;
  lou_parser_take(parser);

  lou_slice_t name = LOU_PARSER_EXPECT(parser, LOU_TOKEN_IDENT).slice;

  lou_ast_type_t *type = lou_parser_take_if(parser, LOU_TOKEN_COLON) ? lou_parser_parse_type(parser) : NULL;
  lou_ast_expr_t *initializer = lou_parser_take_if(parser, LOU_TOKEN_ASSIGN) ? lou_parser_parse_expr(parser) : NULL;

  return lou_ast_node_new_decl(parser->mempool, (lou_ast_decl_t) {
    .is_public = is_public,
    .link_kind = link_kind,
    .kind = kind,
    .name = name,
    .type = type,
    .initializer = initializer,
  });
}


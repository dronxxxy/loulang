#include "body.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/lexer/token.h"
#include "lou/parser/ast/body.h"
#include "parse/stmt.h"
#include "parser.h"

lou_ast_body_t *lou_parser_parse_body(lou_parser_t *parser) {
  lou_ast_stmt_t **stmts = LOU_MEMPOOL_VEC_NEW(parser->mempool, lou_ast_stmt_t*);
  LOU_PARSER_EXPECT(parser, LOU_TOKEN_OPENING_FIGURE_BRACE);
  while (!lou_parser_take_if(parser, LOU_TOKEN_CLOSING_FIGURE_BRACE)) {
    lou_ast_stmt_t *stmt = lou_parser_parse_stmt(parser);
    if (stmt) {
      *LOU_VEC_PUSH(&stmts) = stmt;
    }
    if (lou_parser_take_if(parser, LOU_TOKEN_SEMICOLON) && lou_parser_is_nl(parser)) {
      lou_parser_warn(parser, parser->last_slice, "reduntant semicolon");
    }
  }
  return lou_ast_body_new(parser->mempool, stmts);
}


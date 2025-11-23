#include "body.h"
#include "lou/lexer/token.h"
#include "lou/parser/ast/body.h"
#include "parser.h"

lou_ast_body_t *lou_parser_parse_body(lou_parser_t *parser) {
  LOU_PARSER_EXPECT(parser, LOU_TOKEN_OPENING_FIGURE_BRACE);
  LOU_PARSER_EXPECT(parser, LOU_TOKEN_CLOSING_FIGURE_BRACE);
  return lou_ast_body_new(parser->mempool);
}


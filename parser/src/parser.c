#include "parser.h"
#include "lou/core/vec.h"
#include "lou/lexer/lexer.h"
#include "lou/lexer/token.h"
#include "lou/parser/parser.h"
#include <stdarg.h>

void lou_parser_err(lou_parser_t *parser, lou_slice_t slice, const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  lou_lexer_log_error(parser->lexer, slice, fmt, list);
  va_end(list);

  parser->failed = true;
}

static inline lou_token_t lou_parser_queued(lou_parser_t *parser, lou_token_t token) {
  *LOU_VEC_PUSH(&parser->queue) = token;
  return token;
}

lou_token_t lou_parser_take(lou_parser_t *parser) {
  if (lou_vec_len(parser->queue) != 0) {
    lou_token_t token = parser->queue[0];
    LOU_VEC_UNSHIFT(&parser->queue);
    return token;
  }
  return lou_lexer_next(parser->lexer);
}

lou_token_t lou_parser_peek(lou_parser_t *parser) {
  if (lou_vec_len(parser->queue) != 0) {
    return parser->queue[0];
  }
  return lou_parser_queued(parser, lou_lexer_next(parser->lexer));
}

bool lou_parser_expect(lou_parser_t *parser, lou_token_kind_t kind, lou_token_t *output) {
  lou_token_t token = lou_parser_peek(parser);
  if (token.kind == kind) {
    *output = lou_parser_take(parser);
    return true;
  }
  lou_token_t reference = lou_token_new_simple(lou_slice_from_cstr("_"), kind);
  lou_parser_err(parser, token.slice, "expected #T", &reference);
  return false;
}

bool lou_parser_take_if(lou_parser_t *parser, lou_token_kind_t kind) {
  if (lou_parser_peek(parser).kind == kind) {
    lou_parser_take(parser);
    return true;
  }
  return false;
}

bool lou_parser_finished(lou_parser_t *parser) {
  return lou_parser_peek(parser).kind == LOU_TOKEN_EOI;
}

#include "lou/core/mempool.h"
#include "lou/lexer/lexer.h"
#include "lou/parser/parser.h"
#include <stdbool.h>

typedef struct lou_parser_t {
  lou_mempool_t *mempool;
  lou_lexer_t *lexer;
  lou_token_t *queue;
  bool failed;
} lou_parser_t;

void lou_parser_err(lou_parser_t *parser, lou_slice_t slice, const char *fmt, ...);

lou_token_t lou_parser_take(lou_parser_t *parser);
lou_token_t lou_parser_peek(lou_parser_t *parser);
bool lou_parser_take_if(lou_parser_t *parser, lou_token_kind_t kind);
bool lou_parser_finished(lou_parser_t *parser);

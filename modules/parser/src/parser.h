#include "lou/core/mempool.h"
#include "lou/lexer/lexer.h"
#include "lou/parser/parser.h"
#include <stdbool.h>

typedef struct lou_parser_t {
  lou_mempool_t *mempool;
  lou_lexer_t *lexer;
  lou_token_t *queue;
  size_t last_line;
  bool failed;
} lou_parser_t;

void lou_parser_err(lou_parser_t *parser, lou_slice_t slice, const char *fmt, ...);

lou_token_t lou_parser_take(lou_parser_t *parser);
lou_token_t lou_parser_peek(lou_parser_t *parser);
bool lou_parser_expect(lou_parser_t *parser, lou_token_kind_t kind, lou_token_t *output);
bool lou_parser_take_if(lou_parser_t *parser, lou_token_kind_t kind);
void lou_parser_skip_to(lou_parser_t *parser, lou_token_kind_t kind);
bool lou_parser_finished(lou_parser_t *parser);
bool lou_parser_is_list_end(lou_parser_t *parser, lou_token_kind_t kind);
bool lou_parser_is_nl(lou_parser_t *parser);
bool lou_parser_peek_n(lou_parser_t *parser, size_t length, const lou_token_kind_t *kinds);

#define LOU_PARSER_EXPECT(PARSER, KIND) ({ \
  lou_token_t __token; \
  if (!lou_parser_expect(PARSER, KIND, &__token)) return NULL; \
  __token; \
})


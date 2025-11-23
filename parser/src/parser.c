#include "lou/parser/parser.h"
#include "lou/core/mempool.h"
#include "lou/lexer/lexer.h"

typedef struct lou_parser_t {
  lou_mempool_t *mempool;
  lexer_t *lexer;
  bool failed;
} lou_parser_t;

lou_parser_t *lou_parser_new(lou_slice_t path) {
  lexer_t *lexer = lexer_new(path);
  if (!lexer) {
    return NULL;
  }
  lou_mempool_t *mempool = lou_mempool_new();
  lou_parser_t *parser = LOU_MEMPOOL_ALLOC(mempool, lou_parser_t);
  parser->mempool = mempool;
  parser->lexer = lexer;
  parser->failed = false;
  return parser;
}

void lou_parser_free(lou_parser_t *parser) {
  lexer_free(parser->lexer);
  lou_mempool_free(parser->mempool);
}

bool lou_parser_failed(const lou_parser_t *parser) {
  return lexer_failed(parser->lexer) || parser->failed;
}

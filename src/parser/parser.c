#include "parser.h"
#include "core/mempool.h"
#include "lexer/lexer.h"

typedef struct parser_t {
  mempool_t *mempool;
  lexer_t *lexer;
  bool failed;
} parser_t;

parser_t *parser_new(slice_t path) {
  lexer_t *lexer = lexer_new(path);
  if (!lexer) {
    return NULL;
  }
  mempool_t *mempool = mempool_new();
  parser_t *parser = MEMPOOL_ALLOC(mempool, parser_t);
  parser->mempool = mempool;
  parser->lexer = lexer;
  parser->failed = false;
  return parser;
}

void parser_free(parser_t *parser) {
  lexer_free(parser->lexer);
  mempool_free(parser->mempool);
}

bool parser_failed(const parser_t *parser) {
  return lexer_failed(parser->lexer) || parser->failed;
}

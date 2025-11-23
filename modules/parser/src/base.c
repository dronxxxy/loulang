#include "lou/parser/parser.h"
#include "lou/core/mempool.h"
#include "lou/lexer/lexer.h"
#include "parse/node.h"
#include "parser.h"
#include <assert.h>

lou_parser_t *lou_parser_new(lou_slice_t path) {
  lou_lexer_t *lexer = lou_lexer_new(path);
  if (!lexer) {
    return NULL;
  }
  lou_mempool_t *mempool = lou_mempool_new();
  lou_parser_t *parser = LOU_MEMPOOL_ALLOC(mempool, lou_parser_t);
  parser->mempool = mempool;
  parser->queue = LOU_MEMPOOL_VEC_NEW(mempool, lou_token_t);
  parser->lexer = lexer;
  parser->failed = false;
  return parser;
}

void lou_parser_free(lou_parser_t *parser) {
  lou_lexer_free(parser->lexer);
  lou_mempool_free(parser->mempool);
}

bool lou_parser_failed(const lou_parser_t *parser) {
  return lou_lexer_failed(parser->lexer) || parser->failed;
}

lou_ast_node_t *lou_parser_next(lou_parser_t *parser) {
  while (!lou_parser_finished(parser)) {
    lou_ast_node_t *node = lou_parser_parse_node(parser);
    if (node) {
      return node;
    }
    assert(parser->failed);
  }
  return NULL;
}


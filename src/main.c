#include "log.h"
#include "lou/parser/parser.h"

int main(int argc, char** argv) {
  log_init();

  lou_parser_t *parser = lou_parser_new(lou_slice_from_cstr("./examples/test/lexer.lou"));
  if (!parser) return 1;

  while (lou_parser_next(parser));

  int status = lou_parser_failed(parser) ? 1 : 0;
  lou_parser_free(parser);
  return status;
}

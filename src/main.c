#include "log.h"
#include "parser/parser.h"

int main(int argc, char** argv) {
  log_init();

  parser_t *parser = parser_new(slice_from_cstr("./examples/test/lexer.lou"));
  if (!parser) {
    return 1;
  }

  int status = parser_failed(parser) ? 1 : 0;
  parser_free(parser);
  return status;
}

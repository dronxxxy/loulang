#include "log.h"
#include "lou/sema/sema.h"

int main(int argc, char** argv) {
  log_init();

  lou_sema_t *sema = lou_sema_new(lou_slice_from_cstr("./examples/test/lexer.lou"));
  if (!sema) return 1;
  lou_sema_read(sema);
  lou_sema_analyze(sema);

  int status = lou_sema_failed(sema) ? 1 : 0;
  lou_sema_free(sema);
  return status;
}

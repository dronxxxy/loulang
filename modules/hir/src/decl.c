#include "lou/hir/decl.h"

void lou_hir_decl_init(lou_hir_decl_t *decl, lou_hir_const_t *initializer) {
  assert(!decl->initializer);
  decl->initializer = initializer;
}

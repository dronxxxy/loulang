#include "lou/hir/decl.h"

void lou_hir_decl_init_extern(lou_hir_decl_t *decl, lou_hir_decl_extern_t external) {
  decl->initialized = true;
  decl->kind = LOU_HIR_DECL_EXTERN;
  decl->external = external;
}

void lou_hir_decl_init_global(lou_hir_decl_t *decl, lou_hir_decl_global_t global) {
  decl->initialized = true;
  decl->kind = LOU_HIR_DECL_GLOBAL;
  decl->global = global;
}

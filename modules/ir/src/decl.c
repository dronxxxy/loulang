#include "lou/ir/decl.h"

void lou_ir_decl_init_extern(lou_ir_decl_t *decl, lou_ir_decl_extern_t external) {
  decl->initialized = true;
  decl->kind = LOU_IR_DECL_EXTERN;
  decl->external = external;
}

void lou_ir_decl_init_global(lou_ir_decl_t *decl, lou_ir_decl_global_t global) {
  decl->initialized = true;
  decl->kind = LOU_IR_DECL_GLOBAL;
  decl->global = global;
}

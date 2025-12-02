#include "builtins.h"
#include "builtin/extern.h"
#include "builtin/global.h"
#include "builtin/fun.h"
#include "lou/core/slice.h"
#include "plugin.h"
#include "sema.h"
#include "type.h"
#include "value.h"

static inline void lou_sema_add_type(lou_sema_t *sema, lou_slice_t name, lou_sema_type_t *type) {
  lou_sema_init_decl(
    lou_sema_add_decl(sema, false, name, LOU_SEMA_DECL_META, NULL),
    lou_sema_value_new_type(sema->mempool, type)
  );
}

static inline void lou_sema_add_int(lou_sema_t *sema, const char *name, lou_sema_int_size_t size, bool is_signed) {
  lou_sema_add_type(sema, lou_slice_from_cstr(name), lou_sema_type_new_integer(sema->mempool, (lou_sema_type_int_t) {
    .is_signed = is_signed,
    .size = size,
  }));
}

void lou_sema_apply_builtins(lou_sema_t *sema) {
  lou_sema_add_int(sema, "u8", LOU_SEMA_INT_8, false);
  lou_sema_add_int(sema, "i8", LOU_SEMA_INT_8, true);
  lou_sema_add_int(sema, "u16", LOU_SEMA_INT_16, false);
  lou_sema_add_int(sema, "i16", LOU_SEMA_INT_16, true);
  lou_sema_add_int(sema, "u32", LOU_SEMA_INT_32, false);
  lou_sema_add_int(sema, "i32", LOU_SEMA_INT_32, true);
  lou_sema_add_int(sema, "u64", LOU_SEMA_INT_64, false);
  lou_sema_add_int(sema, "i64", LOU_SEMA_INT_64, true);

  lou_sema_init_decl(
    lou_sema_add_decl(sema, false, lou_slice_from_cstr("@global"), LOU_SEMA_DECL_META, NULL),
    lou_sema_value_new_plugin(sema->mempool, lou_sema_plugin_new(sema->mempool, lou_global_builtin))
  );
  lou_sema_init_decl(
    lou_sema_add_decl(sema, false, lou_slice_from_cstr("@extern"), LOU_SEMA_DECL_META, NULL),
    lou_sema_value_new_plugin(sema->mempool, lou_sema_plugin_new(sema->mempool, lou_extern_builtin))
  );
  lou_sema_init_decl(
    lou_sema_add_decl(sema, false, lou_slice_from_cstr("@procFun"), LOU_SEMA_DECL_META, NULL),
    lou_sema_value_new_plugin(sema->mempool, lou_sema_plugin_new(sema->mempool, lou_fun_builtin))
  );
}

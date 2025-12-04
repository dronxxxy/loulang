#include "builtins.h"
#include "builtin/extern.h"
#include "builtin/global.h"
#include "builtin/fun.h"
#include "builtin/ptr.h"
#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/core/vec.h"
#include "plugin.h"
#include "sema.h"
#include "type.h"
#include "value.h"
#include <stdarg.h>

static inline void lou_sema_add_type(lou_sema_t *sema, lou_slice_t name, lou_sema_type_t *type) {
  lou_sema_init_decl(
    sema,
    name,
    lou_sema_add_decl(sema, false, name, LOU_SEMA_DECL_META, NULL),
    lou_sema_value_new_type(sema->mempool, type)
  );
}

static inline void lou_sema_add_int(lou_sema_t *sema, const char *name, lou_sema_int_size_t size, bool is_signed) {
  lou_sema_add_type(sema, lou_slice_from_cstr(name), lou_sema_type_new_integer(sema->mempool, size, is_signed));
}

static inline void lou_sema_add_plugin(lou_sema_t *sema, const char *name, lou_sema_plugin_t *plugin) {
  lou_slice_t slice = lou_slice_from_cstr(name);
  lou_sema_init_decl(
    sema,
    slice,
    lou_sema_add_decl(sema, false, slice, LOU_SEMA_DECL_META, NULL),
    lou_sema_value_new_plugin(sema->mempool, plugin)
  );
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

  lou_sema_add_plugin(sema, "@global", lou_sema_plugin_new(sema->mempool, lou_global_builtin, ({
    lou_sema_type_t **args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_type_t*);
    *LOU_VEC_PUSH(&args) = lou_sema_type_new_string(sema->mempool);
    *LOU_VEC_PUSH(&args) = NULL;
    args;
  })));
  lou_sema_add_plugin(sema, "@externFun", lou_sema_plugin_new(sema->mempool, lou_extern_fun_builtin, ({
    lou_sema_type_t **args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_type_t*);
    *LOU_VEC_PUSH(&args) = lou_sema_type_new_string(sema->mempool);
    *LOU_VEC_PUSH(&args) = NULL;
    args;
  })));
  lou_sema_add_plugin(sema, "@externVar", lou_sema_plugin_new(sema->mempool, lou_extern_var_builtin, ({
    lou_sema_type_t **args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_type_t*);
    *LOU_VEC_PUSH(&args) = lou_sema_type_new_string(sema->mempool);
    *LOU_VEC_PUSH(&args) = NULL;
    args;
  })));
  lou_sema_add_plugin(sema, "@procFun", lou_sema_plugin_new(sema->mempool, lou_fun_builtin, ({
    lou_sema_type_t **args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_type_t*);
    *LOU_VEC_PUSH(&args) = NULL;
    args;
  })));
  lou_sema_add_plugin(sema, "@retFun", lou_sema_plugin_new(sema->mempool, lou_ret_fun_builtin, ({
    lou_sema_type_t **args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_type_t*);
    *LOU_VEC_PUSH(&args) = NULL;
    args;
  })));
  lou_sema_add_plugin(sema, "@ptr", lou_sema_plugin_new(sema->mempool, lou_ptr_builtin, ({
    lou_sema_type_t **args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_type_t*);
    *LOU_VEC_PUSH(&args) = NULL;
    args;
  })));
}

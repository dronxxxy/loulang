#include "setup.h"
#include "impl.h"
#include "type.h"
#include "value.h"
#include "plugins/extern.h"
#include "plugins/fun.h"
#include "plugins/global.h"
#include "plugins/ptr.h"
#include "plugins/int_cast.h"
#include "plugins/array.h"
#include "plugins/deref.h"

void lou_sema_builtin_setup(lou_sema_t *sema) {
  lou_sema_add_internal_decl(sema, "u8", lou_sema_value_new_type(sema->mempool, lou_sema_type_new_int(sema->mempool, LOU_SEMA_INT_8, false)));
  lou_sema_add_internal_decl(sema, "i8", lou_sema_value_new_type(sema->mempool, lou_sema_type_new_int(sema->mempool, LOU_SEMA_INT_8, true)));
  lou_sema_add_internal_decl(sema, "u16", lou_sema_value_new_type(sema->mempool, lou_sema_type_new_int(sema->mempool, LOU_SEMA_INT_16, false)));
  lou_sema_add_internal_decl(sema, "i16", lou_sema_value_new_type(sema->mempool, lou_sema_type_new_int(sema->mempool, LOU_SEMA_INT_16, true)));
  lou_sema_add_internal_decl(sema, "u32", lou_sema_value_new_type(sema->mempool, lou_sema_type_new_int(sema->mempool, LOU_SEMA_INT_32, false)));
  lou_sema_add_internal_decl(sema, "i32", lou_sema_value_new_type(sema->mempool, lou_sema_type_new_int(sema->mempool, LOU_SEMA_INT_32, true)));
  lou_sema_add_internal_decl(sema, "u64", lou_sema_value_new_type(sema->mempool, lou_sema_type_new_int(sema->mempool, LOU_SEMA_INT_64, false)));
  lou_sema_add_internal_decl(sema, "i64", lou_sema_value_new_type(sema->mempool, lou_sema_type_new_int(sema->mempool, LOU_SEMA_INT_64, true)));
  lou_sema_add_internal_decl(sema, "bool", lou_sema_value_new_type(sema->mempool, lou_sema_type_new_bool(sema->mempool)));

  lou_sema_add_internal_decl(sema, "@externFun", lou_sema_value_new_plugin(sema->mempool, lou_sema_builtin_extern_fun(sema->mempool)));
  lou_sema_add_internal_decl(sema, "@retFun", lou_sema_value_new_plugin(sema->mempool, lou_sema_builtin_ret_fun(sema->mempool)));
  lou_sema_add_internal_decl(sema, "@procFun", lou_sema_value_new_plugin(sema->mempool, lou_sema_builtin_proc_fun(sema->mempool)));
  lou_sema_add_internal_decl(sema, "@global", lou_sema_value_new_plugin(sema->mempool, lou_sema_builtin_global(sema->mempool)));
  lou_sema_add_internal_decl(sema, "@ptr", lou_sema_value_new_plugin(sema->mempool, lou_sema_builtin_ptr(sema->mempool)));
  lou_sema_add_internal_decl(sema, "@intCast", lou_sema_value_new_plugin(sema->mempool, lou_sema_builtin_int_cast(sema->mempool)));
  lou_sema_add_internal_decl(sema, "@array", lou_sema_value_new_plugin(sema->mempool, lou_sema_builtin_array(sema->mempool)));
  lou_sema_add_internal_decl(sema, "@arrayAt", lou_sema_value_new_plugin(sema->mempool, lou_sema_builtin_array_at(sema->mempool)));
  lou_sema_add_internal_decl(sema, "@deref", lou_sema_value_new_plugin(sema->mempool, lou_sema_builtin_deref(sema->mempool)));
  lou_sema_add_internal_decl(sema, "@ptrAt", lou_sema_value_new_plugin(sema->mempool, lou_sema_builtin_ptr_at(sema->mempool)));
}

#include "builtins.h"
#include "builtin/global.h"
#include "lou/core/slice.h"
#include "plugin.h"
#include "sema.h"
#include "value.h"

void lou_sema_apply_builtins(lou_sema_t *sema) {
  lou_sema_init_decl(
    lou_sema_add_decl(sema, false, lou_slice_from_cstr("@global"), LOU_SEMA_DECL_META, NULL),
    lou_sema_value_new_plugin(sema->mempool, lou_sema_plugin_new(sema->mempool, lou_global_builtin))
  );
}

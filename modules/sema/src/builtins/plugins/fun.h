#pragma once

#include "plugin.h"

// TODO: tuples
//       - @fun((t1, t1, t2))
//       - @fun((t1, t1, t2), t3)
lou_sema_plugin_t *lou_sema_builtin_ret_fun(lou_mempool_t *mempool);
lou_sema_plugin_t *lou_sema_builtin_proc_fun(lou_mempool_t *mempool);


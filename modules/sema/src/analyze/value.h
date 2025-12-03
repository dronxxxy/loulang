#pragma once

#include "lou/hir/code.h"
#include "sema.h"

lou_hir_value_t *lou_sema_emit_value(lou_mempool_t *mempool, lou_sema_value_t *value);

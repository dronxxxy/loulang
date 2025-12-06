#pragma once

#include "lou/hir/code.h"
#include "impl.h"

lou_hir_value_t *lou_sema_emit_value(lou_mempool_t *mempool, lou_sema_value_t *value);
lou_hir_const_t *lou_sema_emit_const(lou_mempool_t *mempool, lou_sema_const_t *constant);
lou_hir_type_t *lou_sema_emit_type(lou_mempool_t *mempool, lou_sema_type_t *type);

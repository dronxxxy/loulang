#pragma once

#include "lou/core/mempool.h"

typedef struct lou_ir_t lou_ir_t;
typedef struct lou_ir_decl_t lou_ir_decl_t;

lou_ir_t *lou_ir_new(lou_mempool_t *mempool);
void lou_ir_decl_add(lou_ir_t *ir, lou_ir_decl_t *decl);


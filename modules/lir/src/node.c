#include "lir/node.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "node.h"
#include <stdarg.h>

static inline lir_node_t *lir_node_new(
  lou_mempool_t *mempool, lir_node_kind_t kind,
  lir_node_t *control,
  lir_node_t *memory,
  ...
) {
  lir_node_t **inputs = LOU_MEMPOOL_VEC_NEW(mempool, lir_node_t*); 
  lir_node_t *node = LOU_MEMPOOL_ALLOC(mempool, lir_node_t);

  va_list list;
  va_start(list, memory);
  for (lir_node_t *input = va_arg(list, lir_node_t*); input; input = va_arg(list, lir_node_t*)) {
    *(LOU_VEC_PUSH(&inputs)) = input;
    *LOU_VEC_PUSH(&input->uses) = node;
  }
  va_end(list);

  if (control) *LOU_VEC_PUSH(&control->controlUses) = node;
  if (memory) *LOU_VEC_PUSH(&memory->memoryUses) = node;

  node->kind = kind;
  node->inputs = inputs;
  node->control = control;
  node->memory = memory;
  node->uses = LOU_MEMPOOL_VEC_NEW(mempool, lir_node_t*);
  node->controlUses = LOU_MEMPOOL_VEC_NEW(mempool, lir_node_t*);
  node->memoryUses = LOU_MEMPOOL_VEC_NEW(mempool, lir_node_t*);

  return node;
}

lir_node_t *lir_node_new_func(lou_mempool_t *mempool) {
  return lir_node_new(mempool, LIR_NODE_FUNCTION,
    NULL, NULL,
    NULL
  );
}

lir_node_t *lir_node_new_return(lou_mempool_t *mempool, lir_node_t *control, lir_node_t *value) {
  return lir_node_new(mempool, LIR_NODE_RETURN,
    control, value,
    NULL
  );
}

lir_node_t *lir_node_new_const(lou_mempool_t *mempool, lir_const_t *constant) {
  lir_node_t *node = lir_node_new(mempool, LIR_NODE_CONST,
    NULL, NULL,
    NULL
  );
  node->constant = constant;
  return node;
}


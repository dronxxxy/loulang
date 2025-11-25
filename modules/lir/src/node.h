#pragma once

typedef struct lir_node_t lir_node_t;
typedef struct lir_const_t lir_const_t;

typedef enum {
  LIR_NODE_FUNCTION,
  LIR_NODE_RETURN,
  LIR_NODE_CONST,
} lir_node_kind_t;

typedef struct lir_node_t {
  lir_node_kind_t kind;
  lir_node_t *control;
  lir_node_t *memory;
  lir_node_t **inputs;

  lir_node_t **uses;
  lir_node_t **controlUses;
  lir_node_t **memoryUses;

  union {
    lir_const_t *constant;
  };
} lir_node_t;

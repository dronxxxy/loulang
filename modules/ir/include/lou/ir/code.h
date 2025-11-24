#pragma once

#include "lou/core/mempool.h"
#include <stddef.h>

typedef struct lou_ir_local_t lou_ir_local_t;
typedef struct lou_ir_value_t lou_ir_value_t;

typedef struct {
  lou_ir_value_t *value;
} lou_ir_stmt_ret_t;

typedef struct {
  lou_ir_local_t *output;
  lou_ir_value_t *callable;
  lou_ir_value_t **args;
} lou_ir_stmt_call_t;

typedef enum {
  LOU_IR_STMT_CALL,
  LOU_IR_STMT_RETURN,
} lou_ir_stmt_kind_t;

typedef struct {
  lou_ir_stmt_kind_t kind;

  union {
    lou_ir_stmt_ret_t ret;
    lou_ir_stmt_call_t call;
  };
} lou_ir_stmt_t;

static inline lou_ir_stmt_t *lou_ir_stmt_new_call(lou_mempool_t *mempool, lou_ir_stmt_call_t call)  {
  lou_ir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_ir_stmt_t);
  stmt->kind = LOU_IR_STMT_CALL;
  stmt->call = call;
  return stmt;
}

static inline lou_ir_stmt_t *lou_ir_stmt_new_ret(lou_mempool_t *mempool, lou_ir_stmt_ret_t ret)  {
  lou_ir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_ir_stmt_t);
  stmt->kind = LOU_IR_STMT_RETURN;
  stmt->ret = ret;
  return stmt;
}

typedef struct lou_ir_code_t {
  lou_ir_stmt_t **stmts;
} lou_ir_code_t;

static inline lou_ir_code_t *lou_ir_code_new(lou_mempool_t *mempool, lou_ir_stmt_t **stmts)  {
  lou_ir_code_t *code = LOU_MEMPOOL_ALLOC(mempool, lou_ir_code_t);
  code->stmts = stmts;
  return code;
}

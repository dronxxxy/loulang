#pragma once

#include "lou/core/mempool.h"
#include <stddef.h>

typedef struct lou_hir_local_t lou_hir_local_t;
typedef struct lou_hir_value_t lou_hir_value_t;

typedef struct {
  lou_hir_value_t *value;
} lou_hir_stmt_ret_t;

typedef struct {
  lou_hir_local_t *output;
  size_t num;
} lou_hir_stmt_arg_t;

typedef struct {
  lou_hir_local_t *output;
  lou_hir_value_t *callable;
  lou_hir_value_t **args;
} lou_hir_stmt_call_t;

typedef enum {
  LOU_HIR_STMT_CALL,
  LOU_HIR_STMT_RETURN,
  LOU_HIR_STMT_ARG,
} lou_hir_stmt_kind_t;

typedef struct {
  lou_hir_stmt_kind_t kind;

  union {
    lou_hir_stmt_ret_t ret;
    lou_hir_stmt_call_t call;
    lou_hir_stmt_arg_t arg;
  };
} lou_hir_stmt_t;

static inline lou_hir_stmt_t *lou_hir_stmt_new_arg(lou_mempool_t *mempool, size_t num, lou_hir_stmt_arg_t arg)  {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_ARG;
  stmt->arg = arg;
  return stmt;
}

static inline lou_hir_stmt_t *lou_hir_stmt_new_call(lou_mempool_t *mempool, lou_hir_stmt_call_t call)  {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_CALL;
  stmt->call = call;
  return stmt;
}

static inline lou_hir_stmt_t *lou_hir_stmt_new_ret(lou_mempool_t *mempool, lou_hir_stmt_ret_t ret)  {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_RETURN;
  stmt->ret = ret;
  return stmt;
}

typedef struct lou_hir_code_t {
  lou_hir_stmt_t **stmts;
} lou_hir_code_t;

static inline lou_hir_code_t *lou_hir_code_new(lou_mempool_t *mempool, lou_hir_stmt_t **stmts)  {
  lou_hir_code_t *code = LOU_MEMPOOL_ALLOC(mempool, lou_hir_code_t);
  code->stmts = stmts;
  return code;
}

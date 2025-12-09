#pragma once

#include "lou/core/mempool.h"
#include "lou/hir/const.h"
#include "lou/hir/mutability.h"
#include <stddef.h>

typedef struct lou_hir_local_t lou_hir_local_t;
typedef struct lou_hir_value_t lou_hir_value_t;
typedef struct lou_hir_code_t lou_hir_code_t;

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
  LOU_HIR_BINOP_ARITHM_INT_MOD,
} lou_hir_binop_arithm_int_t;

typedef enum {
  LOU_HIR_BINOP_ARITHM_ADD,
  LOU_HIR_BINOP_ARITHM_SUBTRACT,
  LOU_HIR_BINOP_ARITHM_MULTIPLY,
  LOU_HIR_BINOP_ARITHM_DIVIDE,
} lou_hir_binop_arithm_t;

typedef enum {
  LOU_HIR_BINOP_ARITHM,
  LOU_HIR_BINOP_ARITHM_INT,
} lou_hir_binop_kind_t;

typedef enum {
  LOU_HIR_STMT_CALL,
  LOU_HIR_STMT_RETURN,
  LOU_HIR_STMT_ARG,
  LOU_HIR_STMT_CODE,
  LOU_HIR_STMT_STORE_VAR,
  LOU_HIR_STMT_BINOP,
} lou_hir_stmt_kind_t;

typedef struct lou_hir_local_t {
  lou_hir_mutability_t mutability;
  lou_hir_type_t *type;

  void *codegen;
} lou_hir_local_t;

typedef struct {
  lou_hir_stmt_kind_t kind;

  union {
    lou_hir_stmt_ret_t ret;
    lou_hir_stmt_call_t call;
    lou_hir_stmt_arg_t arg;
    lou_hir_code_t *code;

    struct {
      lou_hir_local_t *output;
      lou_hir_value_t *value;
    } store_var;

    struct {
      lou_hir_binop_kind_t kind;
      lou_hir_value_t *left;
      lou_hir_value_t *right;
      lou_hir_local_t *output;

      union {
        lou_hir_binop_arithm_int_t arithm_int;
        lou_hir_binop_arithm_t arithm;
      };
    } binop;
  };
} lou_hir_stmt_t;

lou_hir_stmt_t *lou_hir_stmt_new_arg(lou_mempool_t *mempool, size_t num, lou_hir_stmt_arg_t arg);
lou_hir_stmt_t *lou_hir_stmt_new_call(lou_mempool_t *mempool, lou_hir_local_t *output, lou_hir_value_t *callable, lou_hir_value_t **args);
lou_hir_stmt_t *lou_hir_stmt_new_ret(lou_mempool_t *mempool, lou_hir_value_t *value);
lou_hir_stmt_t *lou_hir_stmt_new_code(lou_mempool_t *mempool, lou_hir_code_t *code);
lou_hir_stmt_t *lou_hir_stmt_new_store_var(lou_mempool_t *mempool, lou_hir_local_t *output, lou_hir_value_t *value);
lou_hir_stmt_t *lou_hir_stmt_new_binop_arithm_int(
  lou_mempool_t *mempool,
  lou_hir_binop_arithm_int_t kind,
  lou_hir_local_t *output,
  lou_hir_value_t *left, lou_hir_value_t *right
);
lou_hir_stmt_t *lou_hir_stmt_new_binop_arithm(
  lou_mempool_t *mempool,
  lou_hir_binop_arithm_t kind,
  lou_hir_local_t *output,
  lou_hir_value_t *left, lou_hir_value_t *right
);

typedef struct lou_hir_code_t {
  lou_hir_stmt_t **stmts;
  lou_hir_local_t **locals;
} lou_hir_code_t;

lou_hir_code_t *lou_hir_code_new(lou_mempool_t *mempool);
void lou_hir_code_append_stmt(lou_hir_code_t *code, lou_hir_stmt_t *stmt);
lou_hir_local_t *lou_hir_code_local_add(lou_mempool_t *mempool, lou_hir_code_t *code, lou_hir_mutability_t mutability, lou_hir_type_t *type);

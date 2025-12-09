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
  LOU_HIR_BINOP_ARITHM_OF_INT,
} lou_hir_binop_arithm_of_t;

typedef enum {
  LOU_HIR_BINOP_ARITHM_ADD,
  LOU_HIR_BINOP_ARITHM_SUBTRACT,
  LOU_HIR_BINOP_ARITHM_MULTIPLY,
  LOU_HIR_BINOP_ARITHM_DIVIDE,
} lou_hir_binop_arithm_t;

typedef enum {
  LOU_HIR_BINOP_ORDER_OF_INT,
} lou_hir_binop_order_of_t;

typedef enum {
  LOU_HIR_BINOP_ORDER_GREATER,
  LOU_HIR_BINOP_ORDER_LESS,
  LOU_HIR_BINOP_ORDER_GREATER_OR_EQUALS,
  LOU_HIR_BINOP_ORDER_LESS_OR_EQUALS,
} lou_hir_binop_order_t;

typedef enum {
  LOU_HIR_BINOP_EQ_OF_BOOL,
  LOU_HIR_BINOP_EQ_OF_INT,
} lou_hir_binop_eq_of_t;

typedef enum {
  LOU_HIR_BINOP_EQUALS,
  LOU_HIR_BINOP_NOT_EQUALS,
} lou_hir_binop_eq_t;

typedef enum {
  LOU_HIR_BINOP_ARITHM,
  LOU_HIR_BINOP_ARITHM_INT,
  LOU_HIR_BINOP_EQ,
  LOU_HIR_BINOP_ORDER,
} lou_hir_binop_kind_t;

typedef enum {
  LOU_HIR_STMT_CALL,
  LOU_HIR_STMT_RETURN,
  LOU_HIR_STMT_ARG,
  LOU_HIR_STMT_CODE,
  LOU_HIR_STMT_STORE_VAR,
  LOU_HIR_STMT_BINOP,
  LOU_HIR_STMT_COND,
  LOU_HIR_STMT_LOOP,
  LOU_HIR_STMT_BREAK,
  LOU_HIR_STMT_CONTINUE,
} lou_hir_stmt_kind_t;

typedef struct lou_hir_local_t {
  lou_hir_mutability_t mutability;
  lou_hir_type_t *type;

  void *codegen;
} lou_hir_local_t;

typedef struct {
  lou_hir_value_t *condition;
  lou_hir_code_t *code;
  lou_hir_code_t *fallback;
} lou_hir_cond_code_t;

typedef struct lou_hir_stmt_t lou_hir_stmt_t;

typedef struct lou_hir_stmt_t {
  lou_hir_stmt_kind_t kind;

  union {
    lou_hir_stmt_ret_t ret;
    lou_hir_stmt_call_t call;
    lou_hir_stmt_arg_t arg;
    lou_hir_code_t *code;
    lou_hir_cond_code_t cond;

    lou_hir_stmt_t *break_loop;
    lou_hir_stmt_t *continue_loop;

    struct {
      lou_hir_local_t *output;
      lou_hir_value_t *value;
    } store_var;

    struct {
      lou_hir_code_t *code;

      struct {
        void *begin;
        void *end;
      } codegen;
    } loop;

    struct {
      lou_hir_binop_kind_t kind;
      lou_hir_value_t *left;
      lou_hir_value_t *right;
      lou_hir_local_t *output;

      union {
        lou_hir_binop_arithm_int_t arithm_int;

        struct {
          lou_hir_binop_arithm_t kind;
          lou_hir_binop_arithm_of_t of;
        } arithm;

        struct {
          lou_hir_binop_eq_t kind;
          lou_hir_binop_eq_of_t of;
        } eq;

        struct {
          lou_hir_binop_order_t kind;
          lou_hir_binop_order_of_t of;
        } order;
      };
    } binop;
  };
} lou_hir_stmt_t;

lou_hir_stmt_t *lou_hir_stmt_new_arg(lou_mempool_t *mempool, lou_hir_local_t *output, size_t num);
lou_hir_stmt_t *lou_hir_stmt_new_call(lou_mempool_t *mempool, lou_hir_local_t *output, lou_hir_value_t *callable, lou_hir_value_t **args);
lou_hir_stmt_t *lou_hir_stmt_new_ret(lou_mempool_t *mempool, lou_hir_value_t *value);
lou_hir_stmt_t *lou_hir_stmt_new_code(lou_mempool_t *mempool, lou_hir_code_t *code);
lou_hir_stmt_t *lou_hir_stmt_new_store_var(lou_mempool_t *mempool, lou_hir_local_t *output, lou_hir_value_t *value);
lou_hir_stmt_t *lou_hir_stmt_new_cond(lou_mempool_t *mempool, lou_hir_value_t *condition, lou_hir_code_t *code, lou_hir_code_t *fallback);
lou_hir_stmt_t *lou_hir_stmt_new_loop(lou_mempool_t *mempool, lou_hir_value_t *condition, lou_hir_code_t *code);
lou_hir_stmt_t *lou_hir_stmt_new_break(lou_mempool_t *mempool, lou_hir_stmt_t *break_loop);
lou_hir_stmt_t *lou_hir_stmt_new_continue(lou_mempool_t *mempool, lou_hir_stmt_t *continue_loop);
lou_hir_stmt_t *lou_hir_stmt_new_get_argument(lou_mempool_t *mempool, lou_hir_local_t *output, size_t idx);
lou_hir_stmt_t *lou_hir_stmt_new_binop_arithm_int(
  lou_mempool_t *mempool,
  lou_hir_binop_arithm_int_t kind,
  lou_hir_local_t *output,
  lou_hir_value_t *left, lou_hir_value_t *right
);
lou_hir_stmt_t *lou_hir_stmt_new_binop_arithm(
  lou_mempool_t *mempool,
  lou_hir_binop_arithm_t kind,
  lou_hir_binop_arithm_of_t of,
  lou_hir_local_t *output,
  lou_hir_value_t *left, lou_hir_value_t *right
);
lou_hir_stmt_t *lou_hir_stmt_new_binop_eq(
  lou_mempool_t *mempool,
  lou_hir_binop_eq_t kind,
  lou_hir_binop_eq_of_t of,
  lou_hir_local_t *output,
  lou_hir_value_t *left, lou_hir_value_t *right
);
lou_hir_stmt_t *lou_hir_stmt_new_binop_order(
  lou_mempool_t *mempool,
  lou_hir_binop_order_t kind,
  lou_hir_binop_order_of_t of,
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

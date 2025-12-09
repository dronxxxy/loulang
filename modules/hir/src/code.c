#include "lou/hir/code.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"

lou_hir_stmt_t *lou_hir_stmt_new_arg(lou_mempool_t *mempool, lou_hir_local_t *output, size_t num)  {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_ARG;
  stmt->arg.num = num;
  stmt->arg.output = output;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_call(lou_mempool_t *mempool, lou_hir_local_t *output, lou_hir_value_t *callable, lou_hir_value_t **args) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_CALL;
  stmt->call.output = output;
  stmt->call.callable = callable;
  stmt->call.args = args;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_ret(lou_mempool_t *mempool, lou_hir_value_t *value) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_RETURN;
  stmt->ret.value = value;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_code(lou_mempool_t *mempool, lou_hir_code_t *code) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_CODE;
  stmt->code = code;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_store_var(lou_mempool_t *mempool, lou_hir_local_t *output, lou_hir_value_t *value) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_STORE_VAR;
  stmt->store_var.output = output;
  stmt->store_var.value = value;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_binop_arithm_int(
  lou_mempool_t *mempool,
  lou_hir_binop_arithm_int_t kind,
  lou_hir_local_t *output,
  lou_hir_value_t *left, lou_hir_value_t *right
) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_BINOP;
  stmt->binop.kind = LOU_HIR_BINOP_ARITHM_INT;
  stmt->binop.output = output;
  stmt->binop.left = left;
  stmt->binop.right = right;
  stmt->binop.arithm_int = kind;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_binop_arithm(
  lou_mempool_t *mempool,
  lou_hir_binop_arithm_t kind,
  lou_hir_binop_arithm_of_t of,
  lou_hir_local_t *output,
  lou_hir_value_t *left, lou_hir_value_t *right
) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_BINOP;
  stmt->binop.kind = LOU_HIR_BINOP_ARITHM;
  stmt->binop.output = output;
  stmt->binop.left = left;
  stmt->binop.right = right;
  stmt->binop.arithm.kind = kind;
  stmt->binop.arithm.of = of;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_binop_eq(
  lou_mempool_t *mempool,
  lou_hir_binop_eq_t kind,
  lou_hir_binop_eq_of_t of,
  lou_hir_local_t *output,
  lou_hir_value_t *left, lou_hir_value_t *right
) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_BINOP;
  stmt->binop.kind = LOU_HIR_BINOP_EQ;
  stmt->binop.output = output;
  stmt->binop.left = left;
  stmt->binop.right = right;
  stmt->binop.eq.kind = kind;
  stmt->binop.eq.of = of;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_binop_order(
  lou_mempool_t *mempool,
  lou_hir_binop_order_t kind,
  lou_hir_binop_order_of_t of,
  lou_hir_local_t *output,
  lou_hir_value_t *left, lou_hir_value_t *right
) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_BINOP;
  stmt->binop.kind = LOU_HIR_BINOP_ORDER;
  stmt->binop.output = output;
  stmt->binop.left = left;
  stmt->binop.right = right;
  stmt->binop.order.kind = kind;
  stmt->binop.order.of = of;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_cond(lou_mempool_t *mempool, lou_hir_value_t *condition, lou_hir_code_t *code, lou_hir_code_t *fallback) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_COND;
  stmt->cond.condition = condition;
  stmt->cond.code = code;
  stmt->cond.fallback = fallback;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_loop(lou_mempool_t *mempool, lou_hir_value_t *condition, lou_hir_code_t *code) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_LOOP;
  stmt->loop.code = code;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_break(lou_mempool_t *mempool, lou_hir_stmt_t *break_loop) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_BREAK;
  stmt->break_loop = break_loop;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_continue(lou_mempool_t *mempool, lou_hir_stmt_t *continue_loop) {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_CONTINUE;
  stmt->continue_loop = continue_loop;
  return stmt;
}

lou_hir_code_t *lou_hir_code_new(lou_mempool_t *mempool)  {
  lou_hir_code_t *code = LOU_MEMPOOL_ALLOC(mempool, lou_hir_code_t);
  code->stmts = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_stmt_t*);
  code->locals = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_local_t*);
  return code;
}

void lou_hir_code_append_stmt(lou_hir_code_t *code, lou_hir_stmt_t *stmt) {
  *LOU_VEC_PUSH(&code->stmts) = stmt;
}

lou_hir_local_t *lou_hir_code_local_add(lou_mempool_t *mempool, lou_hir_code_t *code, lou_hir_mutability_t mutability, lou_hir_type_t *type) {
  lou_hir_local_t *local = LOU_MEMPOOL_ALLOC(mempool, lou_hir_local_t); 
  local->type = type;
  local->mutability = mutability;
  *LOU_VEC_PUSH(&code->locals) = local;
  return local;
}

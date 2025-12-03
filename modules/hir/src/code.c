#include "lou/hir/code.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"

lou_hir_stmt_t *lou_hir_stmt_new_arg(lou_mempool_t *mempool, size_t num, lou_hir_stmt_arg_t arg)  {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_ARG;
  stmt->arg = arg;
  return stmt;
}

lou_hir_stmt_t *lou_hir_stmt_new_call(lou_mempool_t *mempool, lou_hir_stmt_call_t call)  {
  lou_hir_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_hir_stmt_t);
  stmt->kind = LOU_HIR_STMT_CALL;
  stmt->call = call;
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

lou_hir_code_t *lou_hir_code_new(lou_mempool_t *mempool)  {
  lou_hir_code_t *code = LOU_MEMPOOL_ALLOC(mempool, lou_hir_code_t);
  code->stmts = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_stmt_t*);
  return code;
}

void lou_hir_code_append_stmt(lou_hir_code_t *code, lou_hir_stmt_t *stmt) {
  *LOU_VEC_PUSH(&code->stmts) = stmt;
}

lou_hir_local_t *lou_hir_code_local_add(lou_mempool_t *mempool, lou_hir_code_t *code, lou_hir_mutability_t mutability, lou_hir_type_t *type) {
  lou_hir_local_t *local = LOU_MEMPOOL_ALLOC(mempool, lou_hir_local_t); 
  local->type = type;
  local->mutability = mutability;
  // TODO: add to code in future
  return local;
}

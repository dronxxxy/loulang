#include "lou/hir/const.h"
#include "type.h"
#include <stdint.h>

typedef enum {
  LOU_SEMA_CONST_INTEGER,
  LOU_SEMA_CONST_FUNC,
  LOU_SEMA_CONST_STRING,
  LOU_SEMA_CONST_EXTERN,
} lou_sema_const_kind_t; 

typedef struct {
  lou_sema_const_kind_t kind;
  lou_sema_type_t *type;

  union {
    uint64_t integer;
    lou_hir_func_t *func;
    lou_hir_extern_t *external;
    lou_slice_t string;
  };
} lou_sema_const_t; 

lou_sema_const_t *lou_sema_const_new_int(lou_mempool_t *mempool, lou_sema_type_t *type, uint64_t value);
lou_sema_const_t *lou_sema_const_new_func(lou_mempool_t *mempool, lou_sema_type_t *type, lou_hir_func_t *func);
lou_sema_const_t *lou_sema_const_new_extern(lou_mempool_t *mempool, lou_sema_type_t *type, lou_hir_extern_t *external);
lou_sema_const_t *lou_sema_const_new_string(lou_mempool_t *mempool, lou_slice_t content);

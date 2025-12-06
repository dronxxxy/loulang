#include "type.h"
#include <stdint.h>

typedef enum {
  LOU_SEMA_CONST_INTEGER,
} lou_sema_const_kind_t; 

typedef struct {
  lou_sema_const_kind_t kind;
  lou_sema_type_t *type;

  union {
    uint64_t integer;
  };
} lou_sema_const_t; 

lou_sema_const_t *lou_sema_const_new_int(lou_mempool_t *mempool, lou_sema_type_t *type, uint64_t value);

#pragma once

#include "lou/core/mempool.h"
#include "lou/hir/const.h"
#include <stdbool.h>
#include <stdio.h>

typedef enum {
  LOU_SEMA_INT_8, 
  LOU_SEMA_INT_16, 
  LOU_SEMA_INT_32, 
  LOU_SEMA_INT_64, 
} lou_sema_int_size_t;

typedef enum {
  LOU_SEMA_TYPE_STRING, 
  LOU_SEMA_TYPE_INTEGER, 
  LOU_SEMA_TYPE_FUNCTION, 
  LOU_SEMA_TYPE_POINTER, 
  LOU_SEMA_TYPE_BOOL, 
  LOU_SEMA_TYPE_ARRAY, 
  LOU_SEMA_TYPE_STRUCT, 
} lou_sema_type_kind_t;

typedef struct lou_sema_type_t lou_sema_type_t;

typedef struct {
  lou_slice_t name;
  lou_sema_type_t *type;
} lou_sema_struct_field_t;

typedef struct lou_sema_type_t {
  lou_sema_type_kind_t kind;
  bool complete;

  union {
    struct {
      lou_sema_int_size_t size;
      bool is_signed;
    } integer;

    struct {
      lou_sema_struct_field_t *fields;
    } structure;

    struct {
      lou_sema_type_t *of;
      size_t length;
    } array;

    struct {
      lou_sema_type_t **args;
      lou_sema_type_t *returns;
    } func;

    lou_sema_type_t *pointer_to;
  };

  lou_hir_type_t *converted_type;
} lou_sema_type_t;

lou_sema_struct_field_t lou_sema_struct_field_new(lou_slice_t name, lou_sema_type_t *type); 

lou_sema_type_t *lou_sema_type_new_int(lou_mempool_t *mempool, lou_sema_int_size_t size, bool is_signed);
lou_sema_type_t *lou_sema_type_new_func(lou_mempool_t *mempool);
lou_sema_type_t *lou_sema_type_new_array(lou_mempool_t *mempool, lou_sema_type_t *of, size_t length);
lou_sema_type_t *lou_sema_type_new_pointer(lou_mempool_t *mempool);
lou_sema_type_t *lou_sema_type_new_string(lou_mempool_t *mempool);
lou_sema_type_t *lou_sema_type_new_struct(lou_mempool_t *mempool, lou_sema_struct_field_t *fields);
lou_sema_type_t *lou_sema_type_new_bool(lou_mempool_t *mempool);

void lou_sema_type_init_func(lou_sema_type_t *type, lou_sema_type_t **args, lou_sema_type_t *returns);
void lou_sema_type_init_pointer(lou_sema_type_t *type, lou_sema_type_t *to);

bool lou_sema_type_eq(const lou_sema_type_t *a, const lou_sema_type_t *b);

lou_hir_type_t *lou_sema_type_as_hir(lou_mempool_t *mempool, lou_sema_type_t *type);

void lou_sema_type_log(FILE *stream, lou_sema_type_t *type);

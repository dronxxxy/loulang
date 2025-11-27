#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include <stdint.h>
#include <stdio.h>

typedef enum {
  ELF_ARCH_X86_64,
} elf_arch_t;

typedef enum {
  ELF_BITS_64,
} elf_bits_t;

typedef struct elf_info_t {
  elf_bits_t bits;
  elf_arch_t arch;
  uint64_t entry;
} elf_info_t;

typedef enum {
  ELF_PROGRAM_DATA,
} elf_program_kind_t;

typedef enum {
  ELF_PROGRAM_ACCESS_READ = 1 << 0,
  ELF_PROGRAM_ACCESS_WRITE = 1 << 1,
  ELF_PROGRAM_ACCESS_EXEC = 1 << 2,
} elf_program_access_t;

typedef struct {
  elf_program_kind_t kind;
  elf_program_access_t access;

  union {
    struct {
      uint64_t address;
      size_t alignment;
      lou_slice_t buf;
    } data;
  };
} elf_program_t;

typedef struct {} elf_section_t;

typedef struct elf_builder_t {
  elf_info_t info;
  lou_mempool_t *mempool;
  elf_program_t *ptable;
  elf_section_t *stable;
} elf_builder_t;

elf_builder_t *elf_builder_new(elf_info_t info);
void elf_builder_add_program(elf_builder_t *builder, elf_program_t program);
void elf_builder_dump(const elf_builder_t *builder, FILE *stream);
void elf_builder_free(elf_builder_t *builder);

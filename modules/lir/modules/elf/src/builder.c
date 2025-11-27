#include "lir/elf/builder.h"
#include "dump64.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include <assert.h>
#include <stdio.h>

elf_builder_t *elf_builder_new(elf_info_t info) {
  lou_mempool_t *mempool = lou_mempool_new();
  elf_builder_t *builder = LOU_MEMPOOL_ALLOC(mempool, elf_builder_t);
  builder->stable = LOU_MEMPOOL_VEC_NEW(mempool, elf_section_t);
  builder->ptable = LOU_MEMPOOL_VEC_NEW(mempool, elf_program_t);
  builder->mempool = mempool;
  builder->info = info;
  return builder;
}

void elf_builder_dump(const elf_builder_t *builder, FILE *stream) {
  assert(builder->info.bits == ELF_BITS_64);
  elf_builder_dump64(builder, stream);
}

void elf_builder_free(elf_builder_t *builder) {
  lou_mempool_free(builder->mempool);
}

void elf_builder_add_program(elf_builder_t *builder, elf_program_t program) {
  *LOU_VEC_PUSH(&builder->ptable) = program;
}

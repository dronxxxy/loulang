#include "builder.h"
#include "elf/dump64.h"
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


// this is so bad to have test here but i just wanna now)
void lir_test() {
  unsigned char img[] = {
    0xb8, 0x01, 0x00, 0x00, 0x00, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x48, 0xbe,
    0x27, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0xba, 0x0f, 0x00, 0x00,
    0x00, 0x0f, 0x05, 0xb8, 0x3c, 0x00, 0x00, 0x00, 0xbf, 0x00, 0x00, 0x00,
    0x00, 0x0f, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f,
    0x72, 0x6c, 0x64, 0x21, 0x0a, 0x00
  };

  size_t vma = 0x400000;
  elf_builder_t *builder = elf_builder_new((elf_info_t) {
    .bits = ELF_BITS_64,
    .arch = ELF_ARCH_X86_64,
    .entry = vma,
  });
  elf_builder_add_program(builder, (elf_program_t) {
    .kind = ELF_PROGRAM_DATA,
    .access = ELF_PROGRAM_ACCESS_EXEC | ELF_PROGRAM_ACCESS_READ, 
    .data = {
      .address = vma,
      .alignment = 0x1000,
      .buf = { (const char*)img, sizeof(img) }
    },
  });
  FILE *file = fopen("build/out", "wb");
  elf_builder_dump(builder, file);
  fclose(file);
  elf_builder_free(builder);
}

#include "builder.h"
#include "codegen/x86/x86.h"
#include "elf/dump64.h"
#include "lou/core/mempool.h"
#include "lou/core/slice.h"
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
  size_t vma = 0x400000;
  x86_builder_t *x86_builder = x86_builder_new((x86_machine_t) {
    .code = X86_CODE64,
    .origin = vma,
  });

  x86_builder_movir(x86_builder, 1, X86_REG_RAX);
  x86_builder_movir(x86_builder, 1, X86_REG_RDI);
  x86_label_t *hello_world = x86_builder_label_create(x86_builder);
  const char hello_world_data[] = "Hello, world!\n";
  x86_builder_movlr(x86_builder, hello_world, X86_REG_RSI);
  x86_builder_movir(x86_builder, sizeof(hello_world_data), X86_REG_RDX);
  x86_builder_syscall(x86_builder);

  x86_builder_movir(x86_builder, 60, X86_REG_RAX);
  x86_builder_movir(x86_builder, 0, X86_REG_RDI);
  x86_builder_syscall(x86_builder);

  x86_builder_label_push(x86_builder, hello_world);
  x86_builder_raw(x86_builder, lou_slice_from_cstr(hello_world_data));

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
      .buf = x86_builder_buffer(x86_builder),
    },
  });
  FILE *file = fopen("build/out", "wb");
  elf_builder_dump(builder, file);
  fclose(file);
  elf_builder_free(builder);
  x86_builder_free(x86_builder);
}

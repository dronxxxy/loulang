#include "hello_world.h"
#include "lir/elf/builder.h"
#include "x86.h"

void test_hello_world() {
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
  x86_builder_movir(x86_builder, sizeof(hello_world_data) - 1, X86_REG_RDX);
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
  elf_builder_dump(builder, stdout);
  elf_builder_free(builder);
  x86_builder_free(x86_builder);
}


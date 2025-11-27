#include "dump64.h"
#include "lou/core/vec.h"
#include <assert.h>
#include <elf.h>
#include <stdio.h>

static inline Elf64_Word elf_program_access_to_flags(elf_program_access_t access) {
  Elf64_Word result = 0;
  if (access & ELF_PROGRAM_ACCESS_EXEC) {
    result |= PF_X;
  }
  if (access & ELF_PROGRAM_ACCESS_WRITE) {
    result |= PF_W;
  }
  if (access & ELF_PROGRAM_ACCESS_READ) {
    result |= PF_R;
  }
  return result;
}

static inline size_t get_aligned_addr(size_t *addr, size_t *zeroes, size_t alignment) {
  if (*addr % alignment != 0) {
    if (zeroes) {
      *zeroes = alignment - *addr % alignment;
    }
    *addr += alignment - *addr % alignment;
  }
  size_t old = *addr;
  *addr += alignment;
  return old;
}

static inline Elf64_Phdr elf_program_to_header(elf_program_t *program, size_t *addr) {
  switch (program->kind) {
    case ELF_PROGRAM_DATA:
      return (Elf64_Phdr) {
        .p_type = PT_LOAD,
        .p_flags = elf_program_access_to_flags(program->access),

        .p_offset = get_aligned_addr(addr, NULL, program->data.alignment),
        .p_filesz = program->data.buf.length,
        .p_memsz = program->data.buf.length,

        .p_vaddr = program->data.address,
        .p_paddr = program->data.address,
        .p_align = program->data.alignment,
      };
  }
  // TODO: move to core
  __builtin_unreachable();
}

#define ZEROES_COUNT 128

static inline void file_write_zeroes(FILE *stream, size_t count) {
  char zeroes[ZEROES_COUNT] = { 0 };
  for (size_t i = 0; i < count / ZEROES_COUNT; i++) {
    fwrite(zeroes, 1, ZEROES_COUNT, stream);
  }
  fwrite(zeroes, 1, count % ZEROES_COUNT, stream);
}

void elf_builder_dump64(const elf_builder_t *builder, FILE *stream) {
  size_t ptable_offset = sizeof(Elf64_Ehdr);
  size_t ptable_size = lou_vec_len(builder->ptable) * sizeof(Elf64_Phdr);

  size_t stable_offset = ptable_offset + ptable_size;
  size_t stable_size = lou_vec_len(builder->stable) * sizeof(Elf64_Shdr);

  Elf64_Ehdr eheader = {
    .e_ehsize = sizeof(Elf64_Ehdr),

    .e_ident = { '\x7f', 'E', 'L', 'F', ELFCLASS64, ELFDATA2LSB, EV_CURRENT, 0, 0, 0 },
    .e_type = ET_EXEC,
    .e_machine = EM_X86_64,
    .e_version = EV_CURRENT,

    .e_entry = builder->info.entry,

    .e_phoff = ptable_size ? ptable_offset : 0,
    .e_phentsize = sizeof(Elf64_Phdr),
    .e_phnum = lou_vec_len(builder->ptable),

    .e_shoff = stable_size ? stable_offset : 0,
    .e_shentsize = sizeof(Elf64_Shdr),
    .e_shnum = lou_vec_len(builder->stable),

    .e_flags = 0,
    .e_shstrndx = SHN_UNDEF,
  };
  fwrite(&eheader, sizeof(eheader), 1, stream);

  size_t position = stable_offset + stable_size;

  size_t table_pos = position;
  for (size_t i = 0; i < lou_vec_len(builder->ptable); i++) {
    Elf64_Phdr pheader = elf_program_to_header(&builder->ptable[i], &table_pos);
    fwrite(&pheader, sizeof(pheader), 1, stream);
  }
  assert(!lou_vec_len(builder->stable));
  
  size_t data_pos = position;
  for (size_t i = 0; i < lou_vec_len(builder->ptable); i++) {
    elf_program_t *program = &builder->ptable[i];
    size_t zeroes_count = 0;
    data_pos = get_aligned_addr(&data_pos, &zeroes_count, program->data.alignment);
    file_write_zeroes(stream, zeroes_count);
    fwrite(program->data.buf.ptr, 1, program->data.buf.length, stream);
  }
}

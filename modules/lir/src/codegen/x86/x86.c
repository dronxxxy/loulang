#include "x86.h"
#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/core/vec.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct x86_builder_t {
  lou_mempool_t *mempool;
  x86_machine_t machine;

  char *buffer;
} x86_builder_t;

typedef struct {
  size_t address;
  size_t size;
  uint64_t offset;
} x86_replacement_t;

typedef struct x86_label_t {
  uint64_t address;
  x86_replacement_t *replacements;
} x86_label_t;

x86_builder_t *x86_builder_new(x86_machine_t machine) {
  lou_mempool_t *mempool = lou_mempool_new();
  x86_builder_t *builder = LOU_MEMPOOL_ALLOC(mempool, x86_builder_t);
  builder->mempool = mempool;
  builder->machine = machine;
  builder->buffer = LOU_MEMPOOL_VEC_NEW(mempool, char);
  return builder;
}

void x86_builder_free(x86_builder_t *builder) {
  lou_mempool_free(builder->mempool);
}

void x86_builder_raw(x86_builder_t *builder, lou_slice_t slice) {
  LOU_VEC_RESERVE(&builder->buffer, lou_vec_len(builder->buffer) + slice.length);
  for (size_t i = 0; i < slice.length; i++) {
    *LOU_VEC_PUSH(&builder->buffer) = slice.ptr[i];
  }
}

x86_label_t *x86_builder_label_create(x86_builder_t *builder) {
  x86_label_t *label = LOU_MEMPOOL_ALLOC(builder->mempool, x86_label_t);
  label->replacements = LOU_MEMPOOL_VEC_NEW(builder->mempool, x86_replacement_t);
  return label;
}

static inline uint64_t x86_builder_convert_addr(const x86_builder_t *builder, uint64_t addr) {
  return addr + builder->machine.origin;
}

static inline uint64_t x86_builder_ip(const x86_builder_t *builder) {
  return x86_builder_convert_addr(builder, lou_vec_len(builder->buffer));
}

static inline void x86_builder_apply_replacement(x86_builder_t *builder, x86_replacement_t replacement) {
  uint64_t value = x86_builder_ip(builder) + replacement.offset;
  assert(replacement.size <= 8);
  // TODO: big endian support
  memcpy(&builder->buffer[replacement.address], &value, replacement.size);
}

void x86_builder_label_push(x86_builder_t *builder, x86_label_t *label) {
  for (size_t i = 0; i < lou_vec_len(label->replacements); i++) {
    x86_builder_apply_replacement(builder, label->replacements[i]);
  }
  lou_vec_free(label->replacements);
  label->replacements = NULL;
  label->address = lou_vec_len(builder->buffer);
}

lou_slice_t x86_builder_buffer(const x86_builder_t *builder) {
  return lou_slice_new(builder->buffer, lou_vec_len(builder->buffer));
}

static inline void x86_builder_write_imm(x86_builder_t *builder, size_t size, uint64_t value) {
  assert(size <= 8);
  // TODO: big endian support
  x86_builder_raw(builder, lou_slice_new((char*)&value, size));
}

static inline void x86_builder_write_label(x86_builder_t *builder, x86_label_t *label, size_t size, size_t offset) { 
  x86_replacement_t replacement = {
    .size = size,
    .offset = offset,
    .address = lou_vec_len(builder->buffer),
  };
  assert(size <= 8);
  x86_builder_write_imm(builder, size, 0);
  if (!label->replacements) {
    x86_builder_apply_replacement(builder, replacement);
  } else {
    *LOU_VEC_PUSH(&label->replacements) = replacement;
  }
}

static inline void x86_builder_write_byte(x86_builder_t *builder, char byte) {
  *LOU_VEC_PUSH(&builder->buffer) = byte;
}

static inline size_t x86_register_size(x86_register_t reg) {
  switch (reg >> 3) {
    case 0b00:
      return 2;
    case 0b01:
      return 4;
    case 0b10:
    case 0b11:
      return 8;
  }
  __builtin_unreachable();
}

static inline void x86_builder_write_prefix(x86_builder_t *builder, x86_register_t reg) {
  switch (reg >> 3) {
    case 0b00:
      if (builder->machine.code == X86_CODE32) {
        x86_builder_write_byte(builder, 0x66);
      }
      return;
    case 0b01:
      if (builder->machine.code == X86_CODE16) {
        x86_builder_write_byte(builder, 0x66);
      }
      return;
    case 0b10:
      assert(builder->machine.code == X86_CODE64);
      x86_builder_write_byte(builder, 0b01001000);
      return;
    case 0b11:
      assert(builder->machine.code == X86_CODE64);
      x86_builder_write_byte(builder, 0b01001001);
      return;
  }
  __builtin_unreachable();
}

void x86_builder_movir(x86_builder_t *builder, uint64_t value, x86_register_t reg) {
  x86_builder_write_prefix(builder, reg);
  x86_builder_write_byte(builder, 0xB8 + (reg & 0b111));
  x86_builder_write_imm(builder, x86_register_size(reg), value);
}

void x86_builder_movlr(x86_builder_t *builder, x86_label_t *label, x86_register_t reg) {
  x86_builder_write_prefix(builder, reg);
  x86_builder_write_byte(builder, 0xB8 + (reg & 0b111));
  x86_builder_write_label(builder, label, x86_register_size(reg), 0);
}

void x86_builder_int(x86_builder_t *builder, uint8_t num) {
  x86_builder_write_byte(builder, 0xCD);
  x86_builder_write_byte(builder, num);
}

void x86_builder_syscall(x86_builder_t *builder) {
  x86_builder_write_byte(builder, 0x0f);
  x86_builder_write_byte(builder, 0x05);
}


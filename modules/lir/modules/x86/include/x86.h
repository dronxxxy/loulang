#pragma once

#include "lou/core/slice.h"
#include <stdint.h>

typedef enum {
  X86_REG_AX =  0b00000,
  X86_REG_CX =  0b00001,
  X86_REG_DX =  0b00010,
  X86_REG_BX =  0b00011,
  X86_REG_SP =  0b00100,
  X86_REG_BP =  0b00101,
  X86_REG_SI =  0b00110,
  X86_REG_DI =  0b00111,
  X86_REG_EAX = 0b01000,
  X86_REG_ECX = 0b01001,
  X86_REG_EDX = 0b01010,
  X86_REG_EBX = 0b01011,
  X86_REG_ESP = 0b01100,
  X86_REG_EBP = 0b01101,
  X86_REG_ESI = 0b01110,
  X86_REG_EDI = 0b01111,
  X86_REG_RAX = 0b10000,
  X86_REG_RCX = 0b10001,
  X86_REG_RDX = 0b10010,
  X86_REG_RBX = 0b10011,
  X86_REG_RSP = 0b10100,
  X86_REG_RBP = 0b10101,
  X86_REG_RSI = 0b10110,
  X86_REG_RDI = 0b10111,
  X86_REG_R8 =  0b11000,
  X86_REG_R9 =  0b11001,
  X86_REG_R10 = 0b11010,
  X86_REG_R11 = 0b11011,
  X86_REG_R12 = 0b11100,
  X86_REG_R13 = 0b11101,
  X86_REG_R14 = 0b11110,
  X86_REG_R15 = 0b11111,
} x86_register_t;

typedef enum {
  X86_IMM_INTEGER,
  X86_IMM_LABEL,
} x86_imm_kind_t;

typedef enum {
  X86_CODE16,
  X86_CODE32,
  X86_CODE64,
} x86_code_t;

typedef struct {
  x86_code_t code;
  uint64_t origin;
} x86_machine_t;

typedef struct x86_builder_t x86_builder_t;
typedef struct x86_label_t x86_label_t;

x86_builder_t *x86_builder_new(x86_machine_t machine);
void x86_builder_free(x86_builder_t *builder);
lou_slice_t x86_builder_buffer(const x86_builder_t *builder);

void x86_builder_movir(x86_builder_t *builder, uint64_t value, x86_register_t reg);
void x86_builder_movlr(x86_builder_t *builder, x86_label_t *label, x86_register_t reg);
void x86_builder_int(x86_builder_t *builder, uint8_t num);
void x86_builder_syscall(x86_builder_t *builder);

void x86_builder_raw(x86_builder_t *builder, lou_slice_t slice);

x86_label_t *x86_builder_label_create(x86_builder_t *builder);
void x86_builder_label_push(x86_builder_t *builder, x86_label_t *label);

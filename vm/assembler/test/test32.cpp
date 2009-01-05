#include <iostream>

#include <stdint.h>
#include <stdio.h>
#include <cassert>

#include "assembler_x86.hpp"

using namespace std;
using namespace assembler;
using namespace assembler_x86;

#define assert_kind(what) assert(ud->mnemonic == what)
#define assert_op(which, field, val) assert(ud->operand[which].field == val)
#define assert_len(len) ud_insn_len(ud) == len;

void test_mov1() {
  AssemblerX86 a;
  a.mov(eax, 100);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, size, 32);
  assert_op(1, lval.uword, 100);
  delete ud;
  cout << "test_mov: imm to reg ok!\n";
}

void test_mov2() {
  AssemblerX86 a;
  a.mov(a.address(eax, 4), 100);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_EAX);
  assert_op(0, index, UD_NONE);
  assert_op(0, lval.uword, 4);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, size, 32);
  assert_op(1, lval.uword, 100);
  delete ud;
  cout << "test_mov: imm to address ok!\n";
}

void test_mov3() {
  AssemblerX86 a;
  a.mov(eax, esi);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, index, UD_NONE);

  delete ud;
  cout << "test_mov: reg to reg ok!\n";
}

void test_mov4() {
  AssemblerX86 a;
  a.mov(eax, a.address(esi, 4));
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, lval.uword, 4);

  delete ud;
  cout << "test_mov: address to reg ok!\n";
}

void test_mov5() {
  AssemblerX86 a;
  a.mov(a.address(esi, 4), eax);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.uword, 4);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_EAX);

  delete ud;
  cout << "test_mov: reg to address ok!\n";
}

void test_mov6() {
  AssemblerX86 a;
  a.mov(a.address(esp, 4), ebx);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESP);
  assert_op(0, lval.uword, 4);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_EBX);

  delete ud;
  cout << "test_mov: reg to address (esp base) ok!\n";
}

void test_mov_delayed1() {
  AssemblerX86 a;
  uintptr_t* loc;
  a.mov_delayed(ebx, &loc);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, 0);

  delete ud;
  cout << "test_mov: delayed imm to reg (base case) ok!\n";

}

void test_mov_delayed2() {
  AssemblerX86 a;
  uintptr_t* loc;
  a.mov_delayed(ebx, &loc);
  *loc = 0x47;
  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, 0x47);

  delete ud;
  cout << "test_mov: delayed imm to reg ok!\n";
}

void test_mov_delayed() {
  test_mov_delayed1();
  test_mov_delayed2();
}

void test_mov_scaled() {
  AssemblerX86 a;
  a.mov_scaled(eax, esi, ecx, 4);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, base, UD_R_ESI);
  assert_op(1, index, UD_R_ECX);
  assert_op(1, scale, 4);

  delete ud;
  cout << "test_mov_scaled: ok!\n";
}

void test_mov_to_table() {
  AssemblerX86 a;
  a.mov_to_table(eax, esi, 4, 80, ecx);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_EAX);
  assert_op(0, index, UD_R_ESI);
  assert_op(0, scale, 4);
  assert_op(0, lval.udword, 80);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_ECX);

  delete ud;
  cout << "test_mov_to_table: ok!\n";
}

void test_mov_from_table() {
  AssemblerX86 a;
  a.mov_from_table(ecx, eax, esi, 4, 80);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_EAX);
  assert_op(1, index, UD_R_ESI);
  assert_op(1, scale, 4);
  assert_op(1, lval.udword, 80);

  delete ud;
  cout << "test_mov_from_table: ok!\n";
}

void test_push1() {
  AssemblerX86 a;
  a.push(eax);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ipush);

  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  delete ud;
  cout << "test_push: reg ok!\n";
}

void test_push2() {
  AssemblerX86 a;
  a.push(100);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ipush);

  assert_op(0, type, UD_OP_IMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.uword, 100);

  delete ud;
  cout << "test_push: imm ok!\n";
}

void test_push3() {
  AssemblerX86 a;
  a.push(a.address(esi, 100));
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ipush);

  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.uword, 100);

  delete ud;
  cout << "test_push: address ok!\n";
}

void test_pop() {
  AssemblerX86 a;
  a.pop(eax);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ipop);

  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  delete ud;
  cout << "test_pop: reg ok!\n";
}

void test_lea() {
  AssemblerX86 a;
  a.lea(eax, esi, 16);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ilea);

  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, lval.uword, 16);

  delete ud;
  cout << "test_lea: address to reg ok!\n";
}

void test_leave() {
  AssemblerX86 a;
  a.leave();
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ileave);
  assert_len(1);

  delete ud;
  cout << "test_leave: ok!\n";
}

void test_ret() {
  AssemblerX86 a;
  a.ret();
  ud_t *ud = a.disassemble();
  assert_kind(UD_Iret);
  assert_len(1);

  delete ud;
  cout << "test_ret: ok!\n";
}

void test_sub() {
  AssemblerX86 a;
  a.sub(eax, 16);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Isub);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.uword, 16);

  delete ud;
  cout << "test_sub: imm from reg ok!\n";
}

void test_add1() {
  AssemblerX86 a;
  a.add(eax, 16);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.uword, 16);

  delete ud;
  cout << "test_add: imm to reg ok!\n";
}

void test_add2() {
  AssemblerX86 a;
  a.add(a.address(eax, 8), 16);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_EAX);
  assert_op(0, lval.uword, 8);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.uword, 16);

  delete ud;
  cout << "test_add: imm to address ok!\n";
}

void test_add3() {
  AssemblerX86 a;
  a.add(eax, esi);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_ESI);

  delete ud;
  cout << "test_add: reg to reg ok!\n";
}

void test_dec() {
  AssemblerX86 a;
  a.dec(eax);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Idec);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  delete ud;
  cout << "test_dec: reg ok!\n";
}

void test_inc() {
  AssemblerX86 a;
  a.inc(eax);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Iinc);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  delete ud;
  cout << "test_inc: reg ok!\n";
}

void test_cmp1() {
  AssemblerX86 a;
  a.cmp(eax, 10);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Icmp);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.uword, 10);

  delete ud;
  cout << "test_cmp: imm to reg ok!\n";
}

void test_cmp2() {
  AssemblerX86 a;
  a.cmp(eax, esi);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Icmp);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_ESI);

  delete ud;
  cout << "test_cmp: reg to reg ok!\n";
}

void test_cmp3() {
  AssemblerX86 a;
  a.cmp(a.address(eax, 8), 17);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Icmp);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_EAX);
  assert_op(0, lval.uword, 8);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.uword, 17);

  delete ud;
  cout << "test_cmp: imm to address ok!\n";
}

void test_shift_right() {
  AssemblerX86 a;
  a.shift_right(eax, 2);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Isar);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.uword, 2);

  delete ud;
  cout << "test_shift_right: reg by imm ok!\n";
}

void test_shift_left() {
  AssemblerX86 a;
  a.shift_left(eax, 2);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ishl);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.uword, 2);

  delete ud;
  cout << "test_shift_left: reg by imm ok!\n";
}

void test_bit_or() {
  AssemblerX86 a;
  a.bit_or(eax, 2);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ior);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.uword, 2);

  delete ud;
  cout << "test_bit_or: reg and imm ok!\n";
}

void test_bit_or2() {
  AssemblerX86 a;
  a.bit_or(eax, a.address(ecx, 0));
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ior);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ECX);
  assert_op(1, lval.udword, 0);

  delete ud;
  cout << "test_bit_or: reg and address ok!\n";
}

void test_bit_and1() {
  AssemblerX86 a;
  a.bit_and(eax, 2);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Iand);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.uword, 2);

  delete ud;
  cout << "test_bit_and: reg and imm ok!\n";
}

void test_bit_and2() {
  AssemblerX86 a;
  a.bit_and(eax, a.address(ecx, 0));
  ud_t *ud = a.disassemble();
  assert_kind(UD_Iand);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ECX);
  assert_op(1, lval.udword, 0);

  delete ud;
  cout << "test_bit_and: reg and address ok!\n";
}

void test_bit_and3() {
  AssemblerX86 a;
  a.bit_and(eax, ecx);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Iand);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_ECX);

  delete ud;
  cout << "test_bit_and: reg and reg ok!\n";
}

void test_test1() {
  AssemblerX86 a;
  a.test(eax, esi);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Itest);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_ESI);

  delete ud;
  cout << "test_test: reg and reg ok!\n";
}

void test_test2() {
  AssemblerX86 a;
  a.test(eax, 18);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Itest);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.uword, 18);

  delete ud;
  cout << "test_test: reg and imm ok!\n";
}

void test_call1() {
  AssemblerX86 a;
  a.call(eax);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Icall);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  delete ud;
  cout << "test_call: reg ok!\n";
}

void test_call2() {
  AssemblerX86 a;
  a.call((void*)puts);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Icall);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.udword, (uintptr_t)puts - (uintptr_t)a.pc());

  void* addr = (void*)((uint8_t*)a.pc() - 4);
  Relocation* rel = a.find_relocation(addr);
  assert(rel);
  assert(rel->kind() == Relocation::Relative);
  assert(rel->address() == (void*)puts);

  delete ud;
  cout << "test_call: imm ok!\n";
}

void test_call3() {
  AssemblerX86 a;
  a.call((void*)0x47, "puts");
  ud_t *ud = a.disassemble();
  assert_kind(UD_Icall);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.udword, 0x47 - (uintptr_t)a.pc());

  void* addr = (void*)((uint8_t*)a.pc() - 4);
  Relocation* rel = a.find_relocation(addr);
  assert(rel);
  assert(rel->kind() == Relocation::Relative);
  assert(rel->target_kind() == Relocation::Symbol);
  assert(rel->symbol() == std::string("puts"));
  assert(rel->address() == (void*)0x47);

  rel->resolve_and_write();
  assert(rel->address() == (void*)puts);
  ud = a.disassemble();
  assert_kind(UD_Icall);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.udword, (uintptr_t)puts - (uintptr_t)a.pc());

  delete ud;
  cout << "test_call: symbol ok!\n";
}

void test_jump1_multiple() {
  AssemblerX86 a;
  AssemblerX86::NearJumpLocation label;
  a.set_label(label);
  a.jump(label);
  a.nop();
  a.jump(label);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ijmp);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  uintptr_t loc = (uintptr_t)a.buffer() + ud->pc;
  assert_op(0, lval.udword, (uintptr_t)label.destination() - loc);

  ud_disassemble(ud);
  assert_kind(UD_Inop);

  ud_disassemble(ud);
  assert_kind(UD_Ijmp);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  loc = (uintptr_t)a.buffer() + ud->pc;
  assert_op(0, lval.udword, (uintptr_t)label.destination() - loc);

  delete ud;
  cout << "test_jump: multiple sources ok!\n";
}

void test_jump1_multiple_with_fixup() {
  AssemblerX86 a;
  AssemblerX86::NearJumpLocation label;
  a.jump(label);
  a.nop();
  a.jump(label);
  a.nop();
  a.set_label(label);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Ijmp);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  uintptr_t loc = (uintptr_t)a.buffer() + ud->pc;
  assert_op(0, lval.udword, (uintptr_t)label.destination() - loc);

  ud_disassemble(ud);
  assert_kind(UD_Inop);

  ud_disassemble(ud);
  assert_kind(UD_Ijmp);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  loc = (uintptr_t)a.buffer() + ud->pc;
  assert_op(0, lval.udword, (uintptr_t)label.destination() - loc);

  delete ud;
  cout << "test_jump: multiple sources ok!\n";
}

void test_jump1() {
  AssemblerX86 a;
  AssemblerX86::NearJumpLocation label;
  a.set_label(label);
  a.jump(label);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ijmp);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.udword, (uintptr_t)label.destination() - (uintptr_t)a.pc());

  delete ud;
  cout << "test_jump: label ok!\n";

  test_jump1_multiple();
  test_jump1_multiple_with_fixup();
}

void test_jump2() {
  AssemblerX86 a;
  a.jump(eax);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ijmp);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  delete ud;
  cout << "test_jump: reg ok!\n";
}

void test_jump3() {
  AssemblerX86 a;
  a.jump((void*)puts);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ijmp);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.udword, (uintptr_t)puts - (uintptr_t)a.pc());

  void* addr = (void*)((uint8_t*)a.pc() - 4);
  Relocation* rel = a.find_relocation(addr);
  assert(rel);
  assert(rel->kind() == Relocation::Relative);
  assert(rel->address() == (void*)puts);

  delete ud;
  cout << "test_jump: absolute ok!\n";
}

void test_jump_if_equal() {
  AssemblerX86 a;
  AssemblerX86::NearJumpLocation label;
  a.set_label(label);
  a.jump_if_equal(label);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ijz);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  int8_t offset = (intptr_t)label.destination() - (intptr_t)a.pc();
  assert_op(0, lval.sbyte, offset);

  delete ud;
  cout << "test_jump_if_equal: imm ok!\n";
}

void test_jump_if_not_equal() {
  AssemblerX86 a;
  AssemblerX86::NearJumpLocation label;
  a.set_label(label);
  a.jump_if_not_equal(label);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ijnz);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.sbyte, (uintptr_t)label.destination() - (uintptr_t)a.pc());

  delete ud;
  cout << "test_jump_if_not_equal: imm ok!\n";
}

void test_jump_if_overflow() {
  AssemblerX86 a;
  AssemblerX86::NearJumpLocation label;
  a.set_label(label);
  a.jump_if_overflow(label);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Ijo);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.sbyte, (uintptr_t)label.destination() - (uintptr_t)a.pc());

  delete ud;
  cout << "test_jump_if_overflow: imm ok!\n";
}

void test_read_eip() {
  AssemblerX86 a;
  AssemblerX86::InstructionDisplacement loc;
  a.read_eip(eax, loc);
  a.set_label(loc);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Icall);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.udword, 0);

  ud_disassemble(ud);
  assert_kind(UD_Ipop);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  ud_disassemble(ud);
  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, loc.difference());

  delete ud;
  cout << "test_read_eip: ok!\n";
}

void test_prologue() {
  AssemblerX86 a;
  a.prologue(6);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBP);

  ud_disassemble(ud);
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBP);
  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_ESP);

  ud_disassemble(ud);
  assert_kind(UD_Isub);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESP);
  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 12);

  ud_disassemble(ud);
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EDI);

  ud_disassemble(ud);
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  ud_disassemble(ud);
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBX);

  delete ud;
  cout << "test_prologue: ok!\n";
}

void test_prologue2() {
  AssemblerX86 a;
  a.prologue(20);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBP);

  ud_disassemble(ud);
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBP);
  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_ESP);

  ud_disassemble(ud);
  assert_kind(UD_Isub);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESP);
  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 36);

  ud_disassemble(ud);
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EDI);

  ud_disassemble(ud);
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  ud_disassemble(ud);
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBX);

  delete ud;
  cout << "test_prologue: large local stack ok!\n";
}

void test_epilogue() {
  AssemblerX86 a;
  a.epilogue();
  ud_t *ud = a.disassemble();

  assert_kind(UD_Ipop);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBX);

  ud_disassemble(ud);
  assert_kind(UD_Ipop);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  ud_disassemble(ud);
  assert_kind(UD_Ipop);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EDI);

  ud_disassemble(ud);
  assert_kind(UD_Ileave);

  ud_disassemble(ud);
  assert_kind(UD_Iret);

  delete ud;
  cout << "test_epilogue: ok!\n";
}

void test_start_call() {
  AssemblerX86 a;
  a.start_call(2);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Isub);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESP);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 8);

  delete ud;
  cout << "test_start_call: ok!\n";
}

void test_start_call2() {
  AssemblerX86 a;
  a.start_call(17);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Isub);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESP);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 12);

  delete ud;
  cout << "test_start_call: large arg count ok!\n";
}

void test_end_call() {
  AssemblerX86 a;
  a.end_call(2);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESP);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 16);

  delete ud;
  cout << "test_end_call: ok!\n";
}

void test_end_call2() {
  AssemblerX86 a;
  a.end_call(17);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESP);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 80);

  delete ud;
  cout << "test_end_call: large arg count ok!\n";
}

void test_load_arg() {
  AssemblerX86 a;
  a.load_arg(ebx, 7);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_EBP);
  assert_op(1, lval.udword, 36);

  delete ud;
  cout << "test_load_arg: ok!\n";
}

void test_push_arg() {
  AssemblerX86 a;
  a.push_arg(7);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_EBP);
  assert_op(0, lval.udword, 36);

  delete ud;
  cout << "test_push_arg: ok!\n";
}

void test_jump_via_table() {
  AssemblerX86 a;
  a.jump_via_table(reinterpret_cast<void**>(400), eax);

  ud_t* ud = a.disassemble();
  assert_kind(UD_Ijmp);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_NONE);
  assert_op(0, index, UD_R_EAX);
  assert_op(0, scale, 4);
  assert_op(0, lval.udword, 400);
  cout << "test_jump_via_table: ok!\n";
}

int main(int argc, char** argv) {
  test_mov1();
  test_mov2();
  test_mov3();
  test_mov4();
  test_mov5();
  test_mov6();
  test_mov_delayed();

  test_push1();
  test_push2();
  test_push3();
  test_pop();
  test_lea();
  test_leave();
  test_ret();
  test_sub();
  test_add1();
  test_add2();
  test_add3();
  test_dec();
  test_inc();
  test_cmp1();
  test_cmp2();
  test_cmp3();
  test_shift_right();
  test_shift_left();
  test_bit_or();
  test_bit_or2();
  test_bit_and1();
  test_bit_and2();
  test_bit_and3();
  test_test1();
  test_test2();
  test_call1();
  test_call2();
  test_call3();
  test_jump1();
  test_jump2();
  test_jump3();
  test_jump_if_equal();
  test_jump_if_not_equal();
  test_jump_if_overflow();
  test_read_eip();
  test_prologue();
  test_prologue2();
  test_epilogue();
  test_start_call();
  test_start_call2();
  test_end_call();
  test_end_call2();
  test_load_arg();
  test_push_arg();
  test_jump_via_table();
  test_mov_scaled();
  test_mov_to_table();
  test_mov_from_table();
}

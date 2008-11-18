#include <iostream>

#include <stdint.h>
#include <stdio.h>

#include "assembler_x8664.hpp"

using namespace std;
using namespace assembler_x8664;

#define assert_kind(what) assert(ud->mnemonic == what)
#define assert_op(which, field, val) assert(ud->operand[which].field == val)
#define assert_len(len) ud_insn_len(ud) == len;

void test_mov1() {
  AssemblerX8664 a;
  a.mov(r12, 0x47);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_R12);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, size, 64);
  assert_op(1, lval.uword, 0x47);
  delete ud;
  cout << "test_mov: imm to extended reg ok!\n";
}

void test_mov2() {
  AssemblerX8664 a;
  a.mov(rax, 0x47);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_RAX);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, size, 64);
  assert_op(1, lval.uword, 0x47);
  delete ud;
  cout << "test_mov: imm to reg ok!\n";
}

void test_mov3() {
  AssemblerX8664 a;
  a.mov(a.address(r8, 8), 0x47);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_R8);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, size, 32);
  assert_op(1, lval.uword, 0x47);
  delete ud;
  cout << "test_mov: mem to extended reg ok!\n";
}

void test_mov4() {
  AssemblerX8664 a;
  a.mov(a.address(r12, 8), 0x47);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_R12);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, size, 32);
  assert_op(1, lval.uword, 0x47);
  delete ud;
  cout << "test_mov: mem to reg 12 ok!\n";
}

void test_mov5() {
  AssemblerX8664 a;
  a.mov(a.address(rax, 8), 0x47);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_RAX);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, size, 32);
  assert_op(1, lval.uword, 0x47);
  delete ud;
  cout << "test_mov: mem to extended reg ok!\n";
}

void test_mov6() {
  AssemblerX8664 a;
  a.mov(r8, r9);

  ud_t *ud = a.disassemble();
  // cout << " " << ud_insn_asm(ud) << "\n";

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_R8);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_R9);
  assert_op(1, index, UD_NONE);
  delete ud;
  cout << "test_mov: extended reg to extended reg ok!\n";
}

void test_mov7() {
  AssemblerX8664 a;
  a.mov(rax, r9);

  ud_t *ud = a.disassemble();
  // cout << " " << ud_insn_asm(ud) << "\n";

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_RAX);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_R9);
  assert_op(1, index, UD_NONE);
  delete ud;
  cout << "test_mov: extended reg to reg ok!\n";
}

void test_mov8() {
  AssemblerX8664 a;
  a.mov(r8, rax);

  ud_t *ud = a.disassemble();
  // cout << " " << ud_insn_asm(ud) << "\n";

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_R8);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_RAX);
  assert_op(1, index, UD_NONE);
  delete ud;
  cout << "test_mov: reg to extended reg ok!\n";
}

void test_mov9() {
  AssemblerX8664 a;
  a.mov(rsi, rax);

  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_EAX);
  assert_op(1, index, UD_NONE);
  delete ud;
  cout << "test_mov: reg to reg ok!\n";
}

int main(int argc, char** argv) {
  test_mov1();
  test_mov2();
  test_mov3();
  test_mov4();
  test_mov5();
  test_mov6();
  test_mov7();
  test_mov8();
  test_mov9();
}

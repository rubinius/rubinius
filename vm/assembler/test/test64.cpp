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

void test_mov10() {
  AssemblerX8664 a;
  a.mov(r9, a.address(rax, 8));
  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_R9);
  assert_op(0, index, UD_NONE);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_RAX);
  assert_op(1, lval.udword, 8);

  delete ud;
  cout << "test_mov: address to reg ok!\n";
}

void test_mov11() {
  AssemblerX8664 a;
  a.mov(a.address(rax, 8), r9);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_RAX);
  assert_op(0, lval.udword, 8);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_R9);
  assert_op(1, index, UD_NONE);

  delete ud;
  cout << "test_mov: reg to address ok!\n";
}

void test_push1() {
  AssemblerX8664 a;
  a.push(r8);

  ud_t *ud = a.disassemble();

  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_R8);
  assert_op(0, index, UD_NONE);

  delete ud;
  cout << "test_push: extended reg ok!\n";
}

void test_push2() {
  AssemblerX8664 a;
  a.push(rax);

  ud_t *ud = a.disassemble();

  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_RAX);
  assert_op(0, index, UD_NONE);

  delete ud;
  cout << "test_push: reg ok!\n";
}

void test_push3() {
  AssemblerX8664 a;
  a.push(0x47);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_IMM);
  assert_op(0, size, 32);
  assert_op(0, lval.udword, 0x47);

  delete ud;
  cout << "test_push: imm ok!\n";
}

void test_push4() {
  AssemblerX8664 a;
  a.push(a.address(r8, 8));
  ud_t *ud = a.disassemble();

  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_R8);
  assert_op(0, lval.udword, 8);

  delete ud;
  cout << "test_push: address ok!\n";
}

void test_pop1() {
  AssemblerX8664 a;
  a.pop(r9);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Ipop);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_R9);

  delete ud;
  cout << "test_pop: extended reg ok\n";
}

void test_pop2() {
  AssemblerX8664 a;
  a.pop(rax);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Ipop);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_RAX);

  delete ud;
  cout << "test_pop: reg ok\n";
}

void test_lea1() {
  AssemblerX8664 a;
  a.lea(rax, r8, 8);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Ilea);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_RAX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_R8);
  assert_op(1, lval.udword, 8);

  delete ud;
  cout << "test_lea: extended regs ok!\n";
}

void test_lea2() {
  AssemblerX8664 a;
  a.lea(rax, rsi, 8);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Ilea);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_RSI);
  assert_op(1, lval.udword, 8);

  delete ud;
  cout << "test_lea: regs ok!\n";
}

void test_sub1() {
  AssemblerX8664 a;
  a.sub(r8, 4);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Isub);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_R8);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, 4);

  delete ud;
  cout << "test_sub: imm from extended reg ok!\n";
}

void test_sub2() {
  AssemblerX8664 a;
  a.sub(rax, 4);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Isub);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, 4);

  delete ud;
  cout << "test_sub: imm from reg ok!\n";
}

void test_add1() {
  AssemblerX8664 a;
  a.add(r8, 4);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_R8);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, 4);

  delete ud;
  cout << "test_add: imm add extended reg ok!\n";
}

void test_add2() {
  AssemblerX8664 a;
  a.add(rcx, 4);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, 4);

  delete ud;
  cout << "test_add: imm and reg ok!\n";
}

void test_add3() {
  AssemblerX8664 a;
  a.add(rcx, r9);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_RCX);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_R9);

  delete ud;
  cout << "test_add: reg and extended reg ok!\n";
}

void test_add4() {
  AssemblerX8664 a;
  a.add(r14, r9);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_R14);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_R9);

  delete ud;
  cout << "test_add: extended reg and extended reg ok!\n";
}

void test_add5() {
  AssemblerX8664 a;
  a.add(rcx, rax);
  ud_t *ud = a.disassemble();

  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_EAX);

  delete ud;
  cout << "test_add: reg and reg ok!\n";
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
  test_mov10();
  test_mov11();
  test_push1();
  test_push2();
  test_push3();
  test_push4();
  test_pop1();
  test_pop2();
  test_lea1();
  test_lea2();

  test_sub1();
  test_sub2();

  test_add1();
  test_add2();
  test_add3();
  test_add4();
  test_add5();
}

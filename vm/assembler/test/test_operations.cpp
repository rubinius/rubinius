#include <iostream>

#include "operations.hpp"

using namespace std;
using namespace assembler_x86;
using namespace operations;

#define assert_kind(what) assert(ud->mnemonic == what)
#define assert_op(which, field, val) assert(ud->operand[which].field == val)

void test_push() {
  AssemblerX86 a;
  StackOperations s(a, esi);

  s.push(0x16);

  ud_t *ud = a.disassemble();

  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 4);

  ud_disassemble(ud);

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.udword, 0);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 0x16);

  delete ud;
  cout << "test_push: imm ok!\n";
}

void test_push2() {
  AssemblerX86 a;
  StackOperations s(a, esi);

  s.push(eax);

  ud_t *ud = a.disassemble();

  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 4);

  ud_disassemble(ud);

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.udword, 0);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_EAX);

  delete ud;
  cout << "test_push: reg ok!\n";
}

void test_pop() {
  AssemblerX86 a;
  StackOperations s(a, esi);

  s.pop();

  ud_t *ud = a.disassemble();

  assert_kind(UD_Isub);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 4);

  delete ud;
  cout << "test_pop: ok!\n";
}

void test_load_nth() {
  AssemblerX86 a;
  StackOperations s(a, esi);

  s.load_nth(eax, 0);
  s.load_nth(ebx, 3);

  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EAX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, lval.udword, 0);

  ud_disassemble(ud);

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, lval.sbyte, -12);

  delete ud;

  cout << "test_load_nth: ok!\n";
}

void test_clear() {
  AssemblerX86 a;
  StackOperations s(a, esi);

  s.clear(2);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Isub);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 8);

  delete ud;
  cout << "test_clear: ok!\n";
}

void test_set_top1() {
  AssemblerX86 a;
  StackOperations s(a, esi);

  s.set_top(0x16);

  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.udword, 0);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 0x16);

  delete ud;
  cout << "test_set_top: imm ok!\n";
}

void test_set_top2() {
  AssemblerX86 a;
  StackOperations s(a, esi);

  s.set_top(eax);

  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.udword, 0);

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_EAX);

  delete ud;
  cout << "test_set_top: reg ok!\n";
}

void test_check_both_fixnum() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  AssemblerX86::NearJumpLocation are_not;

  ops.check_both_fixnum(are_not);
  a.set_label(are_not);

  ud_t *ud = a.disassemble();

  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, lval.udword, 0);

  ud_disassemble(ud);

  assert_kind(UD_Iand);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, lval.sbyte, -4);

  ud_disassemble(ud);

  assert_kind(UD_Itest);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, TAG_FIXNUM);

  ud_disassemble(ud);

  assert_kind(UD_Ijnz);

  delete ud;

  cout << "test_check_both_fixnum: ok!\n";
}

void test_load_stack_pointer() {
  AssemblerX86 a;
  StackOperations s(a, ebx);
  ObjectOperations ops(s);

  ops.load_stack_pointer();

  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);
  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_EBP);
  assert_op(1, lval.udword, 16);

  ud_disassemble(ud);
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_EBX);
  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, lval.udword, FIELD_OFFSET(rubinius::MethodContext, js.stack));

  delete ud;

  cout << "test_load_stack_pointer: ok!\n";
}

void test_save_stack_pointer() {
  AssemblerX86 a;
  StackOperations s(a, ebx);
  ObjectOperations ops(s);

  ops.save_stack_pointer();

  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);
  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_EBP);
  assert_op(1, lval.udword, 16);

  ud_disassemble(ud);
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.udword, FIELD_OFFSET(rubinius::MethodContext, js.stack));
  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_EBX);

  delete ud;

  cout << "test_save_stack_pointer: ok!\n";
}
void test_prologue() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.prologue();

  // Pulled in from test32:test_prologue
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

  // seed the stack with the operation arguments so we don't have
  // to push them over and over again
  ud_disassemble(ud);
  assert_kind(UD_Isub);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESP);
  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 0x14); // 0x14 is enough for 4 extra args

  // arg 3
  ud_disassemble(ud);
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_EBP);
  assert_op(0, lval.udword, 16);

  // arg 2
  ud_disassemble(ud);
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_EBP);
  assert_op(0, lval.udword, 12);

  // arg 1
  ud_disassemble(ud);
  assert_kind(UD_Ipush);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_EBP);
  assert_op(0, lval.udword, 8);

  delete ud;
  cout << "test_prologue: ok!\n";
}

void test_epilogue() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.epilogue();

  ud_t *ud = a.disassemble();

  // Remove the op args we pushed in the prologue
  assert_kind(UD_Iadd);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESP);
  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, 0x20);

  // Copied from test32.cpp:test_epilogue
  ud_disassemble(ud);
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

void test_op1() {
  1 + 1;
}

void test_call_operation1() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.call_operation((void*)&test_op1, "test_op1");
  ud_t *ud = a.disassemble();

  assert_kind(UD_Icall);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.udword, (uintptr_t)test_op1 - (uintptr_t)a.pc());

  cout << "test_call_operation: no args ok!\n";
}

void test_call_operation2() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.call_operation((void*)&test_op1, "test_op1", 0x47);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESP);
  assert_op(0, lval.udword, 12);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 0x47);

  ud_disassemble(ud);
  assert_kind(UD_Icall);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.udword, (uintptr_t)test_op1 - (uintptr_t)a.pc());

  delete ud;
  cout << "test_call_operation: one arg ok!\n";
}

void test_call_operation3() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.call_operation((void*)&test_op1, "test_op1", 0x47, 0x22);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESP);
  assert_op(0, lval.udword, 12);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 0x47);

  ud_disassemble(ud);
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESP);
  assert_op(0, lval.udword, 16);

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, base, UD_NONE);
  assert_op(1, lval.udword, 0x22);

  ud_disassemble(ud);
  assert_kind(UD_Icall);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.udword, (uintptr_t)test_op1- (uintptr_t)a.pc());

  delete ud;
  cout << "test_call_operation: two args ok!\n";
}

void test_store_virtual_ip() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.store_virtual_ip(ecx);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_EBP);
  assert_op(1, lval.udword, 16);

  ud_disassemble(ud);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.udword, FIELD_OFFSET(rubinius::MethodContext, ip));

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_ECX);

  delete ud;
  cout << "test_store_virtual_ip: ok! (offset=" <<
    FIELD_OFFSET(rubinius::MethodContext, ip) << ")\n";
}

void test_store_native_ip() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.store_native_ip(ecx);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_EBP);
  assert_op(1, lval.udword, 16);

  ud_disassemble(ud);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.udword, FIELD_OFFSET(rubinius::MethodContext, native_ip));

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_ECX);

  delete ud;
  cout << "test_store_native_ip: ok!\n";
}

void test_load_native_ip() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.load_native_ip(ecx);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_EBP);
  assert_op(1, lval.udword, 16);

  ud_disassemble(ud);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, lval.udword, FIELD_OFFSET(rubinius::MethodContext, native_ip));

  delete ud;
  cout << "test_load_native_ip: ok!\n";
}

void test_store_ip() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.store_ip(ecx, edx);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_EBP);
  assert_op(1, lval.udword, 16);

  ud_disassemble(ud);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.udword, FIELD_OFFSET(rubinius::MethodContext, ip));

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_ECX);

  ud_disassemble(ud);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.udword, FIELD_OFFSET(rubinius::MethodContext, native_ip));

  assert_op(1, type, UD_OP_REG);
  assert_op(1, base, UD_R_EDX);

  delete ud;
  cout << "test_store_ip: ok!\n";
}

void test_store_call_flags() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.store_call_flags(1);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_EBP);
  assert_op(1, lval.udword, 12);

  ud_disassemble(ud);
  assert_op(0, type, UD_OP_MEM);
  assert_op(0, base, UD_R_ESI);
  assert_op(0, lval.udword, FIELD_OFFSET(rubinius::Task, call_flags));

  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, 1);


  delete ud;
  cout << "test_store_call_flags: ok!\n";
}

void test_load_self() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.load_self(ecx);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_EBP);
  assert_op(1, lval.udword, 16);

  ud_disassemble(ud);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, lval.udword, FIELD_OFFSET(rubinius::MethodContext, self_));

  delete ud;
  cout << "test_load_self: ok!\n";
}

void test_jump_if_true() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  AssemblerX86::NearJumpLocation label;
  ops.jump_if_true(ecx, label);
  a.set_label(label);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Iand);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);
  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, FALSE_MASK);

  ud_disassemble(ud);
  assert_kind(UD_Icmp);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);
  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, rubinius::cFalse);

  ud_disassemble(ud);
  assert_kind(UD_Ijnz);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.udword, (uintptr_t)label.destination() - (uintptr_t)a.pc());

  delete ud;
  cout << "test_jump_if_true: ok!\n";
}

void test_jump_if_false() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  AssemblerX86::NearJumpLocation label;
  ops.jump_if_false(ecx, label);
  a.set_label(label);

  ud_t *ud = a.disassemble();
  assert_kind(UD_Iand);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);
  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, FALSE_MASK);

  ud_disassemble(ud);
  assert_kind(UD_Icmp);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);
  assert_op(1, type, UD_OP_IMM);
  assert_op(1, lval.udword, rubinius::cFalse);

  ud_disassemble(ud);
  assert_kind(UD_Ijz);
  assert_op(0, type, UD_OP_JIMM);
  assert_op(0, base, UD_NONE);
  assert_op(0, lval.udword, (uintptr_t)label.destination() - (uintptr_t)a.pc());

  delete ud;
  cout << "test_jump_if_false: ok!\n";
}

void test_esi_value_is_remembered() {
  AssemblerX86 a;
  StackOperations s(a, esi);
  ObjectOperations ops(s);

  ops.load_self(ecx);
  ops.load_self(ecx);
  ud_t *ud = a.disassemble();
  assert_kind(UD_Imov);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ESI);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_EBP);
  assert_op(1, lval.udword, 16);

  ud_disassemble(ud);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, lval.udword, FIELD_OFFSET(rubinius::MethodContext, self_));

  ud_disassemble(ud);
  assert_op(0, type, UD_OP_REG);
  assert_op(0, base, UD_R_ECX);

  assert_op(1, type, UD_OP_MEM);
  assert_op(1, base, UD_R_ESI);
  assert_op(1, lval.udword, FIELD_OFFSET(rubinius::MethodContext, self_));

  delete ud;
  cout << "test_esi_value_is_remembered: ok!\n";
}


int main(int argc, char** argv) {
  test_push();
  test_push2();
  test_pop();
  test_load_nth();
  test_clear();

  test_set_top1();
  test_set_top2();

  test_load_stack_pointer();
  test_save_stack_pointer();
  test_check_both_fixnum();

  test_prologue();
  test_epilogue();

  test_call_operation1();
  test_call_operation2();
  test_call_operation3();

  test_store_virtual_ip();
  test_store_native_ip();
  test_load_native_ip();

  test_jump_if_true();
  test_jump_if_false();

  test_load_self();

  test_store_call_flags();
  test_esi_value_is_remembered();
}

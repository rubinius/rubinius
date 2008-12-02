#include <iostream>
#include "vm.hpp"
#include "assembler_x86.hpp"

#include <vector>

using namespace std;
using namespace assembler_x86;

void RegisterVM::run(int* op) {
  int dest, imm, ip, reg, reg2, ins;
  int registers[32];

  ip = 0;
  for(;;) {
    ins = op[ip++];
    // cout << "ip=" << ip << " ins=" << ins << "\n";
    switch(ins) {
    case add:
      dest = op[ip++];
      imm =  op[ip++];
      registers[dest] = registers[dest] + imm;
      break;

    case set:
      dest = op[ip++];
      imm =  op[ip++];
      registers[dest] = imm;
      break;

    case show:
      reg = op[ip++];
      cout << "=> " << registers[reg] << "\n";
      break;

    case exit:
      return;

    case jump:
      dest = op[ip++];
      ip = dest;
      break;

    case jump_if_equal:
      reg =  op[ip++];
      reg2 = op[ip++];
      dest = op[ip++];

      if(registers[reg] == registers[reg2]) ip = dest;
      break;
    }
  }
}

static void _show(int val) {
  cout << "=> " << val << "\n";
}

RegisterVM::Function RegisterVM::compile(int* op, int size) {
  int dest, imm, ip, reg, reg2, ins;
  AssemblerX86 a;

  std::vector<AssemblerX86::NearJumpLocation> labels(size);

  std::cout << "compiling...\n";

  int stack = a.prologue(0);

  // register table
  a.load_arg(ebx, 0);

  for(size_t ip = 0; ip < size;) {
    a.set_label(labels[ip]);

    int ins = op[ip++];
    switch(ins) {
    case add:
      dest = op[ip++];
      imm =  op[ip++];

      a.add(a.address(ebx, dest * 4), imm);
      break;

    case set:
      dest = op[ip++];
      imm =  op[ip++];

      a.mov(a.address(ebx, dest * 4), imm);
      break;

    case show:
      reg = op[ip++];

      a.start_call(1);
      a.push(a.address(ebx, reg * 4));
      a.call((void*)&_show);
      a.end_call(1);
      break;

    case exit:
      a.epilogue();
      break;

    case jump:
      dest = op[ip++];
      a.jump(labels[dest]);
      break;

    case jump_if_equal:
      reg =  op[ip++];
      reg2 = op[ip++];
      dest = op[ip++];

      a.mov(eax, a.address(ebx, reg * 4));
      a.mov(ecx, a.address(ebx, reg2 * 4));
      a.cmp(eax, ecx);
      a.jump_if_equal(labels[dest]);
      break;
    }
  }

  a.show();
  return (Function)a.buffer();
}

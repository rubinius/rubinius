#include <iostream>

#include <stdint.h>
#include <stdio.h>

#include "assembler_x86.hpp"

using namespace std;
using namespace assembler_x86;

struct state_cont {
  void* continuation;
};

typedef int (*ptr_only)(struct state_cont*, int off);
typedef int (*int_only)();
typedef int (*int_arg1)(const char* data);
typedef void* (*ptr_add)(struct state*);

extern "C" void show_int(int val) {
  std::cout << "show_int => " << val << "\n";
}

struct state {
  int *top;
  int *bottom;
};

int main4(int argc, char** argv) {
  AssemblerX86 a;
  int s = a.prologue(0);
  a.load_arg(ebx, 0);
  a.mov(esi, a.address(ebx, offsetof(struct state, top)));
  a.lea(esi, esi, 4);
  // a.add(esi, 4);
  a.mov(a.address(esi), 47);
  // a.mov_to_memory(ebx, offsetof(struct state, top), esi);

  a.lea(esi, esi, 4);
  a.mov(a.address(esi), 57);
  a.mov(a.address(ebx, offsetof(struct state, top)), esi);

  a.epilogue(s);

  a.show();
  ptr_add func = (ptr_add)a.buffer();

  struct state st;
  st.top = new int[10];
  st.bottom = st.top;

  cout << "top=" << st.top << "\n";
  void *out = func(&st);
  cout << "top=" << st.top << "\n";

  cout << "out=" << out << ", top=" << *st.top << "\n";

  cout << st.bottom[1] << "\n";
}

int main(int argc, char** argv) {
  AssemblerX86 a;
  int s = a.prologue(0);

  AssemblerX86::NearJumpLocation retrn;

  a.load_arg(ebx, 0);
  a.mov(ecx, a.address(ebx, offsetof(struct state_cont, continuation)));
  a.cmp(ecx, 0);

  AssemblerX86::NearJumpLocation normal;
  a.jump_if_equal(normal);
  a.jump(ecx);

  a.set_label(normal);

  AssemblerX86::FuturePosition loc;
  a.mov(eax, loc);
  a.mov(a.address(ebx, offsetof(struct state_cont, continuation)), eax);
  a.mov(eax, 0);

  a.jump(retrn);
  loc.update(a.pc());

  a.mov(eax, 47);
  a.load_arg(ecx, 1);
  a.add(eax, ecx);

  a.set_label(retrn);
  a.epilogue(s);

  a.show();

  struct state_cont cont;
  cont.continuation = 0;

  ptr_only func = (ptr_only)a.buffer();

  int out = func(&cont, 0);
  cout << "cont = " << cont.continuation << ", out = " << out << "\n";
  cout << "out  = " << func(&cont, 10) << "\n";
  return 0;
}

int main3(int argc, char** argv) {
  AssemblerX86 a;
  a.prologue(0);
  a.mov(eax, 0x7fffffff);
  a.add(eax, 10);

  AssemblerX86::NearJumpLocation of;
  a.jump_if_overflow(of);
  a.mov(eax, 0);
  a.epilogue(10);

  a.set_label(of);

  a.start_call(1);
  a.push(eax);
  a.call((void*)&show_int);
  a.end_call(1);

  a.sub(esp, 8);
  a.mov(eax, 1);
  a.epilogue(0);

  a.show();

  int_only func = (int_only)a.buffer();
  return func();
}

int main2(int argc, char** argv) {
  AssemblerX86 a;
  a.prologue(0);

  a.mov(edi, 5);
  AssemblerX86::NearJumpLocation top;
  a.set_label(top);

  AssemblerX86::NearJumpLocation done;
  a.cmp(edi, 0);
  a.jump_if_equal(done);

  a.dec(edi);

  a.mov(eax, a.address(ebp, 8));
  a.push(eax);
  a.call((void*)&puts);

  a.jump(top);

  a.set_label(done);
  a.mov(eax, 47);
  a.epilogue(0);

  a.show();
  //int_only func = (int_only)a.buffer();
  int_arg1 func = (int_arg1)a.buffer();
  return func("hello world2!");
}

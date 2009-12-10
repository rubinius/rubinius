#ifndef RBX_LLVM_OFFSET_HPP
#define RBX_LLVM_OFFSET_HPP

namespace offset {
  const static int cf_previous = 0;
  const static int cf_static_scope = 1;
  const static int cf_msg = 2;
  const static int cf_cm = 3;
  const static int cf_flags = 4;
  const static int cf_ip = 5;
  const static int cf_top_scope = 6;
  const static int cf_scope = 7;
  const static int cf_stk = 8;

  const static int args_recv = 0;
  const static int args_block = 1;
  const static int args_total = 2;
  const static int args_ary = 3;

  const static int vars_on_heap = 0;
  const static int vars_parent = 1;
  const static int vars_self = 2;
  const static int vars_block = 3;
  const static int vars_module = 4;
  const static int vars_tuple = 5;

  const static int varscope_block = 1;
  const static int varscope_method = 2;
  const static int varscope_module = 3;
  const static int varscope_parent = 4;
  const static int varscope_self = 5;
  const static int varscope_num_locals = 6;
  const static int varscope_isolated = 7;
  const static int varscope_locals = 8;
  const static int varscope_heap_locals = 9;

  const static int tuple_full_size = 1;
  const static int tuple_field = 2;

  const static int cm_literals = 13;
  const static int cm_static_scope = 11;

  const static int msg_name = 0;
  const static int msg_module = 1;
  const static int msg_method = 2;
  const static int msg_method_missing = 3;

  const static int blockinv_flags = 0;
  const static int blockinv_self = 1;
  const static int blockinv_static_scope = 2;
  const static int blockinv_module = 3;

  const static int blockenv_scope = 1;
  const static int blockenv_top_scope = 2;
  const static int blockenv_local_count = 3;
  const static int blockenv_method = 4;
  const static int blockenv_module = 5;
  const static int blockenv_vmm = 6;
  const static int blockenv_execute = 7;
};

#endif

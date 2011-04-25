#ifndef RBX_LLVM_OFFSET_HPP
#define RBX_LLVM_OFFSET_HPP

namespace offset {
  const static int cf_previous = 0;
  const static int cf_static_scope = 1;
  const static int cf_msg = 2;
  const static int cf_cm = 3;
  const static int cf_flags = 4;
  const static int cf_ip = 5;
  const static int cf_jit_data = 6;
  const static int cf_top_scope = 7;
  const static int cf_scope = 8;
  const static int cf_arguments = 9;
  const static int cf_stk = 10;

  const static int args_recv = 0;
  const static int args_block = 1;
  const static int args_total = 2;
  const static int args_ary = 3;
  const static int args_container = 4;

  const static int vars_on_heap = 0;
  const static int vars_parent = 1;
  const static int vars_self = 2;
  const static int vars_block = 3;
  const static int vars_module = 4;
  const static int vars_last_match = 5;
  const static int vars_tuple = 6;

  const static int varscope_block = 1;
  const static int varscope_method = 2;
  const static int varscope_module = 3;
  const static int varscope_parent = 4;
  const static int varscope_heap_locals = 5;
  const static int varscope_self = 6;
  const static int varscope_num_locals = 7;
  const static int varscope_isolated = 8;
  const static int varscope_locals = 9;

  const static int tuple_full_size = 1;
  const static int tuple_field = 2;

  const static int cm_backend_method = 14;
  const static int cm_jit_data = 15;
  const static int cm_literals = 16;
  const static int cm_static_scope = 12;

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
  const static int blockenv_code = 3;
  const static int blockenv_module = 4;
  const static int blockenv_vmm = 5;
  const static int blockenv_execute = 6;

  const static int runtime_data_method = 0;
  const static int runtime_data_name = 1;
  const static int runtime_data_module = 2;

  const static int class_class_id = 4;

  const static int ic_execute = 4;
};

#endif

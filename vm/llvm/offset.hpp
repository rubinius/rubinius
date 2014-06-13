#ifndef RBX_LLVM_OFFSET_HPP
#define RBX_LLVM_OFFSET_HPP

namespace offset {
namespace Arguments {
  const static int name = 0;
  const static int recv = 1;
  const static int block = 2;
  const static int total = 3;
  const static int arguments = 4;
  const static int argument_container = 5;
}
namespace Array {
  const static int header = 0;
  const static int total = 1;
  const static int tuple = 2;
  const static int start = 3;
}
namespace BlockEnvironment {
  const static int header = 0;
  const static int scope = 1;
  const static int top_scope = 2;
  const static int code = 3;
  const static int constant_scope = 4;
  const static int module = 5;
}
namespace BlockInvocation {
  const static int flags = 0;
  const static int self = 1;
  const static int constant_scope = 2;
  const static int module = 3;
}
namespace ByteArray {
  const static int header = 0;
  const static int full_size = 1;
  const static int field = 2;
}
namespace CallFrame {
  const static int previous = 0;
  const static int constant_scope = 1;
  const static int dispatch_data = 2;
  const static int compiled_code = 3;
  const static int flags = 4;
  const static int ip = 5;
  const static int jit_data = 6;
  const static int top_scope = 7;
  const static int scope = 8;
  const static int arguments = 9;
  const static int stk = 10;
}
namespace CallSite {
  const static int header = 0;
  const static int name = 1;
  const static int executor = 2;
  const static int fallback = 3;
  const static int updater = 4;
  const static int executable = 5;
  const static int ip = 6;
}
namespace Class {
  const static int header = 0;
  const static int instance_type = 1;
  const static int packed_ivar_info = 2;
  const static int type_info = 3;
  const static int class_id = 4;
  const static int serial_id = 5;
  const static int packed_size = 6;
}
namespace CompiledCode {
  const static int header = 0;
  const static int metadata = 1;
  const static int name = 2;
  const static int iseq = 3;
  const static int stack_size = 4;
  const static int local_count = 5;
  const static int required_args = 6;
  const static int post_args = 7;
  const static int total_args = 8;
  const static int splat = 9;
  const static int lines = 10;
  const static int local_names = 11;
  const static int file = 12;
  const static int scope = 13;
  const static int keywords = 14;
  const static int arity = 15;
  const static int breakpoints = 16;
  const static int machine_code = 17;
  const static int jit_data = 18;
  const static int literals = 19;
}
namespace ConstantCache {
  const static int header = 0;
  const static int name = 1;
  const static int value = 2;
  const static int under = 3;
  const static int scope = 4;
  const static int executable = 5;
  const static int ip = 6;
  const static int serial = 7;
}
namespace Dispatch {
  const static int name = 0;
  const static int module = 1;
  const static int method = 2;
  const static int visibility = 3;
  const static int method_missing = 4;
}
namespace Executable {
  const static int header = 0;
  const static int primitive = 1;
  const static int serial = 2;
  const static int execute = 3;
  const static int inliners = 4;
  const static int prim_index = 5;
  const static int custom_call_site = 6;
}
namespace Float {
  const static int header = 0;
  const static int val = 1;
}
namespace HeaderWord {
  const static int flags64 = 0;
}
namespace InlineCacheEntry {
  const static int header = 0;
  const static int stored_module = 1;
  const static int receiver_class = 2;
  const static int method = 3;
  const static int receiver_class_id = 4;
  const static int receiver_serial_id = 5;
  const static int method_missing = 6;
  const static int hits = 7;
}
namespace Module {
  const static int header = 0;
  const static int method_table = 1;
  const static int module_name = 2;
  const static int constants = 3;
  const static int superclass = 4;
  const static int origin = 5;
  const static int seen_ivars = 6;
  const static int mirror = 7;
  const static int subclasses = 8;
}
namespace MonoInlineCache {
  const static int header = 0;
  const static int receiver_class_id = 1;
  const static int receiver_serial_id = 2;
  const static int receiver_class = 3;
  const static int stored_module = 4;
  const static int method = 5;
  const static int method_missing = 6;
  const static int hits = 7;
}
namespace Numeric {
  const static int header = 0;
}
namespace Object {
  const static int header = 0;
}
namespace ObjectFlags {
  const static int flags = 0;
  const static int aux_word = 1;
}
namespace ObjectHeader {
  const static int header = 0;
  const static int klass = 1;
  const static int ivars = 2;
  const static int body = 3;
}
namespace PolyInlineCache {
  const static int header = 0;
  const static int entries = 1;
  const static int seen_classes_overflow = 2;
}
namespace Proc {
  const static int block = 0;
  const static int lambda = 1;
  const static int bound_method = 2;
}
namespace StackVariables {
  const static int on_heap = 0;
  const static int parent = 1;
  const static int self = 2;
  const static int block = 3;
  const static int module = 4;
  const static int last_match = 5;
  const static int locals = 6;
}
namespace State {
  const static int vm = 0;
  const static int vm_jit = 1;
  const static int shared = 2;
}
namespace Tuple {
  const static int header = 0;
  const static int full_size = 1;
  const static int field = 2;
}
namespace UnwindInfo {
  const static int target_ip = 0;
  const static int stack_depth = 1;
  const static int type = 2;
}
namespace VMJIT {
  const static int stack_start = 0;
  const static int stack_limit = 1;
  const static int stack_size = 2;
  const static int interrupt_with_signal = 3;
  const static int interrupt_by_kill = 4;
  const static int check_local_interrupts = 5;
  const static int thread_step = 6;
}
namespace VariableScope {
  const static int header = 0;
  const static int block = 1;
  const static int method = 2;
  const static int module = 3;
  const static int parent = 4;
  const static int heap_locals = 5;
  const static int dynamic_locals = 6;
  const static int last_match = 7;
  const static int fiber = 8;
  const static int self = 9;
  const static int locals = 10;
  const static int number_of_locals = 11;
  const static int isolated = 12;
  const static int flags = 13;
}
namespace jit_RuntimeData {
  const static int method = 0;
  const static int name = 1;
  const static int module = 2;
}
}
#endif
